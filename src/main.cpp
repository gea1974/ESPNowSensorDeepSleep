#include <configuration.h>

#include <lib/logging.h>

#include <lib/EspNowSensor.h>

uint8_t   dataBatteryLevel = 0;
bool datasent=false;

//=============================ESP!Now
void OnDataSent(
    #ifdef ESP8266 
    uint8_t *mac, uint8_t sendStatus
    #endif
    #ifdef ESP32
    const uint8_t *mac_addr, esp_now_send_status_t status
    #endif
)
{
    #ifdef ESP32
    uint8_t sendStatus = (uint8_t)status;
    uint8_t mac[6];
    mac[0] = mac_addr[0];
    mac[1] = mac_addr[1];
    mac[2] = mac_addr[2];
    mac[3] = mac_addr[3];
    mac[4] = mac_addr[4];
    mac[5] = mac_addr[5];
    #endif

    EspNowSensor.OnDataSent(mac,sendStatus);
}
void OnDataRecv(
                #ifdef ESP8266
                uint8_t * mac, uint8_t *incomingData, uint8_t len
                #endif
                #ifdef ESP32
                const esp_now_recv_info *esp_now_info, const uint8_t *incomingData, int len
                #endif
)
{
  #ifdef ESP32
  uint8_t mac[6];
  mac[0] = esp_now_info->src_addr[0];
  mac[1] = esp_now_info->src_addr[1];
  mac[2] = esp_now_info->src_addr[2];
  mac[3] = esp_now_info->src_addr[3];
  mac[4] = esp_now_info->src_addr[4];
  mac[5] = esp_now_info->src_addr[5];
  #endif
      
  EspNowSensor.OnDataRecv(mac,incomingData,len);
}


#if (SENSOR_TYPE==SENSOR_OPT300X)

  #include "OPT300x.h"
  #include <Wire.h>

  OPT300x opt300x;

  void sensorConfig() {
    opt300x.config.RangeNumber = 0b1100;
    opt300x.config.ConvertionTime = 0b1;
    opt300x.config.ModeOfConversionOperation = 0b11;
    opt300x.config.Latch = 0b1;
    opt300x.config.Polarity = 0b0;
    opt300x.config.MaskExponent = 0b0;
    opt300x.config.FaultCount = 0b01;

    OPT300x_ErrorCode errorConfig = opt300x.writeConfig(opt300x.config);
    if (errorConfig != NO_ERROR) printLogMsgTime("OPT300x: configuration error: %d\n", errorConfig);
    else printLogMsgTime("OPT300x: Config: Done: %04X\n", opt300x.config.rawData);
  }

  void sensorPrintConfig(bool verbose) {
      printLogMsgTime("OPT300x Config: %04X\n", opt300x.config.rawData);
      if (verbose) {
        printLogMsg("          Conversion ready (R): %02X\n", opt300x.config.ConversionReady);
        printLogMsg("          Conversion time (R/W): %02X\n", opt300x.config.ConvertionTime);
        printLogMsg("          Fault count field (R/W): %02X\n", opt300x.config.FaultCount);
        printLogMsg("          Flag high field (R-only): %02X\n",opt300x.config.FlagHigh);
        printLogMsg("          Flag low field (R-only): %02X\n", opt300x.config.FlagLow);
        printLogMsg("          Latch field (R/W): %02X\n", opt300x.config.Latch);
        printLogMsg("          Mask exponent field (R/W): %02X\n", opt300x.config.MaskExponent);
        printLogMsg("          Mode of conversion operation (R/W): %02X\n", opt300x.config.ModeOfConversionOperation);
        printLogMsg("          Polarity field (R/W): %02X\n", opt300x.config.Polarity);
        printLogMsg("          Overflow flag (R-only): %02X\n", opt300x.config.OverflowFlag);
        printLogMsg("          Range number (R/W): %02X\n", opt300x.config.RangeNumber);
      }
  }

  void sensorSetup() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    opt300x.begin(I2C_ADDRESS);

    printLogMsgTime("OPT300x: Manufacturer:%04X Device:%04X\n", opt300x.readManufacturerID(),opt300x.readDeviceID());
    opt300x.readConfig();
    sensorConfig();
  }

  void sensorRead() {
    OPT300x_S result = opt300x.readResult();
    OPT300x_S hiLimit = opt300x.readHighLimit();
    OPT300x_S loLimit = opt300x.readLowLimit();
    printLogMsgTime("OPT300x: Read: Result: %0.1f (Result=%04X, Exponent=%02X)\n", opt300x.result, result.raw.Result, result.raw.Exponent);
    printLogMsgTime("OPT300x: Read: High-Limit: %0.1f Low-Limit: %0.1f\n", opt300x.highLimit, opt300x.lowLimit);
  }

  void sensorSetLimits() {
    float hysteresis = opt300x.result*SETTINGS_HYSTERESIS_PERC * 0.01;
    if (hysteresis<=SETTINGS_HYSTERESIS_MIN) hysteresis = SETTINGS_HYSTERESIS_MIN;
    if (hysteresis<=0) hysteresis = SETTINGS_HYSTERESIS_MIN_INIT;
    opt300x.highLimit = opt300x.result + hysteresis;
    opt300x.lowLimit = opt300x.result - hysteresis;
    printLogMsgTime("OPT300x: SetLimits: calculated hysteresis: %0.1f\n", hysteresis);
    if (opt300x.highLimit<hysteresis) opt300x.highLimit = hysteresis;
    if (opt300x.lowLimit<0) opt300x.lowLimit = 0;

    OPT300x_ErrorCode errorHiLimit = opt300x.writeHighLimit(opt300x.highLimit);
    OPT300x_ErrorCode errorLoLimit = opt300x.writeLowLimit(opt300x.lowLimit);
    printLogMsgTime("OPT300x: SetLimits: Write: High-Limit: %0.1f Low-Limit: %0.1f\n", opt300x.highLimit, opt300x.lowLimit);
  }

  void sensorEspNowData() {
      EspNowSensor.espnowMessageDataAddSensorValue(DPID_STATE, uint8_t(opt300x.config.FlagHigh) * 2 + uint8_t(opt300x.config.FlagLow));
      EspNowSensor.espnowMessageDataAddSensorValue(DPID_BATTERY,dataBatteryLevel); 
      EspNowSensor.espnowMessageDataAddSensorValue(DPID_VALUE1,uint32_t(opt300x.result));  
      EspNowSensor.espnowMessageDataAddSensorValue(DPID_VALUE2,uint32_t(opt300x.highLimit-opt300x.result));    
  }

#endif //SENSOR_TYPE==SENSOR_OPT300X)

#ifndef SENSOR_TYPE
    #define SENSOR_TYPE       SENSOR_NONE
    void sensorSetup(){
    printLogMsgTime("Sensor setup: no sensor defined, enter config mode\n");
      EspNowSensor.configmodeEnter();
    };    
    void sensorRead(){};
    void sensorSetLimits(){};
    void sensorEspNowData(){};    
#endif

//=============================Main
void setup() {
  EspNowSensor.begin();
  EspNowSensor.registerSendCallback(OnDataSent);
  EspNowSensor.registerRecvCallback(OnDataRecv);

  sensorSetup();    
  sensorRead();
  sensorSetLimits();

  dataBatteryLevel = EspNowSensor.batteryLevel();
}

void loop() {
  EspNowSensor.configmodeHandle();

  if (EspNowSensor.configmode) ;
  else if (EspNowSensor.broadcastSending) ;
  else if (!EspNowSensor.readyToSend) EspNowSensor.espnowAuthCheck();
  else if (!datasent){
    sensorEspNowData();    
    datasent = true;                                 
  }
  else EspNowSensor.powerOff();


  EspNowSensor.shutDownCheck();
  printLogMsgIdle();
  delay(10);    // Add a small delay to avoid overwhelming the CPU
}
