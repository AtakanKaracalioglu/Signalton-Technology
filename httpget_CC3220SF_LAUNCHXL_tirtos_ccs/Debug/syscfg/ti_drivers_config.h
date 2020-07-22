/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  DO NOT EDIT - This file is generated for the CC3220SF_LAUNCHXL
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC3220SF_LAUNCHXL

#ifndef DeviceFamily_CC3220
#define DeviceFamily_CC3220
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== Crypto ========
 */

#define CONFIG_Crypto_0             0

/*
 *  ======== GPIO ========
 */

/* P04, LaunchPad User Button SW2 (left) */
#define CONFIG_GPIO_BUTTON_0        0
/* P64, LaunchPad LED D10 (Red) */
#define CONFIG_GPIO_LED_0           1
/* P18 */
#define CONFIG_GPIO_0               2
/* P58 */
#define CONFIG_GPIO_1               3
/* P03 */
#define CONFIG_GPIO_2               4
/* P21 */
#define CONFIG_GPIO_3               5

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== I2C ========
 */

/*
 *  SCL: P01
 *  SDA: P02
 *  LaunchPad I2C
 */
#define CONFIG_I2C_TMP              0

/* ======== I2C Addresses and Speeds ======== */
#include <ti/drivers/I2C.h>

/* ---- CONFIG_I2C_TMP I2C bus components ---- */

/* BMA222E address and max speed */
#define CONFIG_I2C_TMP_BMA222E_ADDR     (0x18)
#define CONFIG_I2C_TMP_BMA222E_MAXSPEED (400U) /* Kbps */

/* TMP006 address and max speed */
#define CONFIG_I2C_TMP_TMP006_ADDR     (0x41)
#define CONFIG_I2C_TMP_TMP006_MAXSPEED (3400U) /* Kbps */

/* CONFIG_I2C_TMP max speed (supported by all components) */
#define CONFIG_I2C_TMP_MAXSPEED   (3400U) /* Kbps */
#define CONFIG_I2C_TMP_MAXBITRATE ((I2C_BitRate)I2C_3400kHz)


/*
 *  ======== SPI ========
 */

#define CONFIG_NWP_SPI               0

/*
 *  MOSI: 07
 *  MISO: 06
 *  SCLK: 05
 *  SS: 08
 */
#define CONFIG_SPI_0                  1


/*
 *  ======== Timer ========
 */

#define CONFIG_TIMER_0              0
#define CONFIG_TIMER_1              1
#define CONFIG_TIMER_2              2

/*
 *  ======== UART ========
 */

/*
 *  TX: P55
 *  RX: P57
 *  XDS110 UART
 */
#define CONFIG_UART_0               0


/*
 *  ======== Watchdog ========
 */

#define CONFIG_WATCHDOG_0           0


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
