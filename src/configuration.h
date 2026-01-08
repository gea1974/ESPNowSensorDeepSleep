
#define PRODUCT                             "ESP!NOW Sensor - Deep sleep"
#define VERSION                             0x001000
#define OWNER                               "gea"


#define SHUTDOWN_TIMER                      60              //Shutdown ESP after s
//#define ESPNOW_ALIVE                                        //Send alive message to ESP!Now on power on (if no Authentification token is required)
#define ESPNOW_SEND_DATA_COMPLETE                           //Send data message to ESP!Now if all dpid recweived

#ifdef TUYA_TY_OPT300X 
    #define DESCRIPTION                         "Tuya TY Sensor w/o MCU"
    #define PRODUCT_ID                          "TUYATY"

    #define ACTIVE_PIN                      6
    #define ACTIVE_PIN_POLARITY             LOW
    #define SETUP_PIN                       1
    #define SETUP_PIN_POLARITY              LOW

    #define I2C_ADDRESS                     0x44
    #define I2C_SDA_PIN                     3
    #define I2C_SCL_PIN                     7

    #define ESPNOW_TELEGRAM_EXTENDED
    #define DPID_STATE                      32
    #define DPID_BATTERY                    4
    #define DPID_VALUE1                     2

    #define SETTINGS_CONFIG0
    #define SETTINGS_CONFIG0_NAME           "Hysteresis"
    #define SETTINGS_CONFIG0_INIT           10.0
    #define VALUE_1_HYSTERESIS              (float) EspNowSensor.settings.Config[0]
    #define VALUE_1_HYSTERESIS_INIT         SETTINGS_CONFIG0_INIT

    #define POWER_OFF_DEEPSLEEP
    #define DEEPSLEEP_WAKEUP_GPIO_PIN1      1
    #define DEEPSLEEP_WAKEUP_GPIO_PIN2      2
#endif


#ifndef PRODUCT_ID
    #define PRODUCT_ID                          "SENSOR"
#endif
#ifndef PRODUCT_KEY
    #define PRODUCT_KEY                         0xB0
#endif

#ifndef DESCRIPTION
    #define DESCRIPTION                         "Generic Sensor"
#endif
// Enter Project configuration above this line
#include <lib/configuration_defaults.h>