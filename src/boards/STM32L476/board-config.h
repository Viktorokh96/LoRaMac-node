/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#define BOARD_TCXO_WAKEUP_TIME                      0

/*!
 * Define indicating if an external IO expander is to be used
 */
#define BOARD_IOE_EXT

/*!
 * Board IO Extender pins definitions
 */
/*
#define IRQ_MPL3115                                 IOE_0
#define IRQ_MAG3110                                 IOE_1
#define GPS_POWER_ON                                IOE_2
#define RADIO_PUSH_BUTTON                           IOE_3
#define BOARD_POWER_DOWN                            IOE_4
#define SPARE_IO_EXT_5                              IOE_5
#define SPARE_IO_EXT_6                              IOE_6
#define SPARE_IO_EXT_7                              IOE_7
#define N_IRQ_SX9500                                IOE_8
#define IRQ_1_MMA8451                               IOE_9
#define IRQ_2_MMA8451                               IOE_10
#define TX_EN_SX9500                                IOE_11
#define LED_1                                       IOE_12
#define LED_2                                       IOE_13
*/

#define LED_4                                       PB_2
#define LED_5                                       PE_8


/*!
 * UART Settings
 */
#define UART_BAUDRATE								115200

/*!
 * Board MCU pins definitions
 */

#define RADIO_RESET                                 PB_7

#define RADIO_MOSI                                  PE_15
#define RADIO_MISO                                  PE_14
#define RADIO_SCLK                                  PE_13
#define RADIO_NSS                                   PE_12

#define RADIO_DIO_0                                 PA_5
#define RADIO_DIO_1                                 PA_1
#define RADIO_DIO_2                                 PA_2
#define RADIO_DIO_3                                 PA_3
#define RADIO_DIO_4                                 PE_11
#define RADIO_DIO_5                                 PE_10

#define RADIO_ANT_SWITCH_HF                         NC
#define RADIO_ANT_SWITCH_LF                         NC

#define OSC_LSE_IN                                  PC_14
#define OSC_LSE_OUT                                 PC_15

#define OSC_HSE_IN                                  NC
#define OSC_HSE_OUT									NC

#define USB_DM                                      NC
#define USB_DP                                      NC

#define I2C_SCL                                     NC
#define I2C_SDA                                     NC

#define BOOT_1                                      NC

//#define GPS_PPS                                     PB_1
#define UART_TX                                     PD_5
#define UART_RX                                     PD_6

#define DC_DC_EN                                    NC
#define BAT_LEVEL_PIN                               NC
#define BAT_LEVEL_CHANNEL                           ADC_CHANNEL_8

#define WKUP1                                       NC
//#define USB_ON                                      PA_2

#define RF_RXTX                                     NC

#define SWDIO                                       PA_13
#define SWCLK                                       PA_14

//#define TEST_POINT1                                 PB_12
//#define TEST_POINT2                                 PB_13
//#define TEST_POINT3                                 PB_14
//#define TEST_POINT4                                 PB_15


#define PIN_NC                                      PB_5

// Debug pins definition.
#define RADIO_DBG_PIN_TX                            NC
#define RADIO_DBG_PIN_RX                            NC

#endif // __BOARD_CONFIG_H__
