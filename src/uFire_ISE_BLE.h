#ifndef ISE_ISE_BLE_H
#define ISE_ISE_BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <uFire_ISE.h>

// https://www.uuidgenerator.net/
#define UFIRE_ISE_UUID "4805d2d0-af9f-42c1-b950-eae78304c408"

#define MV_UUID "ca0331f9-e237-4f81-b9d4-6b2facabfceb"
#define TEMP_UUID "aee115cf-26f0-4096-8914-686b32f123fd"

#define OFFSET_UUID "097335d9-60dd-4194-b606-2fdcb9c37330"
#define HIGH_REF_UUID "1dadca6b-3ecc-41bd-a116-f77248975310"
#define HIGH_READ_UUID "e5c4e636-85d9-4da2-a39b-82b5364ea103"
#define LOW_REF_UUID "1baa566e-4657-4080-a580-d236af1c6bd9"
#define LOW_READ_UUID "b2e6fa56-ba50-4913-8b3e-906715dc5a40"
#define VERSION_UUID "61b9f392-52a9-4127-9048-c130e54f49b4"

class ServerCallback : public BLEServerCallbacks {
public:

  void onConnect(BLEServer *pServer) {
    Serial.println("connected");
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("disconnected");
  }
};

class mVCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onRead(BLECharacteristic *pCharacteristic) {
    String smV = String(ISE_Probe::measuremV());

    pCharacteristic->setValue(smV.c_str());
    pCharacteristic->notify();
  }
};

class tempCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(ISE_Probe::measureTemp());

    pCharacteristic->setValue(s.c_str());
    pCharacteristic->notify();
  }
};

class offsetCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string sOffset = pCharacteristic->getValue();
    float offset        = ::atof(sOffset.c_str());

    ISE_Probe::calibrateSingle(offset);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String sOffset = String(ISE_Probe::getCalibrateOffset());

    pCharacteristic->setValue(sOffset.c_str());
  }
};

class highRefCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string shighRef = pCharacteristic->getValue();
    float highRef        = ::atof(shighRef.c_str());

    ISE_Probe::calibrateProbeHigh(highRef);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String sHighRef = String(ISE_Probe::getCalibrateHighReference());

    pCharacteristic->setValue(sHighRef.c_str());
  }
};

class lowRefCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string s = pCharacteristic->getValue();
    float f       = ::atof(s.c_str());

    ISE_Probe::calibrateProbeLow(f);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(ISE_Probe::getCalibrateLowReference());

    pCharacteristic->setValue(s.c_str());
  }
};

class highReadCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(ISE_Probe::getCalibrateHighReading());

    pCharacteristic->setValue(s.c_str());
  }
};

class lowReadCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(ISE_Probe::getCalibrateLowReading());

    pCharacteristic->setValue(s.c_str());
  }
};

class versionCallback : public BLECharacteristicCallbacks, ISE_Probe {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(ISE_Probe::getVersion());

    pCharacteristic->setValue(s.c_str());
  }
};

class uFire_ISE_BLE : public ISE_Probe, ServerCallback {
public:

  uFire_ISE_BLE(uint8_t sda,
                uint8_t scl) : ISE_Probe(sda, scl) {}

  uFire_ISE_BLE(uint8_t sda,
                uint8_t scl, uint8_t i2c_address) : ISE_Probe(sda, scl, i2c_address) {}

  uFire_ISE_BLE();
  bool connected();
  void startBLE();
  void measuremV();
  void measureTemp();

private:

  BLEServer *pServer;
  BLEService *pService;
  BLECharacteristic *pmV_Characteristic;
  BLECharacteristic *poffset_Characteristic;
  BLECharacteristic *ptemp_Characteristic;
  BLECharacteristic *phigh_ref_Characteristic;
  BLECharacteristic *plow_ref_Characteristic;
  BLECharacteristic *phigh_read_Characteristic;
  BLECharacteristic *plow_read_Characteristic;
  BLECharacteristic *ptc_Characteristic;
  BLECharacteristic *pdp_Characteristic;
  BLECharacteristic *pversion_Characteristic;
};

#endif // ifndef ISE_ISE_BLE_H
