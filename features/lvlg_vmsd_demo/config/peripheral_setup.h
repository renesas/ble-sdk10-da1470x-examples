 /**
 ****************************************************************************************
 *
 * @file periph_setup.h
 *
 * @brief Peripheral Setup file.
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef PERIPH_SETUP_H_
#define PERIPH_SETUP_H_

#include "hw_gpio.h"
#include "hw_wkup.h"

#if (DEVICE_FAMILY == DA1469X)
/*
 * Include definitions for configuring the hardware blocks.
 */
#if dg_configUSE_H13TB18A

#define H13TB18A_RST_PORT                       HW_GPIO_PORT_1
#define H13TB18A_RST_PIN                        HW_GPIO_PIN_21

#define H13TB18A_SPI_CS_PORT                    HW_GPIO_PORT_1
#define H13TB18A_SPI_CS_PIN                     HW_GPIO_PIN_14

#define H13TB18A_SPI_CLK_PORT                   HW_GPIO_PORT_1
#define H13TB18A_SPI_CLK_PIN                    HW_GPIO_PIN_5

#define H13TB18A_DCX_PORT                       HW_GPIO_PORT_1
#define H13TB18A_DCX_PIN                        HW_GPIO_PIN_4

#define H13TB18A_SPI_SDA_PORT                   HW_GPIO_PORT_1
#define H13TB18A_SPI_SDA_PIN                    HW_GPIO_PIN_3

#define H13TB18A_TE_PORT                        HW_GPIO_PORT_1
#define H13TB18A_TE_PIN                         HW_GPIO_PIN_22

#endif /* dg_configUSE_H13TB18A */
#if dg_configUSE_TFT12864_57_E

#define TFT12864_57_E_SPI4_SEL_PORT             HW_GPIO_PORT_1
#define TFT12864_57_E_SPI4_SEL_PIN              HW_GPIO_PIN_3

#define TFT12864_57_E_CSB_PORT                  HW_GPIO_PORT_1
#define TFT12864_57_E_CSB_PIN                   HW_GPIO_PIN_8

#define TFT12864_57_E_RESB_PORT                 HW_GPIO_PORT_1
#define TFT12864_57_E_RESB_PIN                  HW_GPIO_PIN_4

#define TFT12864_57_E_SCL_PORT                  HW_GPIO_PORT_1
#define TFT12864_57_E_SCL_PIN                   HW_GPIO_PIN_2

#define TFT12864_57_E_DCX_PORT                  HW_GPIO_PORT_1
#define TFT12864_57_E_DCX_PIN                   HW_GPIO_PIN_7

#define TFT12864_57_E_MOSI_PORT                 HW_GPIO_PORT_1
#define TFT12864_57_E_MOSI_PIN                  HW_GPIO_PIN_5

#endif /* dg_configUSE_TFT12864_57_E */
#if dg_configUSE_LPM013M091A

#define LPM013M091A_SDI_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_SDI_PIN                     HW_GPIO_PIN_13

#define LPM013M091A_SCL_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_SCL_PIN                     HW_GPIO_PIN_17

#define LPM013M091A_DCX_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_DCX_PIN                     HW_GPIO_PIN_14

#define LPM013M091A_CSX_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_CSX_PIN                     HW_GPIO_PIN_16

#define LPM013M091A_RST_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_RST_PIN                     HW_GPIO_PIN_7

#define LPM013M091A_TE_PORT                     HW_GPIO_PORT_1
#define LPM013M091A_TE_PIN                      HW_GPIO_PIN_22

#define LPM013M091A_IM0_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_IM0_PIN                     HW_GPIO_PIN_3

#endif /* dg_configUSE_LPM013M091A */
#if dg_configUSE_LPM010M297B

#define LPM010M297B_SCLK_PORT                   HW_GPIO_PORT_1
#define LPM010M297B_SCLK_PIN                    HW_GPIO_PIN_4

#define LPM010M297B_SI_PORT                     HW_GPIO_PORT_1
#define LPM010M297B_SI_PIN                      HW_GPIO_PIN_2

#define LPM010M297B_SCS_PORT                    HW_GPIO_PORT_1
#define LPM010M297B_SCS_PIN                     HW_GPIO_PIN_7

#define LPM010M297B_EXTCOMIN_PORT               HW_GPIO_PORT_1
#define LPM010M297B_EXTCOMIN_PIN                HW_GPIO_PIN_3

#define LPM010M297B_DISP_PORT                   HW_GPIO_PORT_1
#define LPM010M297B_DISP_PIN                    HW_GPIO_PIN_8

#endif /* dg_configUSE_LPM010M297B */

#if dg_configUSE_HM80160A090
#define HM80160A090_SCK_PORT                    HW_GPIO_PORT_1
#define HM80160A090_SCK_PIN                     HW_GPIO_PIN_11

#define HM80160A090_SDA_PORT                    HW_GPIO_PORT_1
#define HM80160A090_SDA_PIN                     HW_GPIO_PIN_17

#define HM80160A090_RST_PORT                    HW_GPIO_PORT_1
#define HM80160A090_RST_PIN                     HW_GPIO_PIN_16

#define HM80160A090_DC_PORT                     HW_GPIO_PORT_1
#define HM80160A090_DC_PIN                      HW_GPIO_PIN_7

#define HM80160A090_CS_PORT                     HW_GPIO_PORT_1
#define HM80160A090_CS_PIN                      HW_GPIO_PIN_2
#endif /* dg_configUSE_HM80160A090 */

#if dg_configUSE_E1394AA65A
#define E1394AA65A_TE_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_TE_PIN                       HW_GPIO_PIN_24

#define E1394AA65A_SCK_PORT                     HW_GPIO_PORT_0
#define E1394AA65A_SCK_PIN                      HW_GPIO_PIN_26

#define E1394AA65A_DO_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_DO_PIN                       HW_GPIO_PIN_27

#define E1394AA65A_CS_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_CS_PIN                       HW_GPIO_PIN_28

#define E1394AA65A_DC_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_DC_PIN                       HW_GPIO_PIN_29

#define E1394AA65A_RST_PORT                     HW_GPIO_PORT_0
#define E1394AA65A_RST_PIN                      HW_GPIO_PIN_31
#endif /* dg_configUSE_E1394AA65A */

#if dg_configUSE_LPM012M134B
#define LPM012M134B_BLUE_0_PORT                 HW_GPIO_PORT_1
#define LPM012M134B_BLUE_0_PIN                  HW_GPIO_PIN_2

#define LPM012M134B_BLUE_1_PORT                 HW_GPIO_PORT_1
#define LPM012M134B_BLUE_1_PIN                  HW_GPIO_PIN_3

#define LPM012M134B_GREEN_0_PORT                HW_GPIO_PORT_1
#define LPM012M134B_GREEN_0_PIN                 HW_GPIO_PIN_4

#define LPM012M134B_GREEN_1_PORT                HW_GPIO_PORT_1
#define LPM012M134B_GREEN_1_PIN                 HW_GPIO_PIN_5

#define LPM012M134B_RED_0_PORT                  HW_GPIO_PORT_1
#define LPM012M134B_RED_0_PIN                   HW_GPIO_PIN_7

#define LPM012M134B_RED_1_PORT                  HW_GPIO_PORT_1
#define LPM012M134B_RED_1_PIN                   HW_GPIO_PIN_8

#define LPM012M134B_FRP_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_FRP_PIN                     HW_GPIO_PIN_10

#define LPM012M134B_XFRP_PORT                   HW_GPIO_PORT_1
#define LPM012M134B_XFRP_PIN                    HW_GPIO_PIN_11

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.26
 */
#define LPM012M134B_VCK_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_VCK_PIN                     HW_GPIO_PIN_13

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.27
 */
#define LPM012M134B_ENB_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_ENB_PIN                     HW_GPIO_PIN_14

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.28
 */
#define LPM012M134B_VST_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_VST_PIN                     HW_GPIO_PIN_15

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.29
 */
#define LPM012M134B_HCK_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_HCK_PIN                     HW_GPIO_PIN_16

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.30
 */
#define LPM012M134B_HST_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_HST_PIN                     HW_GPIO_PIN_17

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.31
 */
#define LPM012M134B_XRST_PORT                   HW_GPIO_PORT_1
#define LPM012M134B_XRST_PIN                    HW_GPIO_PIN_21

#define LPM012M134B_PEN_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_PEN_PIN                     HW_GPIO_PIN_19
#endif /* dg_configUSE_LPM012M134B */

#if dg_configUSE_NHD43480272EFASXN
#define NHD43480272EFASXN_BLUE_0_PORT           HW_GPIO_PORT_1
#define NHD43480272EFASXN_BLUE_0_PIN            HW_GPIO_PIN_2

#define NHD43480272EFASXN_BLUE_1_PORT           HW_GPIO_PORT_1
#define NHD43480272EFASXN_BLUE_1_PIN            HW_GPIO_PIN_3

#define NHD43480272EFASXN_GREEN_0_PORT          HW_GPIO_PORT_1
#define NHD43480272EFASXN_GREEN_0_PIN           HW_GPIO_PIN_4

#define NHD43480272EFASXN_GREEN_1_PORT          HW_GPIO_PORT_1
#define NHD43480272EFASXN_GREEN_1_PIN           HW_GPIO_PIN_5

#define NHD43480272EFASXN_RED_0_PORT            HW_GPIO_PORT_1
#define NHD43480272EFASXN_RED_0_PIN             HW_GPIO_PIN_7

#define NHD43480272EFASXN_RED_1_PORT            HW_GPIO_PORT_1
#define NHD43480272EFASXN_RED_1_PIN             HW_GPIO_PIN_8

#define NHD43480272EFASXN_DEN_PORT              HW_GPIO_PORT_0
#define NHD43480272EFASXN_DEN_PIN               HW_GPIO_PIN_27

#define NHD43480272EFASXN_VSYNC_PORT            HW_GPIO_PORT_0
#define NHD43480272EFASXN_VSYNC_PIN             HW_GPIO_PIN_28

#define NHD43480272EFASXN_CLK_PORT              HW_GPIO_PORT_0
#define NHD43480272EFASXN_CLK_PIN               HW_GPIO_PIN_29

#define NHD43480272EFASXN_HSYNC_PORT            HW_GPIO_PORT_0
#define NHD43480272EFASXN_HSYNC_PIN             HW_GPIO_PIN_30

#define NHD43480272EFASXN_DISP_PORT             HW_GPIO_PORT_0
#define NHD43480272EFASXN_DISP_PIN              HW_GPIO_PIN_31

#define NHD43480272EFASXN_READY_PORT            HW_GPIO_PORT_0
#define NHD43480272EFASXN_READY_PIN             HW_GPIO_PIN_10

#if dg_configUSE_FT5306
#define FT5306_SCL_PORT                         HW_GPIO_PORT_0
#define FT5306_SCL_PIN                          HW_GPIO_PIN_20

#define FT5306_SDA_PORT                         HW_GPIO_PORT_0
#define FT5306_SDA_PIN                          HW_GPIO_PIN_19
#endif /* dg_configUSE_FT5306 */

#endif /* dg_configUSE_NHD480272EF */

#if dg_configUSE_LS013B7DH06
#define LS013B7DH06_SCK_PORT                    HW_GPIO_PORT_0
#define LS013B7DH06_SCK_PIN                     HW_GPIO_PIN_28

#define LS013B7DH06_SDA_PORT                    HW_GPIO_PORT_0
#define LS013B7DH06_SDA_PIN                     HW_GPIO_PIN_29

#define LS013B7DH06_CS_PORT                     HW_GPIO_PORT_0
#define LS013B7DH06_CS_PIN                      HW_GPIO_PIN_30

#define LS013B7DH06_EXTCOMIN_PORT               HW_GPIO_PORT_1
#define LS013B7DH06_EXTCOMIN_PIN                HW_GPIO_PIN_10

#define LS013B7DH06_DISP_PORT                   HW_GPIO_PORT_0
#define LS013B7DH06_DISP_PIN                    HW_GPIO_PIN_31
#endif /* dg_configUSE_LS013B7DH06 */

#if dg_configUSE_LS013B7DH03
#define LS013B7DH03_SCK_PORT                    HW_GPIO_PORT_0
#define LS013B7DH03_SCK_PIN                     HW_GPIO_PIN_28

#define LS013B7DH03_SDA_PORT                    HW_GPIO_PORT_0
#define LS013B7DH03_SDA_PIN                     HW_GPIO_PIN_29

#define LS013B7DH03_CS_PORT                     HW_GPIO_PORT_0
#define LS013B7DH03_CS_PIN                      HW_GPIO_PIN_30

#define LS013B7DH03_EXTCOMIN_PORT               HW_GPIO_PORT_1
#define LS013B7DH03_EXTCOMIN_PIN                HW_GPIO_PIN_10

#define LS013B7DH03_DISP_PORT                   HW_GPIO_PORT_0
#define LS013B7DH03_DISP_PIN                    HW_GPIO_PIN_31
#endif /* dg_configUSE_LS013B7DH03 */

#if dg_configUSE_DT280QV10CT
#define DT280QV10CT_SDA_PORT                    HW_GPIO_PORT_0
#define DT280QV10CT_SDA_PIN                     HW_GPIO_PIN_24

#define DT280QV10CT_RST_PORT                    HW_GPIO_PORT_0
#define DT280QV10CT_RST_PIN                     HW_GPIO_PIN_12

#define DT280QV10CT_SCK_PORT                    HW_GPIO_PORT_0
#define DT280QV10CT_SCK_PIN                     HW_GPIO_PIN_27

#define DT280QV10CT_DC_PORT                     HW_GPIO_PORT_0
#define DT280QV10CT_DC_PIN                      HW_GPIO_PIN_20

#define DT280QV10CT_CS_PORT                     HW_GPIO_PORT_0
#define DT280QV10CT_CS_PIN                      HW_GPIO_PIN_21

#define DT280QV10CT_TE_PORT                     HW_GPIO_PORT_1
#define DT280QV10CT_TE_PIN                      HW_GPIO_PIN_22

/*
 * \note For the resistive touch shield the backlight is routed on P1.5
 */
#define DT280QV10CT_BL_PORT                     HW_GPIO_PORT_1
#define DT280QV10CT_BL_PIN                      HW_GPIO_PIN_8

#if dg_configUSE_FT6206
#define FT6206_SCL_PORT                         HW_GPIO_PORT_0
#define FT6206_SCL_PIN                          HW_GPIO_PIN_29

#define FT6206_SDA_PORT                         HW_GPIO_PORT_0
#define FT6206_SDA_PIN                          HW_GPIO_PIN_28
#endif /* dg_configUSE_FT6206 */

#endif /* dg_configUSE_DT280QV10CT */

#if dg_configUSE_T1D3BP006
#define T1D3BP006_CS_PORT                       HW_GPIO_PORT_0
#define T1D3BP006_CS_PIN                        HW_GPIO_PIN_20

#define T1D3BP006_SCK_PORT                      HW_GPIO_PORT_0
#define T1D3BP006_SCK_PIN                       HW_GPIO_PIN_21

#define T1D3BP006_SDA_PORT                      HW_GPIO_PORT_0
#define T1D3BP006_SDA_PIN                       HW_GPIO_PIN_30

#define T1D3BP006_RST_PORT                      HW_GPIO_PORT_0
#define T1D3BP006_RST_PIN                       HW_GPIO_PIN_31

#define T1D3BP006_TE_PORT                       HW_GPIO_PORT_0
#define T1D3BP006_TE_PIN                        HW_GPIO_PIN_24

#define T1D3BP006_DC_PORT                       HW_GPIO_PORT_0
#define T1D3BP006_DC_PIN                        HW_GPIO_PIN_26
#endif /* dg_configUSE_T1D3BP006 */

#if dg_configUSE_T1D54BP002
#define T1D54BP002_CS_PORT                      HW_GPIO_PORT_0
#define T1D54BP002_CS_PIN                       HW_GPIO_PIN_20

#define T1D54BP002_SCK_PORT                     HW_GPIO_PORT_0
#define T1D54BP002_SCK_PIN                      HW_GPIO_PIN_21

#define T1D54BP002_SDA_PORT                     HW_GPIO_PORT_0
#define T1D54BP002_SDA_PIN                      HW_GPIO_PIN_30

#define T1D54BP002_RST_PORT                     HW_GPIO_PORT_0
#define T1D54BP002_RST_PIN                      HW_GPIO_PIN_31
#endif /* dg_configUSE_T1D54BP002 */

#if dg_configUSE_PSP27801
#define PSP27801_RW_PORT                        HW_GPIO_PORT_1
#define PSP27801_RW_PIN                         HW_GPIO_PIN_9

#define PSP27801_CS_PORT                        HW_GPIO_PORT_0
#define PSP27801_CS_PIN                         HW_GPIO_PIN_20

#define PSP27801_SCK_PORT                       HW_GPIO_PORT_0
#define PSP27801_SCK_PIN                        HW_GPIO_PIN_21

#define PSP27801_SDO_PORT                       HW_GPIO_PORT_0
#define PSP27801_SDO_PIN                        HW_GPIO_PIN_24

#define PSP27801_SDI_PORT                       HW_GPIO_PORT_0
#define PSP27801_SDI_PIN                        HW_GPIO_PIN_26

#define PSP27801_DC_PORT                        HW_GPIO_PORT_1
#define PSP27801_DC_PIN                         HW_GPIO_PIN_1

#define PSP27801_EN_PORT                        HW_GPIO_PORT_0
#define PSP27801_EN_PIN                         HW_GPIO_PIN_27

#define PSP27801_RST_PORT                       HW_GPIO_PORT_0
#define PSP27801_RST_PIN                        HW_GPIO_PIN_12
#endif /* dg_configUSE_PSP27801 */

#if dg_configUSE_MCT024L6W240320PML
#define MCT024L6W240320PML_SDA_PORT             HW_GPIO_PORT_1
#define MCT024L6W240320PML_SDA_PIN              HW_GPIO_PIN_17

#define MCT024L6W240320PML_RST_PORT             HW_GPIO_PORT_1
#define MCT024L6W240320PML_RST_PIN              HW_GPIO_PIN_16

#define MCT024L6W240320PML_SCK_PORT             HW_GPIO_PORT_1
#define MCT024L6W240320PML_SCK_PIN              HW_GPIO_PIN_5

#define MCT024L6W240320PML_DC_PORT              HW_GPIO_PORT_1
#define MCT024L6W240320PML_DC_PIN               HW_GPIO_PIN_2

#define MCT024L6W240320PML_CS_PORT              HW_GPIO_PORT_1
#define MCT024L6W240320PML_CS_PIN               HW_GPIO_PIN_3
#endif /* dg_configUSE_MCT024L6W240320PML */

#if dg_configUSE_ILI9341

#define ILI9341_CS_PORT                                                 (HW_GPIO_PORT_0)
#define ILI9341_CS_PIN                                                  (HW_GPIO_PIN_30)

#define ILI9341_DC_PORT                                                 (HW_GPIO_PORT_0)
#define ILI9341_DC_PIN                                                  (HW_GPIO_PIN_29)

#define ILI9341_RST_PORT                                                (HW_GPIO_PORT_0)
#define ILI9341_RST_PIN                                                 (HW_GPIO_PIN_27)

#define ILI9341_DO_PORT                                                 (HW_GPIO_PORT_0)
#define ILI9341_DO_PIN                                                  (HW_GPIO_PIN_21)

#define ILI9341_SCK_PORT                                                (HW_GPIO_PORT_0)
#define ILI9341_SCK_PIN                                                 (HW_GPIO_PIN_20)

#endif

#if dg_configUSE_ONOC40108_SPI4
#define ONOC40108_SPI4_SD0_PORT                                           HW_GPIO_PORT_0
#define ONOC40108_SPI4_SD0_PIN                                            HW_GPIO_PIN_21

#define ONOC40108_SPI4_SCL_PORT                                           HW_GPIO_PORT_0
#define ONOC40108_SPI4_SCL_PIN                                            HW_GPIO_PIN_20

#define ONOC40108_SPI4_CSX_PORT                                           HW_GPIO_PORT_0
#define ONOC40108_SPI4_CSX_PIN                                            HW_GPIO_PIN_30

#define ONOC40108_SPI4_RST_PORT                                           HW_GPIO_PORT_0
#define ONOC40108_SPI4_RST_PIN                                            HW_GPIO_PIN_27

#define ONOC40108_SPI4_TE_PORT                                            HW_GPIO_PORT_0
#define ONOC40108_SPI4_TE_PIN                                             HW_GPIO_PIN_24
#endif

#if dg_configUSE_XSJ120TY2401_SPI3 || dg_configUSE_XSJ120TY2401_SPI4
#define XSJ120TY2401_SPI3_SD0_PORT                                      (HW_GPIO_PORT_0)
#define XSJ120TY2401_SPI3_SD0_PIN                                       (HW_GPIO_PIN_21)

#define XSJ120TY2401_SPI3_SCL_PORT                                      (HW_GPIO_PORT_0)
#define XSJ120TY2401_SPI3_SCL_PIN                                       (HW_GPIO_PIN_20)

#define XSJ120TY2401_SPI3_CSX_PORT                                      (HW_GPIO_PORT_0)
#define XSJ120TY2401_SPI3_CSX_PIN                                       (HW_GPIO_PIN_30)

#define XSJ120TY2401_SPI3_RST_PORT                                      (HW_GPIO_PORT_0)
#define XSJ120TY2401_SPI3_RST_PIN                                       (HW_GPIO_PIN_27)

#define XSJ120TY2401_SPI3_TE_PORT                                       (HW_GPIO_PORT_0)
#define XSJ120TY2401_SPI3_TE_PIN                                        (HW_GPIO_PIN_24)
#endif /* dg_configUSE_XSJ120TY2401_SPI3 || dg_configUSE_XSJ120TY2401_SPI4 */

#elif (DEVICE_FAMILY == DA1470X)
/* Quad, Dual and SPI3/4 I/O Definitions */
#define LCD_SPI_SD_PORT                                        HW_GPIO_PORT_0
#define LCD_SPI_SD_PIN                                         HW_GPIO_PIN_15

#define LCD_SPI_SD1_PORT                                       HW_GPIO_PORT_0
#define LCD_SPI_SD1_PIN                                        HW_GPIO_PIN_16

#define LCD_SPI_SD2_PORT                                       HW_GPIO_PORT_0
#define LCD_SPI_SD2_PIN                                        HW_GPIO_PIN_22

#define LCD_SPI_SD3_PORT                                       HW_GPIO_PORT_0
#define LCD_SPI_SD3_PIN                                        HW_GPIO_PIN_17

#define LCD_SPI_SI_PORT                                        HW_GPIO_PORT_0
#define LCD_SPI_SI_PIN                                         HW_GPIO_PIN_9

#define LCD_SPI_SCLK_PORT                                      HW_GPIO_PORT_0
#define LCD_SPI_SCLK_PIN                                       HW_GPIO_PIN_14

#define LCD_SPI_CS_PORT                                        HW_GPIO_PORT_0
#define LCD_SPI_CS_PIN                                         HW_GPIO_PIN_18

#define LCD_SPI_DC_PORT                                        HW_GPIO_PORT_0
#define LCD_SPI_DC_PIN                                         HW_GPIO_PIN_16

#if 0//DEVICE_FPGA
#define LCD_RST_PORT                                           HW_GPIO_PORT_0
#define LCD_RST_PIN                                            HW_GPIO_PIN_19
#else
#define LCD_RST_PORT                                           HW_GPIO_PORT_0
#define LCD_RST_PIN                                            HW_GPIO_PIN_23
#endif /* DEVICE_FPGA */

#define LCD_TE_PORT                                            HW_GPIO_PORT_0
#define LCD_TE_PIN                                             HW_GPIO_PIN_10

/* MIPI DPI-2 I/O Definitions */
#define LCD_DPI_CLK_PORT                                       HW_GPIO_PORT_0
#define LCD_DPI_CLK_PIN                                        HW_GPIO_PIN_14

#define LCD_DPI_DE_PORT                                        HW_GPIO_PORT_0
#define LCD_DPI_DE_PIN                                         HW_GPIO_PIN_18

#define LCD_DPI_HSYNC_PORT                                     HW_GPIO_PORT_0
#define LCD_DPI_HSYNC_PIN                                      HW_GPIO_PIN_15

#define LCD_DPI_VSYNC_PORT                                     HW_GPIO_PORT_0
#define LCD_DPI_VSYNC_PIN                                      HW_GPIO_PIN_16

#define LCD_DPI_SD_PORT                                        HW_GPIO_PORT_0
#define LCD_DPI_SD_PIN                                         HW_GPIO_PIN_22

#define LCD_DPI_CM_PORT                                        HW_GPIO_PORT_0
#define LCD_DPI_CM_PIN                                         HW_GPIO_PIN_9

#define LCD_DPI_RED0_PORT                                      HW_GPIO_PORT_0
#define LCD_DPI_RED0_PIN                                       HW_GPIO_PIN_17

#define LCD_DPI_RED1_PORT                                      HW_GPIO_PORT_0
#define LCD_DPI_RED1_PIN                                       HW_GPIO_PIN_23

#define LCD_DPI_GREEN0_PORT                                    HW_GPIO_PORT_0
#define LCD_DPI_GREEN0_PIN                                     HW_GPIO_PIN_24

#define LCD_DPI_GREEN1_PORT                                    HW_GPIO_PORT_1
#define LCD_DPI_GREEN1_PIN                                     HW_GPIO_PIN_0

#define LCD_DPI_BLUE0_PORT                                     HW_GPIO_PORT_1
#define LCD_DPI_BLUE0_PIN                                      HW_GPIO_PIN_1

#define LCD_DPI_BLUE1_PORT                                     HW_GPIO_PORT_0
#define LCD_DPI_BLUE1_PIN                                      HW_GPIO_PIN_21

#define LCD_DPI_READY_PORT                                     HW_GPIO_PORT_0
#define LCD_DPI_READY_PIN                                      HW_GPIO_PIN_10

/* JDI Parallel I/O Definitions */
#define LCD_JDI_HST_PORT                                       HW_GPIO_PORT_0
#define LCD_JDI_HST_PIN                                        HW_GPIO_PIN_15

#define LCD_JDI_VST_PORT                                       HW_GPIO_PORT_0
#define LCD_JDI_VST_PIN                                        HW_GPIO_PIN_16

#define LCD_JDI_HCK_PORT                                       HW_GPIO_PORT_0
#define LCD_JDI_HCK_PIN                                        HW_GPIO_PIN_14

#define LCD_JDI_VCK_PORT                                       HW_GPIO_PORT_0
#define LCD_JDI_VCK_PIN                                        HW_GPIO_PIN_9

#define LCD_JDI_RED0_PORT                                      HW_GPIO_PORT_0
#define LCD_JDI_RED0_PIN                                       HW_GPIO_PIN_17

#define LCD_JDI_RED1_PORT                                      HW_GPIO_PORT_0
#define LCD_JDI_RED1_PIN                                       HW_GPIO_PIN_23

#define LCD_JDI_GREEN0_PORT                                    HW_GPIO_PORT_0
#define LCD_JDI_GREEN0_PIN                                     HW_GPIO_PIN_24

#define LCD_JDI_GREEN1_PORT                                    HW_GPIO_PORT_1
#define LCD_JDI_GREEN1_PIN                                     HW_GPIO_PIN_0

#define LCD_JDI_BLUE0_PORT                                     HW_GPIO_PORT_1
#define LCD_JDI_BLUE0_PIN                                      HW_GPIO_PIN_1

#define LCD_JDI_BLUE1_PORT                                     HW_GPIO_PORT_0
#define LCD_JDI_BLUE1_PIN                                      HW_GPIO_PIN_21

#define LCD_JDI_ENB_PORT                                       HW_GPIO_PORT_0
#define LCD_JDI_ENB_PIN                                        HW_GPIO_PIN_18

#define LCD_JDI_XRST_PORT                                      HW_GPIO_PORT_0
#define LCD_JDI_XRST_PIN                                       HW_GPIO_PIN_22

#define LCD_JDI_FRP_PORT                                       HW_GPIO_PORT_0
#define LCD_JDI_FRP_PIN                                        HW_GPIO_PIN_19

#define LCD_JDI_XFRP_PORT                                      HW_GPIO_PORT_0
#define LCD_JDI_XFRP_PIN                                       HW_GPIO_PIN_10

/* MIPI DBI Type B I/O Definitions */
#define LCD_DBIB_CSX_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_CSX_PIN                                       HW_GPIO_PIN_14

#define LCD_DBIB_RESX_PORT                                     HW_GPIO_PORT_0
#define LCD_DBIB_RESX_PIN                                      HW_GPIO_PIN_18

#define LCD_DBIB_DCX_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_DCX_PIN                                       HW_GPIO_PIN_15

#define LCD_DBIB_WRX_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_WRX_PIN                                       HW_GPIO_PIN_16

#define LCD_DBIB_RDX_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_RDX_PIN                                       HW_GPIO_PIN_22

#define LCD_DBIB_STALL_PORT                                    HW_GPIO_PORT_0
#define LCD_DBIB_STALL_PIN                                     HW_GPIO_PIN_9

#define LCD_DBIB_DB0_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_DB0_PIN                                       HW_GPIO_PIN_17

#define LCD_DBIB_DB1_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_DB1_PIN                                       HW_GPIO_PIN_23

#define LCD_DBIB_DB2_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_DB2_PIN                                       HW_GPIO_PIN_24

#define LCD_DBIB_DB3_PORT                                      HW_GPIO_PORT_1
#define LCD_DBIB_DB3_PIN                                       HW_GPIO_PIN_0

#define LCD_DBIB_DB4_PORT                                      HW_GPIO_PORT_1
#define LCD_DBIB_DB4_PIN                                       HW_GPIO_PIN_1

#define LCD_DBIB_DB5_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_DB5_PIN                                       HW_GPIO_PIN_21

#define LCD_DBIB_DB6_PORT                                      HW_GPIO_PORT_0
#define LCD_DBIB_DB6_PIN                                       HW_GPIO_PIN_19

#define LCD_DBIB_DB7_PORT                                      HW_GPIO_PORT_1
#define LCD_DBIB_DB7_PIN                                       HW_GPIO_PIN_7

#define LCD_DBIB_TE_PORT                                       HW_GPIO_PORT_0
#define LCD_DBIB_TE_PIN                                        HW_GPIO_PIN_10

/*
 * Include definitions for configuring the hardware blocks.
 */
#define LCD_EXT_CLK1_PORT                       HW_GPIO_PORT_1
#define LCD_EXT_CLK1_PIN                        HW_GPIO_PIN_10

#define LCD_EXT_CLK2_PORT                       HW_GPIO_PORT_1
#define LCD_EXT_CLK2_PIN                        HW_GPIO_PIN_11

#if dg_configUSE_H13TB18A
#define H13TB18A_RST_PORT                                               (LCD_RST_PORT)
#define H13TB18A_RST_PIN                                                (LCD_RST_PIN)

#define H13TB18A_SPI_CS_PORT                                            (LCD_SPI_CS_PORT)
#define H13TB18A_SPI_CS_PIN                                             (LCD_SPI_CS_PIN)

#define H13TB18A_SPI_CLK_PORT                                           (LCD_SPI_SCLK_PORT)
#define H13TB18A_SPI_CLK_PIN                                            (LCD_SPI_SCLK_PIN)

#define H13TB18A_DCX_PORT                                               (LCD_SPI_DC_PORT)
#define H13TB18A_DCX_PIN                                                (LCD_SPI_DC_PIN)

#define H13TB18A_SPI_SDA_PORT                                           (LCD_SPI_SD_PORT)
#define H13TB18A_SPI_SDA_PIN                                            (LCD_SPI_SD_PIN)

#define H13TB18A_TE_PORT                                                (LCD_TE_PORT)
#define H13TB18A_TE_PIN                                                 (LCD_TE_PIN)
#endif /* dg_configUSE_H13TB18A */

#if dg_configUSE_TFT12864_57_E
#define TFT12864_57_E_SPI4_SEL_PORT                                     (HW_GPIO_PORT_1)
#define TFT12864_57_E_SPI4_SEL_PIN                                      (HW_GPIO_PIN_3)

#define TFT12864_57_E_CSB_PORT                                          (LCD_SPI_CS_PORT)
#define TFT12864_57_E_CSB_PIN                                           (LCD_SPI_CS_PIN)

#define TFT12864_57_E_RESB_PORT                                         (HW_GPIO_PORT_1)
#define TFT12864_57_E_RESB_PIN                                          (HW_GPIO_PIN_4)

#define TFT12864_57_E_SCL_PORT                                          (LCD_SPI_SCLK_PORT)
#define TFT12864_57_E_SCL_PIN                                           (LCD_SPI_SCLK_PIN)

#define TFT12864_57_E_DCX_PORT                                          (LCD_SPI_DC_PORT)
#define TFT12864_57_E_DCX_PIN                                           (LCD_SPI_DC_PIN)

#define TFT12864_57_E_MOSI_PORT                                         (LCD_SPI_SD_PORT)
#define TFT12864_57_E_MOSI_PIN                                          (LCD_SPI_SD_PIN)
#endif /* dg_configUSE_TFT12864_57_E */

#if dg_configUSE_LPM013M091A
#define LPM013M091A_SDI_PORT                                            (LCD_SPI_SD_PORT)
#define LPM013M091A_SDI_PIN                                             (LCD_SPI_SD_PIN)

#define LPM013M091A_SCL_PORT                                            (LCD_SPI_SCLK_PORT)
#define LPM013M091A_SCL_PIN                                             (LCD_SPI_SCLK_PIN)

#define LPM013M091A_DCX_PORT                                            (LCD_SPI_DC_PORT)
#define LPM013M091A_DCX_PIN                                             (LCD_SPI_DC_PIN)

#define LPM013M091A_CSX_PORT                                            (LCD_SPI_CS_PORT)
#define LPM013M091A_CSX_PIN                                             (LCD_SPI_CS_PIN)

#define LPM013M091A_RST_PORT                                            (LCD_RST_PORT)
#define LPM013M091A_RST_PIN                                             (LCD_RST_PIN)

#define LPM013M091A_TE_PORT                                             (LCD_TE_PORT)
#define LPM013M091A_TE_PIN                                              (LCD_TE_PIN)

#define LPM013M091A_IM0_PORT                                            (HW_GPIO_PORT_1)
#define LPM013M091A_IM0_PIN                                             (HW_GPIO_PIN_3)
#endif /* dg_configUSE_LPM013M091A */

#if dg_configUSE_HM80160A090
#define HM80160A090_SCK_PORT                                            (LCD_SPI_SCLK_PORT)
#define HM80160A090_SCK_PIN                                             (LCD_SPI_SCLK_PIN)

#define HM80160A090_SDA_PORT                                            (LCD_SPI_SD_PORT)
#define HM80160A090_SDA_PIN                                             (LCD_SPI_SD_PIN)

#define HM80160A090_RST_PORT                                            (LCD_RST_PORT)
#define HM80160A090_RST_PIN                                             (LCD_RST_PIN)

#define HM80160A090_DC_PORT                                             (LCD_SPI_DC_PORT)
#define HM80160A090_DC_PIN                                              (LCD_SPI_DC_PIN)

#define HM80160A090_CS_PORT                                             (LCD_SPI_CS_PORT)
#define HM80160A090_CS_PIN                                              (LCD_SPI_CS_PIN)
#endif /* dg_configUSE_HM80160A090 */

#if dg_configUSE_E1394AA65A
#define E1394AA65A_TE_PORT                                              (LCD_TE_PORT)
#define E1394AA65A_TE_PIN                                               (LCD_TE_PIN)

#define E1394AA65A_SCK_PORT                                             (LCD_SPI_SCLK_PORT)
#define E1394AA65A_SCK_PIN                                              (LCD_SPI_SCLK_PIN)

#define E1394AA65A_DO_PORT                                              (LCD_SPI_SD_PORT)
#define E1394AA65A_DO_PIN                                               (LCD_SPI_SD_PIN)

#define E1394AA65A_CS_PORT                                              (LCD_SPI_CS_PORT)
#define E1394AA65A_CS_PIN                                               (LCD_SPI_CS_PIN)

#define E1394AA65A_DC_PORT                                              (LCD_SPI_DC_PORT)
#define E1394AA65A_DC_PIN                                               (LCD_SPI_DC_PIN)

#define E1394AA65A_RST_PORT                                             (LCD_RST_PORT)
#define E1394AA65A_RST_PIN                                              (LCD_RST_PIN)
#endif /* dg_configUSE_E1394AA65A */

#if dg_configUSE_LPM012M134B
#define LPM012M134B_BLUE_0_PORT                                         (LCD_JDI_BLUE0_PORT)
#define LPM012M134B_BLUE_0_PIN                                          (LCD_JDI_BLUE0_PIN)

#define LPM012M134B_BLUE_1_PORT                                         (LCD_JDI_BLUE1_PORT)
#define LPM012M134B_BLUE_1_PIN                                          (LCD_JDI_BLUE0_PIN)

#define LPM012M134B_GREEN_0_PORT                                        (LCD_JDI_GREEN0_PORT)
#define LPM012M134B_GREEN_0_PIN                                         (LCD_JDI_GREEN0_PIN)

#define LPM012M134B_GREEN_1_PORT                                        (LCD_JDI_GREEN1_PORT)
#define LPM012M134B_GREEN_1_PIN                                         (LCD_JDI_GREEN1_PIN)

#define LPM012M134B_RED_0_PORT                                          (LCD_JDI_RED0_PORT)
#define LPM012M134B_RED_0_PIN                                           (LCD_JDI_RED0_PIN)

#define LPM012M134B_RED_1_PORT                                          (LCD_JDI_RED1_PORT)
#define LPM012M134B_RED_1_PIN                                           (LCD_JDI_RED1_PIN)

#define LPM012M134B_FRP_PORT                                            (LCD_JDI_FRP_PORT)
#define LPM012M134B_FRP_PIN                                             (LCD_JDI_FRP_PIN)

#define LPM012M134B_XFRP_PORT                                           (LCD_JDI_XFRP_PORT)
#define LPM012M134B_XFRP_PIN                                            (LCD_JDI_XFRP_PIN)

#define LPM012M134B_VCK_PORT                                            (LCD_JDI_VCK_PORT)
#define LPM012M134B_VCK_PIN                                             (LCD_JDI_VCK_PIN)

#define LPM012M134B_ENB_PORT                                            (LCD_JDI_ENB_PORT)
#define LPM012M134B_ENB_PIN                                             (LCD_JDI_ENB_PIN)

#define LPM012M134B_VST_PORT                                            (LCD_JDI_VST_PORT)
#define LPM012M134B_VST_PIN                                             (LCD_JDI_VST_PIN)

#define LPM012M134B_HCK_PORT                                            (LCD_JDI_HCK_PORT)
#define LPM012M134B_HCK_PIN                                             (LCD_JDI_HCK_PIN)

#define LPM012M134B_HST_PORT                                            (LCD_JDI_HST_PORT)
#define LPM012M134B_HST_PIN                                             (LCD_JDI_HST_PIN)

#define LPM012M134B_XRST_PORT                                           (LCD_JDI_XRST_PORT)
#define LPM012M134B_XRST_PIN                                            (LCD_JDI_XRST_PIN)

#define LPM012M134B_PEN_PORT                                            (HW_GPIO_PORT_1)
#define LPM012M134B_PEN_PIN                                             (HW_GPIO_PIN_19)
#endif /* dg_configUSE_LPM012M134B */

#if dg_configUSE_NHD43480272EFASXN
#define NHD43480272EFASXN_BLUE_0_PORT                                   (LCD_DPI_BLUE0_PORT)
#define NHD43480272EFASXN_BLUE_0_PIN                                    (LCD_DPI_BLUE0_PIN)

#define NHD43480272EFASXN_BLUE_1_PORT                                   (LCD_DPI_BLUE1_PORT)
#define NHD43480272EFASXN_BLUE_1_PIN                                    (LCD_DPI_BLUE1_PIN)

#define NHD43480272EFASXN_GREEN_0_PORT                                  (LCD_DPI_GREEN0_PORT)
#define NHD43480272EFASXN_GREEN_0_PIN                                   (LCD_DPI_GREEN0_PIN)

#define NHD43480272EFASXN_GREEN_1_PORT                                  (LCD_DPI_GREEN1_PORT)
#define NHD43480272EFASXN_GREEN_1_PIN                                   (LCD_DPI_GREEN1_PIN)

#define NHD43480272EFASXN_RED_0_PORT                                    (LCD_DPI_RED0_PORT)
#define NHD43480272EFASXN_RED_0_PIN                                     (LCD_DPI_RED0_PIN)

#define NHD43480272EFASXN_RED_1_PORT                                    (LCD_DPI_RED1_PORT)
#define NHD43480272EFASXN_RED_1_PIN                                     (LCD_DPI_RED1_PIN)

#define NHD43480272EFASXN_DEN_PORT                                      (LCD_DPI_DE_PORT)
#define NHD43480272EFASXN_DEN_PIN                                       (LCD_DPI_DE_PIN)

#define NHD43480272EFASXN_VSYNC_PORT                                    (LCD_DPI_VSYNC_PORT)
#define NHD43480272EFASXN_VSYNC_PIN                                     (LCD_DPI_VSYNC_PIN)

#define NHD43480272EFASXN_CLK_PORT                                      (LCD_DPI_CLK_PORT)
#define NHD43480272EFASXN_CLK_PIN                                       (LCD_DPI_CLK_PIN)

#define NHD43480272EFASXN_HSYNC_PORT                                    (LCD_DPI_HSYNC_PORT)
#define NHD43480272EFASXN_HSYNC_PIN                                     (LCD_DPI_HSYNC_PIN)

#define NHD43480272EFASXN_DISP_PORT                                     (HW_GPIO_PORT_0)
#define NHD43480272EFASXN_DISP_PIN                                      (HW_GPIO_PIN_19)

#define NHD43480272EFASXN_READY_PORT                                    (LCD_DPI_READY_PORT)
#define NHD43480272EFASXN_READY_PIN                                     (LCD_DPI_READY_PIN)

#define NHD43480272EFASXN_CM_PORT                                       (LCD_DPI_CM_PORT)
#define NHD43480272EFASXN_CM_PIN                                        (LCD_DPI_CM_PIN)

#define NHD43480272EFASXN_SD_PORT                                       (LCD_DPI_SD_PORT)
#define NHD43480272EFASXN_SD_PIN                                        (LCD_DPI_SD_PIN)

#endif /* dg_configUSE_NHD480272EF */

#if dg_configUSE_LPM010M297B
#define LPM010M297B_SCLK_PORT                                           (LCD_SPI_SCLK_PORT)
#define LPM010M297B_SCLK_PIN                                            (LCD_SPI_SCLK_PIN)

#define LPM010M297B_SI_PORT                                             (LCD_SPI_SD_PORT)
#define LPM010M297B_SI_PIN                                              (LCD_SPI_SD_PIN)

#define LPM010M297B_SCS_PORT                                            (LCD_SPI_CS_PORT)
#define LPM010M297B_SCS_PIN                                             (LCD_SPI_CS_PIN)

#define LPM010M297B_EXTCOMIN_PORT                                       (HW_GPIO_PORT_0)
#define LPM010M297B_EXTCOMIN_PIN                                        (HW_GPIO_PIN_24)

#define LPM010M297B_DISP_PORT                                           (HW_GPIO_PORT_0)
#define LPM010M297B_DISP_PIN                                            (HW_GPIO_PIN_11)
#endif /* dg_configUSE_LPM010M297B */

#if dg_configUSE_LS013B7DH06
#define LS013B7DH06_SCK_PORT                                            (LCD_SPI_SCLK_PORT)
#define LS013B7DH06_SCK_PIN                                             (LCD_SPI_SCLK_PIN)

#define LS013B7DH06_SDA_PORT                                            (LCD_SPI_SD_PORT)
#define LS013B7DH06_SDA_PIN                                             (LCD_SPI_SD_PIN)

#define LS013B7DH06_CS_PORT                                             (LCD_SPI_CS_PORT)
#define LS013B7DH06_CS_PIN                                              (LCD_SPI_CS_PIN)

#define LS013B7DH06_EXTCOMIN_PORT                                       (HW_GPIO_PORT_0)
#define LS013B7DH06_EXTCOMIN_PIN                                        (HW_GPIO_PIN_24)

#define LS013B7DH06_DISP_PORT                                           (HW_GPIO_PORT_0)
#define LS013B7DH06_DISP_PIN                                            (HW_GPIO_PIN_11)
#endif /* dg_configUSE_LS013B7DH06 */

#if dg_configUSE_LS013B7DH03
#define LS013B7DH03_SCK_PORT                                            (LCD_SPI_SCLK_PORT)
#define LS013B7DH03_SCK_PIN                                             (LCD_SPI_SCLK_PIN)

#define LS013B7DH03_SDA_PORT                                            (LCD_SPI_SD_PORT)
#define LS013B7DH03_SDA_PIN                                             (LCD_SPI_SD_PIN)

#define LS013B7DH03_CS_PORT                                             (LCD_SPI_CS_PORT)
#define LS013B7DH03_CS_PIN                                              (LCD_SPI_CS_PIN)

#define LS013B7DH03_EXTCOMIN_PORT                                       (HW_GPIO_PORT_0)
#define LS013B7DH03_EXTCOMIN_PIN                                        (HW_GPIO_PIN_24)

#define LS013B7DH03_DISP_PORT                                           (HW_GPIO_PORT_0)
#define LS013B7DH03_DISP_PIN                                            (HW_GPIO_PIN_11)
#endif /* dg_configUSE_LS013B7DH03 */

#if dg_configUSE_DT280QV10CT
#define DT280QV10CT_SDA_PORT                                            (LCD_SPI_SD_PORT)
#define DT280QV10CT_SDA_PIN                                             (LCD_SPI_SD_PIN)

#define DT280QV10CT_RST_PORT                                            (LCD_RST_PORT)
#define DT280QV10CT_RST_PIN                                             (LCD_RST_PIN)

#define DT280QV10CT_SI_PORT                                             (LCD_SPI_SI_PORT)
#define DT280QV10CT_SI_PIN                                              (LCD_SPI_SI_PIN)

#define DT280QV10CT_SCK_PORT                                            (LCD_SPI_SCLK_PORT)
#define DT280QV10CT_SCK_PIN                                             (LCD_SPI_SCLK_PIN)

#define DT280QV10CT_DC_PORT                                             (LCD_SPI_DC_PORT)
#define DT280QV10CT_DC_PIN                                              (LCD_SPI_DC_PIN)

#define DT280QV10CT_CS_PORT                                             (LCD_SPI_CS_PORT)
#define DT280QV10CT_CS_PIN                                              (LCD_SPI_CS_PIN)

#define DT280QV10CT_TE_PORT                                             (LCD_TE_PORT)
#define DT280QV10CT_TE_PIN                                              (LCD_TE_PIN)

/*
 * \note For the resistive touch shield the backlight is routed on P1.5
 */
#define DT280QV10CT_BL_PORT                                             (HW_GPIO_PORT_1)
#define DT280QV10CT_BL_PIN                                              (HW_GPIO_PIN_8)

#endif /* dg_configUSE_DT280QV10CT */

#if dg_configUSE_T1D3BP006 || dg_configUSE_T1D3BP006_DSPI
#define T1D3BP006_CS_PORT                                               (LCD_SPI_CS_PORT)
#define T1D3BP006_CS_PIN                                                (LCD_SPI_CS_PIN)

#define T1D3BP006_SCK_PORT                                              (LCD_SPI_SCLK_PORT)
#define T1D3BP006_SCK_PIN                                               (LCD_SPI_SCLK_PIN)

#define T1D3BP006_SDA_PORT                                              (LCD_SPI_SD_PORT)
#define T1D3BP006_SDA_PIN                                               (LCD_SPI_SD_PIN)

#define T1D3BP006_RST_PORT                                              (LCD_RST_PORT)
#define T1D3BP006_RST_PIN                                               (LCD_RST_PIN)

#define T1D3BP006_TE_PORT                                               (LCD_TE_PORT)
#define T1D3BP006_TE_PIN                                                (LCD_TE_PIN)

#define T1D3BP006_DC_PORT                                               (LCD_SPI_DC_PORT)
#define T1D3BP006_DC_PIN                                                (LCD_SPI_DC_PIN)
#endif /* dg_configUSE_T1D3BP006 */

#if dg_configUSE_T1D54BP002
#define T1D54BP002_CS_PORT                                              (LCD_SPI_CS_PORT)
#define T1D54BP002_CS_PIN                                               (LCD_SPI_CS_PIN)

#define T1D54BP002_SCK_PORT                                             (LCD_SPI_SCLK_PORT)
#define T1D54BP002_SCK_PIN                                              (LCD_SPI_SCLK_PIN)

#define T1D54BP002_SDA_PORT                                             (LCD_SPI_SD_PORT)
#define T1D54BP002_SDA_PIN                                              (LCD_SPI_SD_PIN)

#define T1D54BP002_RST_PORT                                             (LCD_RST_PORT)
#define T1D54BP002_RST_PIN                                              (LCD_RST_PIN)
#endif /* dg_configUSE_T1D54BP002 */

#if dg_configUSE_PSP27801
#define PSP27801_RW_PORT                                                (HW_GPIO_PORT_1)
#define PSP27801_RW_PIN                                                 (HW_GPIO_PIN_9)

#define PSP27801_CS_PORT                                                (LCD_SPI_CS_PORT)
#define PSP27801_CS_PIN                                                 (LCD_SPI_CS_PIN)

#define PSP27801_SCK_PORT                                               (LCD_SPI_SCLK_PORT)
#define PSP27801_SCK_PIN                                                (LCD_SPI_SCLK_PIN)

#define PSP27801_SDO_PORT                                               (LCD_SPI_SD_PORT)
#define PSP27801_SDO_PIN                                                (LCD_SPI_SD_PIN)

#define PSP27801_SDI_PORT                                               (LCD_SPI_SI_PORT)
#define PSP27801_SDI_PIN                                                (LCD_SPI_SI_PIN)

#define PSP27801_DC_PORT                                                (LCD_SPI_DC_PORT)
#define PSP27801_DC_PIN                                                 (LCD_SPI_DC_PIN)

#define PSP27801_EN_PORT                                                (HW_GPIO_PORT_0)
#define PSP27801_EN_PIN                                                 (HW_GPIO_PIN_27)

#define PSP27801_RST_PORT                                               (LCD_RST_PORT)
#define PSP27801_RST_PIN                                                (LCD_RST_PIN)
#endif /* dg_configUSE_PSP27801 */

#if dg_configUSE_MCT024L6W240320PML
#define MCT024L6W240320PML_SDA_PORT                                     (LCD_SPI_SD_PORT)
#define MCT024L6W240320PML_SDA_PIN                                      (LCD_SPI_SD_PIN)

#define MCT024L6W240320PML_RST_PORT                                     (LCD_RST_PORT)
#define MCT024L6W240320PML_RST_PIN                                      (LCD_RST_PIN)

#define MCT024L6W240320PML_SCK_PORT                                     (LCD_SPI_SCLK_PORT)
#define MCT024L6W240320PML_SCK_PIN                                      (LCD_SPI_SCLK_PIN)

#define MCT024L6W240320PML_DC_PORT                                      (LCD_SPI_DC_PORT)
#define MCT024L6W240320PML_DC_PIN                                       (LCD_SPI_DC_PIN)

#define MCT024L6W240320PML_CS_PORT                                      (LCD_SPI_CS_PORT)
#define MCT024L6W240320PML_CS_PIN                                       (LCD_SPI_CS_PIN)
#endif /* dg_configUSE_MCT024L6W240320PML */

#if dg_configUSE_ILI9341
#define ILI9341_CS_PORT                                                 (LCD_SPI_CS_PORT)
#define ILI9341_CS_PIN                                                  (LCD_SPI_CS_PIN)

#define ILI9341_DC_PORT                                                 (LCD_SPI_DC_PORT)
#define ILI9341_DC_PIN                                                  (LCD_SPI_DC_PIN)

#define ILI9341_RST_PORT                                                (LCD_RST_PORT)
#define ILI9341_RST_PIN                                                 (LCD_RST_PIN)

#define ILI9341_DO_PORT                                                 (LCD_SPI_SD_PORT)
#define ILI9341_DO_PIN                                                  (LCD_SPI_SD_PIN)

#define ILI9341_SCK_PORT                                                (LCD_SPI_SCLK_PORT)
#define ILI9341_SCK_PIN                                                 (LCD_SPI_SCLK_PIN)
#endif /* dg_configUSE_ILI9341 */

#if dg_configUSE_XSJ120TY2401_QSPI || dg_configUSE_XSJ120TY2401_DSPI || dg_configUSE_XSJ120TY2401_SPI3 || dg_configUSE_XSJ120TY2401_SPI4
#define XSJ120TY2401_SPI_SD0_PORT                                       (LCD_SPI_SD_PORT)
#define XSJ120TY2401_SPI_SD0_PIN                                        (LCD_SPI_SD_PIN)

#define XSJ120TY2401_SPI_SD1_PORT                                       (LCD_SPI_SD1_PORT)
#define XSJ120TY2401_SPI_SD1_PIN                                        (LCD_SPI_SD1_PIN)

#define XSJ120TY2401_SPI_SD2_PORT                                       (LCD_SPI_SD2_PORT)
#define XSJ120TY2401_SPI_SD2_PIN                                        (LCD_SPI_SD2_PIN)

#define XSJ120TY2401_SPI_SD3_PORT                                       (LCD_SPI_SD3_PORT)
#define XSJ120TY2401_SPI_SD3_PIN                                        (LCD_SPI_SD3_PIN)

#define XSJ120TY2401_SPI_SI_PORT                                        (LCD_SPI_SI_PORT)
#define XSJ120TY2401_SPI_SI_PIN                                         (LCD_SPI_SI_PIN)

#define XSJ120TY2401_SPI_SCL_PORT                                       (LCD_SPI_SCLK_PORT)
#define XSJ120TY2401_SPI_SCL_PIN                                        (LCD_SPI_SCLK_PIN)

#define XSJ120TY2401_SPI_CSX_PORT                                       (LCD_SPI_CS_PORT)
#define XSJ120TY2401_SPI_CSX_PIN                                        (LCD_SPI_CS_PIN)

#define XSJ120TY2401_SPI_RST_PORT                                       (LCD_RST_PORT)
#define XSJ120TY2401_SPI_RST_PIN                                        (LCD_RST_PIN)

#define XSJ120TY2401_SPI_TE_PORT                                        (LCD_TE_PORT)
#define XSJ120TY2401_SPI_TE_PIN                                         (LCD_TE_PIN)

#define XSJ120TY2401_SPI_IM0_PORT                                       (HW_GPIO_PORT_0)
#define XSJ120TY2401_SPI_IM0_PIN                                        (HW_GPIO_PIN_30)

#define XSJ120TY2401_SPI_IM1_PORT                                       (HW_GPIO_PORT_0)
#define XSJ120TY2401_SPI_IM1_PIN                                        (HW_GPIO_PIN_31)
#endif /* dg_configUSE_XSJ120TY2401_QSPI || dg_configUSE_XSJ120TY2401_DSPI || dg_configUSE_XSJ120TY2401_SPI3 || dg_configUSE_XSJ120TY2401_SPI4 */

#if dg_configUSE_XSJ120TY2401_DBIB
#define XSJ120TY2401_DBIB_DB0_PORT                                      (LCD_DBIB_DB0_PORT)
#define XSJ120TY2401_DBIB_DB0_PIN                                       (LCD_DBIB_DB0_PIN)

#define XSJ120TY2401_DBIB_DB1_PORT                                      (LCD_DBIB_DB1_PORT)
#define XSJ120TY2401_DBIB_DB1_PIN                                       (LCD_DBIB_DB1_PIN)

#define XSJ120TY2401_DBIB_DB2_PORT                                      (LCD_DBIB_DB2_PORT)
#define XSJ120TY2401_DBIB_DB2_PIN                                       (LCD_DBIB_DB2_PIN)

#define XSJ120TY2401_DBIB_DB3_PORT                                      (LCD_DBIB_DB3_PORT)
#define XSJ120TY2401_DBIB_DB3_PIN                                       (LCD_DBIB_DB3_PIN)

#define XSJ120TY2401_DBIB_DB4_PORT                                      (LCD_DBIB_DB4_PORT)
#define XSJ120TY2401_DBIB_DB4_PIN                                       (LCD_DBIB_DB4_PIN)

#define XSJ120TY2401_DBIB_DB5_PORT                                      (LCD_DBIB_DB5_PORT)
#define XSJ120TY2401_DBIB_DB5_PIN                                       (LCD_DBIB_DB5_PIN)

#define XSJ120TY2401_DBIB_DB6_PORT                                      (LCD_DBIB_DB6_PORT)
#define XSJ120TY2401_DBIB_DB6_PIN                                       (LCD_DBIB_DB6_PIN)

#define XSJ120TY2401_DBIB_DB7_PORT                                      (LCD_DBIB_DB7_PORT)
#define XSJ120TY2401_DBIB_DB7_PIN                                       (LCD_DBIB_DB7_PIN)

#define XSJ120TY2401_DBIB_CS_PORT                                       (LCD_DBIB_CSX_PORT)
#define XSJ120TY2401_DBIB_CS_PIN                                        (LCD_DBIB_CSX_PIN)

#define XSJ120TY2401_DBIB_RESX_PORT                                     (LCD_DBIB_RESX_PORT)
#define XSJ120TY2401_DBIB_RESX_PIN                                      (LCD_DBIB_RESX_PIN)

#define XSJ120TY2401_DBIB_DCX_PORT                                      (LCD_DBIB_DCX_PORT)
#define XSJ120TY2401_DBIB_DCX_PIN                                       (LCD_DBIB_DCX_PIN)

#define XSJ120TY2401_DBIB_WRX_PORT                                      (LCD_DBIB_WRX_PORT)
#define XSJ120TY2401_DBIB_WRX_PIN                                       (LCD_DBIB_WRX_PIN)

#define XSJ120TY2401_DBIB_RDX_PORT                                      (LCD_DBIB_RDX_PORT)
#define XSJ120TY2401_DBIB_RDX_PIN                                       (LCD_DBIB_RDX_PIN)

#define XSJ120TY2401_DBIB_TE_PORT                                       (LCD_DBIB_TE_PORT)
#define XSJ120TY2401_DBIB_TE_PIN                                        (LCD_DBIB_TE_PIN)
#endif /* dg_configUSE_XSJ120TY2401_DBIB */

#if dg_configUSE_ONOC40108_SPI4
#define ONOC40108_SPI4_SD0_PORT                                         (LCD_SPI_SD_PORT)
#define ONOC40108_SPI4_SD0_PIN                                          (LCD_SPI_SD_PIN)

#define ONOC40108_SPI4_SCL_PORT                                         (LCD_SPI_SCLK_PORT)
#define ONOC40108_SPI4_SCL_PIN                                          (LCD_SPI_SCLK_PIN)

#define ONOC40108_SPI4_CSX_PORT                                         (LCD_SPI_CS_PORT)
#define ONOC40108_SPI4_CSX_PIN                                          (LCD_SPI_CS_PIN)

#define ONOC40108_SPI4_DCX_PORT                                         (LCD_SPI_DC_PORT)
#define ONOC40108_SPI4_DCX_PIN                                          (LCD_SPI_DC_PIN)

#define ONOC40108_SPI4_RST_PORT                                         (LCD_RST_PORT)
#define ONOC40108_SPI4_RST_PIN                                          (LCD_RST_PIN)

#define ONOC40108_SPI4_TE_PORT                                          (LCD_TE_PORT)
#define ONOC40108_SPI4_TE_PIN                                           (LCD_TE_PIN)
#endif

#if dg_configUSE_MRB3973_DBIB
#define MRB3973_DBIB_DB0_PORT                                           (LCD_DBIB_DB0_PORT)
#define MRB3973_DBIB_DB0_PIN                                            (LCD_DBIB_DB0_PIN)

#define MRB3973_DBIB_DB1_PORT                                           (LCD_DBIB_DB1_PORT)
#define MRB3973_DBIB_DB1_PIN                                            (LCD_DBIB_DB1_PIN)

#define MRB3973_DBIB_DB2_PORT                                           (LCD_DBIB_DB2_PORT)
#define MRB3973_DBIB_DB2_PIN                                            (LCD_DBIB_DB2_PIN)

#define MRB3973_DBIB_DB3_PORT                                           (LCD_DBIB_DB3_PORT)
#define MRB3973_DBIB_DB3_PIN                                            (LCD_DBIB_DB3_PIN)

#define MRB3973_DBIB_DB4_PORT                                           (LCD_DBIB_DB4_PORT)
#define MRB3973_DBIB_DB4_PIN                                            (LCD_DBIB_DB4_PIN)

#define MRB3973_DBIB_DB5_PORT                                           (LCD_DBIB_DB5_PORT)
#define MRB3973_DBIB_DB5_PIN                                            (LCD_DBIB_DB5_PIN)

#define MRB3973_DBIB_DB6_PORT                                           (LCD_DBIB_DB6_PORT)
#define MRB3973_DBIB_DB6_PIN                                            (LCD_DBIB_DB6_PIN)

#define MRB3973_DBIB_DB7_PORT                                           (LCD_DBIB_DB7_PORT)
#define MRB3973_DBIB_DB7_PIN                                            (LCD_DBIB_DB7_PIN)

#define MRB3973_DBIB_CS_PORT                                            (LCD_DBIB_CSX_PORT)
#define MRB3973_DBIB_CS_PIN                                             (LCD_DBIB_CSX_PIN)

#define MRB3973_DBIB_RESX_PORT                                          (LCD_DBIB_RESX_PORT)
#define MRB3973_DBIB_RESX_PIN                                           (LCD_DBIB_RESX_PIN)

#define MRB3973_DBIB_DCX_PORT                                           (LCD_DBIB_DCX_PORT)
#define MRB3973_DBIB_DCX_PIN                                            (LCD_DBIB_DCX_PIN)

#define MRB3973_DBIB_WRX_PORT                                           (LCD_DBIB_WRX_PORT)
#define MRB3973_DBIB_WRX_PIN                                            (LCD_DBIB_WRX_PIN)

#define MRB3973_DBIB_RDX_PORT                                           (LCD_DBIB_RDX_PORT)
#define MRB3973_DBIB_RDX_PIN                                            (LCD_DBIB_RDX_PIN)

#define MRB3973_DBIB_TE_PORT                                            (LCD_DBIB_TE_PORT)
#define MRB3973_DBIB_TE_PIN                                             (LCD_DBIB_TE_PIN)

#endif /* dg_configUSE_MRB3973_DBIB */

#if dg_configUSE_BOE139F454SM
# define BOE139F454SM_TE_PORT                                            LCD_DBIB_TE_PORT
# define BOE139F454SM_TE_PIN                                             LCD_DBIB_TE_PIN

# define BOE139F454SM_RESX_PORT                                          HW_GPIO_PORT_1
# define BOE139F454SM_RESX_PIN                                           HW_GPIO_PIN_7
#endif /* dg_configUSE_BOE139F454SM */

#if dg_configUSE_E120A390QSR
#define E120A390QSR_SD0_PORT                                             (LCD_SPI_SD_PORT)
#define E120A390QSR_SD0_PIN                                              (LCD_SPI_SD_PIN)

#define E120A390QSR_SD1_PORT                                             (LCD_SPI_SD1_PORT)
#define E120A390QSR_SD1_PIN                                              (LCD_SPI_SD1_PIN)

#define E120A390QSR_SD2_PORT                                             (LCD_SPI_SD2_PORT)
#define E120A390QSR_SD2_PIN                                              (LCD_SPI_SD2_PIN)

#define E120A390QSR_SD3_PORT                                             (LCD_SPI_SD3_PORT)
#define E120A390QSR_SD3_PIN                                              (LCD_SPI_SD3_PIN)

#define E120A390QSR_SCL_PORT                                             (LCD_SPI_SCLK_PORT)
#define E120A390QSR_SCL_PIN                                              (LCD_SPI_SCLK_PIN)

#define E120A390QSR_CSX_PORT                                             (LCD_SPI_CS_PORT)
#define E120A390QSR_CSX_PIN                                              (LCD_SPI_CS_PIN)

#define E120A390QSR_RST_PORT                                             (LCD_RST_PORT)
#define E120A390QSR_RST_PIN                                              (LCD_RST_PIN)

#define E120A390QSR_TE_PORT                                              (LCD_TE_PORT)
#define E120A390QSR_TE_PIN                                               (LCD_TE_PIN)

#define E120A390QSR_IM0_PORT                                             (HW_GPIO_PORT_0)
#define E120A390QSR_IM0_PIN                                              (HW_GPIO_PIN_24)

#define E120A390QSR_IM1_PORT                                             (HW_GPIO_PORT_1)
#define E120A390QSR_IM1_PIN                                              (HW_GPIO_PIN_0)
#endif /* dg_configUSE_E120A390QSR */

#endif

#if dg_configUSE_FT6206
#define TOUCH_MODEL                                                     FT6206

#if DEVICE_FPGA
#define FT6206_SCL_PORT                                                 (HW_GPIO_PORT_1)
#define FT6206_SCL_PIN                                                  (HW_GPIO_PIN_12)

#define FT6206_SDA_PORT                                                 (HW_GPIO_PORT_1)
#define FT6206_SDA_PIN                                                  (HW_GPIO_PIN_11)
#else
#define FT6206_SCL_PORT                                                 (HW_GPIO_PORT_0)
#define FT6206_SCL_PIN                                                  (HW_GPIO_PIN_26)

#define FT6206_SDA_PORT                                                 (HW_GPIO_PORT_0)
#define FT6206_SDA_PIN                                                  (HW_GPIO_PIN_27)
#endif

#define FT6206_INT_PORT                                                 (HW_GPIO_PORT_0)
#define FT6206_INT_PIN                                                  (HW_GPIO_PIN_29)
#define FT6206_INT_MODE                                                 (HW_GPIO_MODE_INPUT)
#define FT6206_INT_POL                                                  (HW_WKUP_TRIG_EDGE_HI)
#endif /* dg_configUSE_FT6206 */

#if dg_configUSE_ZT2628
#define TOUCH_MODEL                                                     ZT2628

#define ZT2628_SCL_PORT                                                 (HW_GPIO_PORT_1)
#define ZT2628_SCL_PIN                                                  (HW_GPIO_PIN_12)

#define ZT2628_SDA_PORT                                                 (HW_GPIO_PORT_1)
#define ZT2628_SDA_PIN                                                  (HW_GPIO_PIN_11)

#define ZT2628_INT_PORT                                                 (HW_GPIO_PORT_1)
#define ZT2628_INT_PIN                                                  (HW_GPIO_PIN_3)
#define ZT2628_INT_MODE                                                 (HW_GPIO_MODE_INPUT_PULLUP)
#define ZT2628_INT_POL                                                  (HW_WKUP_TRIG_EDGE_LO)

#define ZT2628_RST_PORT                                                 (HW_GPIO_PORT_1)
#define ZT2628_RST_PIN                                                  (HW_GPIO_PIN_1)
#endif /* dg_configUSE_ZT2628 */

/* Power control pin that enables I2C on P111 and P112 */
#define DA_PWR_ON_PORT                                                 (HW_GPIO_PORT_0)
#define DA_PWR_ON_PIN                                                  (HW_GPIO_PIN_28)

/*********************************************************************************
 * Touch controller's interrupt line
 */
#ifdef TOUCH_MODEL
#define _TOUCH_INT_MACRO(model, func)                                   model ## _INT_ ## func
#define TOUCH_INT_MACRO(model, func)                                    _TOUCH_INT_MACRO(model, func)

#define TOUCH_INT_PORT                                                  TOUCH_INT_MACRO(TOUCH_MODEL, PORT)
#define TOUCH_INT_PIN                                                   TOUCH_INT_MACRO(TOUCH_MODEL, PIN)
#define TOUCH_INT_MODE                                                  TOUCH_INT_MACRO(TOUCH_MODEL, MODE)
#define TOUCH_INT_POL                                                   TOUCH_INT_MACRO(TOUCH_MODEL, POL)
#endif

#endif /* PERIPH_SETUP_H_ */
