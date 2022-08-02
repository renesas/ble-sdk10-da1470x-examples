/**
 ****************************************************************************************
 *
 * @file metrics.c
 *
 * @brief Performance Metrics module
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include "metrics.h"
#include <string.h>
#include "osal.h"

#define METRICS_MAX               (1000)
#define METRICS_TAG_MAX           (10)
#define MAX_OS_TASK_NUM           (10)
#define GUI_THREAD_NAME           ("GUI thread")

static struct {
        METRICS data[METRICS_MAX];
        uint16_t data_count;
        const char *tag_names[METRICS_TAG_MAX];
        uint32_t cpu_usage[METRICS_TAG_MAX];
} metrics;
static TaskStatus_t task_status_array[MAX_OS_TASK_NUM];
static uint32_t gui_runtime_start, gui_runtime_end;
static uint32_t total_runtime_start, total_runtime_end;
volatile uint8_t current_tag;
volatile uint8_t gpu_current_tag;

static void get_GUI_thread_CPU_time(uint32_t *task_time, uint32_t *runtime)
{
        uint8_t i;
        uint8_t os_task_count;

        os_task_count = uxTaskGetSystemState(task_status_array, ARRAY_LENGTH(task_status_array), runtime);
        for (i = 0; i < os_task_count; i++) {
                if (strcmp(task_status_array[i].pcTaskName, GUI_THREAD_NAME) == 0) {
                        *task_time = task_status_array[i].ulRunTimeCounter;
                        break;
                }
        }
}

static uint32_t calculate_cpu_usage(uint32_t task_start, uint32_t task_end, uint32_t runtime_start, uint32_t runtime_end)
{
        return ((task_end - task_start) * 100UL) / (runtime_end - runtime_start);
}

void metrics_init(void)
{
        metrics.data_count = 0;
        memset(metrics.tag_names, 0, METRICS_TAG_MAX * sizeof(char*));
        memset(metrics.data, 0, METRICS_MAX * sizeof(METRICS));
        memset(metrics.cpu_usage, 0, METRICS_TAG_MAX * sizeof(uint32_t));

        metrics_register_tag(METRICS_TAG_TICK_ROTATION, "Watch face tick rotation");
        metrics_register_tag(METRICS_TAG_SLIDING_WATCH_FACE_TO_MENU, "Sliding from watch face to menu screen");
        metrics_register_tag(METRICS_TAG_BROWSE_MENU, "Browse in menu");
        metrics_register_tag(METRICS_TAG_TIMER_RUNNING, "Timer running");
        metrics_register_tag(METRICS_TAG_COMPASS_ROTATION, "Compass rotation");
}

void metrics_reinit(void)
{
        metrics_set_tag(METRICS_TAG_NO_LOGGING);
        metrics_print();
        metrics_init();
        metrics_set_tag(current_tag);
}

void metrics_add(METRICS *metric)
{
        if (current_tag) {
                memcpy(&metrics.data[metrics.data_count], metric, sizeof(METRICS));
                metrics.data[metrics.data_count].tag = current_tag;
                metrics.data_count++;
                if (metrics.data_count == METRICS_MAX) {
                        metrics_reinit();
                }
        }
}

void metrics_set_tag(uint8_t tag)
{
        if ((current_tag != METRICS_TAG_NO_LOGGING) && (tag == METRICS_TAG_NO_LOGGING)) {
                get_GUI_thread_CPU_time(&gui_runtime_end, &total_runtime_end);
                metrics.cpu_usage[current_tag] = calculate_cpu_usage(gui_runtime_start,
                                                                     gui_runtime_end,
                                                                     total_runtime_start,
                                                                     total_runtime_end);
        } else if (tag != METRICS_TAG_NO_LOGGING) {
                get_GUI_thread_CPU_time(&gui_runtime_start, &total_runtime_start);
        }
        current_tag = tag;
}

METRICS get_metrics_data()
{
        return metrics.data[metrics.data_count];
}

void metrics_gpu_add(int gpu_rendering_time)
{
        if (current_tag && gpu_current_tag) {
                metrics.data[metrics.data_count].gpu_data[gpu_current_tag-1] += gpu_rendering_time;
        }
}

void metrics_set_gpu_tag(uint8_t tag)
{
        gpu_current_tag = tag;
}

void metrics_register_tag(uint8_t tag, const char *tag_name)
{
        if (tag < METRICS_TAG_MAX) {
                metrics.tag_names[tag] = tag_name;
        }
}

void metrics_print(void)
{
        uint16_t i;
        uint8_t tag = 0;
        int fps_total[4];
        int rendering_count = 0;
        int pixel_rate_total = 0;

        int gpu_total_values_per_tag[GPU_METRICS_MAX_TAG];
        int gpu_valid_values_per_tag[GPU_METRICS_MAX_TAG];
        int gpu_avg_values_per_tag[GPU_METRICS_MAX_TAG];

        printf("\r\nPerformance metrics:\r\n");
        for (i = 0; i < metrics.data_count; i++) {
                if (metrics.data[i].tag != tag) {
                        tag = metrics.data[i].tag;
                        printf("\r\n%s, CPU: %ld%%\r\n", metrics.tag_names[tag], metrics.cpu_usage[tag]);
                        memset(fps_total, 0, 4 * sizeof(int));
                        rendering_count = 0;
                        pixel_rate_total = 0;

                        memset(gpu_total_values_per_tag, 0, sizeof(gpu_total_values_per_tag));
                        memset(gpu_valid_values_per_tag, 0, sizeof(gpu_valid_values_per_tag));
                        memset(gpu_avg_values_per_tag, 0, sizeof(gpu_avg_values_per_tag));
                }

                fps_total[0] += metrics.data[i].fps;
                if (metrics.data[i].frame_rendering_time) {
                        rendering_count++;
                }
                fps_total[1] += metrics.data[i].frame_rendering_time;
                fps_total[2] += metrics.data[i].display_transfer_time;
                fps_total[3]++; //counts the number of samples per metric tag
                pixel_rate_total += (metrics.data[i].pixel_count * 1000) / metrics.data[i].display_transfer_time;


                for (uint8_t gpu_tag = 1; gpu_tag < GPU_METRICS_MAX_TAG + 1; gpu_tag++) {
                        if (metrics.data[i].gpu_data[gpu_tag - 1]) {
                                gpu_total_values_per_tag[gpu_tag - 1] += metrics.data[i].gpu_data[gpu_tag - 1];
                                gpu_valid_values_per_tag[gpu_tag - 1]++;
                        }
                }

                if (metrics.data[i+1].tag != tag) {
                        for (uint8_t gpu_tag = 1; gpu_tag < GPU_METRICS_MAX_TAG + 1; gpu_tag++) {
                                gpu_avg_values_per_tag[gpu_tag - 1] = gpu_total_values_per_tag[gpu_tag - 1] / gpu_valid_values_per_tag[gpu_tag - 1];
                        }

                        printf("Average GPU: Fill: %10d.%.2d ms,\r\n"
                               "             BlitBitmap: %4d.%.2d ms,\r\n"
                               "             RotateImage: %3d.%.2d ms,\r\n",
                                (gpu_avg_values_per_tag[0]) / 1000, ((gpu_avg_values_per_tag[0]) / 10) % 100,
                                (gpu_avg_values_per_tag[1]) / 1000, ((gpu_avg_values_per_tag[1]) / 10) % 100,
                                (gpu_avg_values_per_tag[2]) / 1000, ((gpu_avg_values_per_tag[2]) / 10) % 100);

                        printf("Average FPS: %3d.%d (frame: %3d.%.2d ms, transfer: %3d.%.2d ms), Pixel Rate = %3d.%.2d kP/sec\r\n\r\n",
                                (fps_total[0] / fps_total[3]) / 10, (fps_total[0] / fps_total[3]) % 10,
                                (fps_total[1] / rendering_count) / 1000, ((fps_total[1] / rendering_count) / 10) % 100,
                                (fps_total[2] / fps_total[3]) / 1000, ((fps_total[2] / fps_total[3]) / 10) % 100,
                                (pixel_rate_total / fps_total[3]) / 1000, ((pixel_rate_total / fps_total[3]) / 10) % 100);
                }
        }
}
