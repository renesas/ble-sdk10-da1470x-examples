/**
 ****************************************************************************************
 *
 * @file PortFunctions.c
 *
 * @brief
 *
 * Copyright (c) 2022 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */
#include <string.h>
#include "osal.h"
#include "PortFunction.h"

#ifdef INC_FREERTOS_H

void *PortMalloc(size_t size)
{
    void *p;

    p = pvPortMalloc(size);
    if(p==NULL)
    {
        __asm("BKPT #0\n");
    }
    return p;
}

void *PortCalloc(size_t num, size_t size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = pvPortMalloc(num * size);
    if (p) {
      /* zero the memory */
      memset(p, 0, num * size);
    }
    else
    {
        __asm("BKPT #0\n");
    }
    return p;
}

void *PortRealloc(void *ptr, size_t size)
{
    if (size == 0)
    {
        vPortFree(ptr);
        return NULL;
    }

    void *p;
    p = pvPortMalloc(size);
    if (p) {
        /* zero the memory */
        if (ptr != NULL) {
            memcpy(p, ptr, size);
            vPortFree(ptr);
        }
    }

    return p;
}

void  PortFree(void *ptr)
{
    vPortFree(ptr);
}

void PortDelay(uint32_t delay_in_ms)
{
    TickType_t xTicks = (delay_in_ms * configTICK_RATE_HZ + 500) / 1000;

    if (xTicks == 0)
        xTicks = 1;

    vTaskDelay(xTicks);
}

#else

void *PortMalloc(size_t size)
{
    return malloc(size);
}

void *PortCalloc(size_t num, size_t size)
{
    return calloc(num, size);
}

void *PortRealloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void  PortFree(void *ptr)
{
    free(ptr);
}

void PortDelay(uint32_t delay_in_ms)
{
    
}

#endif
