/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "../../core/macros.h"

#ifndef __STM32F1__
  #error "Oops! Select an STM32F1 board in 'Tools > Board.'"
#endif

/**
 * 21017 Victor Perez Marlin for stm32f1 test
 */

#define BOARD_INFO_NAME      "Misc. STM32F103RCT6"
#define DEFAULT_MACHINE_NAME "Soongon MINI"

// Ignore temp readings during development.
//#define BOGUS_TEMPERATURE_GRACE_PERIOD    2000

//
// EEPROM
//
#if EITHER(NO_EEPROM_SELECTED, FLASH_EEPROM_EMULATION)
  #define FLASH_EEPROM_EMULATION
  //#define DEBUG_EEPROM_READWRITE
  #define EEPROM_PAGE_SIZE       (0x800U)  /* Page size = 2KByte */
  #define EEPROM_START_ADDRESS   ((uint32)(0x8000000UL + 256 * 1024UL - (EEPROM_PAGE_SIZE) * 2UL))
  #define MARLIN_EEPROM_SIZE     EEPROM_PAGE_SIZE // 2k
#endif

//
// Timers
//
// These are already defined in DUE, so must be undefined first
#define STEP_TIMER_NUM                         4

//#define DISABLE_DEBUG                           //  We still want to debug with STLINK...
#define DISABLE_JTAG                              //  We free the jtag pins (PA15) but keep STLINK
                                                  //  Release PB3 (X_STOP_PIN) from JTAG NRST role.
                                                  //  Release PB4 (Y_STOP_PIN) from JTAG NRST role.

//
// Limit Switches
//
#define X_STOP_PIN                          PB3
#define Y_STOP_PIN                          PB4
#define Z_STOP_PIN                          PB5

//
// Steppers
//
#define X_STEP_PIN                          PC0
#define X_DIR_PIN                           PC1
#define X_ENABLE_PIN                        PA7

#define Y_STEP_PIN                          PC2
#define Y_DIR_PIN                           PC3
#define Y_ENABLE_PIN                        PA7

#define Z_STEP_PIN                          PC4
#define Z_DIR_PIN                           PC5
#define Z_ENABLE_PIN                        PC15

#define E0_STEP_PIN                         PA11
#define E0_DIR_PIN                          PA12
#define E0_ENABLE_PIN                       PA7

/**
 * TODO: Currently using same Enable pin to all steppers.
 */

#define E1_STEP_PIN                         -1
#define E1_DIR_PIN                          -1
#define E1_ENABLE_PIN                       -1

#define E2_STEP_PIN                         -1
#define E2_DIR_PIN                          -1
#define E2_ENABLE_PIN                       -1

//
// Misc. Functions
//
#define SDSS                                -1
#define LED_PIN                             -1

//
// Heaters / Fans
//
#define HEATER_0_PIN                        PB0   // EXTRUDER 1
#define HEATER_1_PIN                        -1//PB2
#define HEATER_BED_PIN                      PB1   // BED
#define FAN_PIN                             PB7

//
// Temperature Sensors
//
#define TEMP_BED_PIN                        PA0   // Analog Input
#define TEMP_0_PIN                          PA1   // Analog Input
#define TEMP_1_PIN                          -1//PA2   // Analog Input

//
// SD Card
//
#define SDIO_SUPPORT
#define SDIO_CLOCK                          2000000  // 2.0 MHz
#define SD_CD                               PC7
#define SD_DATA0                            PC8
#define SD_DATA1                            PC9
#define SD_DATA2                            PC10
#define SD_DATA3                            PC11
#define SD_CMD                              PD2
#define SD_CLK                              PC12
#define SD_DETECT_PIN                       PC7

#define BTN_EN1                             PA4
#define BTN_EN2                             PA5
#define BTN_ENC                             PA6

//
// Button
//
#define BTN_PRINT                           PC6
#define BTN_LOAD                            PB8
#define BTN_UNLOAD                          PB9

//
// Led
//
#define LED_RED_PIN                         PC14
#define LED_GREEN_PIN                       PA8

//
// Level
//
#define LEVEL_START                         PA2
#define LEVEL_CHECK                         PA3

#if 0
//
// LCD Pins
//
#if HAS_SPI_LCD

  #if ENABLED(NEWPANEL)

    #if ENABLED(REPRAP_DISCOUNT_SMART_CONTROLLER)

      #define BEEPER_PIN                      PB6

      #define BTN_EN1                         PA4
      #define BTN_EN2                         PA5
      #define BTN_ENC                         PA6

      #define SD_DETECT_PIN                   PC7
      #define KILL_PIN                        -1

      #if ENABLED(BQ_LCD_SMART_CONTROLLER)
        #define LCD_BACKLIGHT_PIN             -1
      #endif

      #define LCD_PINS_RS                     PB10
      #define LCD_PINS_ENABLE                 PB11
      #define LCD_PINS_D4                     PB12
      #define LCD_PINS_D5                     PB13
      #define LCD_PINS_D6                     PB14
      #define LCD_PINS_D7                     PB15
    #endif
  #endif // NEWPANEL

#endif // HAS_SPI_LCD
#endif

