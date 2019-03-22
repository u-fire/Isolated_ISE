#ifdef ESP32
#if __has_include("BLEDevice.h")

# include "uFire_ISE_BLE.h"

bool uFire_ISE_BLE::connected() {
  if (pServer->getConnectedCount() > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void uFire_ISE_BLE::startBLE() {
  // setup the server
  BLEDevice::init("uFire ISE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallback());
  pService = pServer->createService(BLEUUID(UFIRE_ISE_UUID), 50);

  // setup the mV characteristic
  pmV_Characteristic = pService->createCharacteristic(
    MV_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
  pmV_Characteristic->setCallbacks(new mVCallback());
  pmV_Characteristic->addDescriptor(new BLE2902());
  BLEDescriptor *mV_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  mV_Descriptor->setValue("mV");
  pmV_Characteristic->addDescriptor(mV_Descriptor);
  pService->addCharacteristic(pmV_Characteristic);

  // setup the temp characteristic
  ptemp_Characteristic = pService->createCharacteristic(
    TEMP_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
  ptemp_Characteristic->setCallbacks(new tempCallback());
  ptemp_Characteristic->addDescriptor(new BLE2902());
  BLEDescriptor *temp_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  temp_Descriptor->setValue("C");
  ptemp_Characteristic->addDescriptor(temp_Descriptor);
  pService->addCharacteristic(ptemp_Characteristic);

  // setup the offset characteristic
  poffset_Characteristic = pService->createCharacteristic(
    OFFSET_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
  BLEDescriptor *offset_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  offset_Descriptor->setValue("offset");
  poffset_Characteristic->addDescriptor(offset_Descriptor);
  poffset_Characteristic->setCallbacks(new offsetCallback());
  pService->addCharacteristic(poffset_Characteristic);

  // setup the high ref characteristic
  phigh_ref_Characteristic = pService->createCharacteristic(
    HIGH_REF_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
  BLEDescriptor *highRef_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  highRef_Descriptor->setValue("high reference");
  phigh_ref_Characteristic->addDescriptor(highRef_Descriptor);
  phigh_ref_Characteristic->setCallbacks(new highRefCallback());
  pService->addCharacteristic(phigh_ref_Characteristic);

  // add low reference
  plow_ref_Characteristic = pService->createCharacteristic(
    LOW_REF_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
  BLEDescriptor *lowRef_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  lowRef_Descriptor->setValue("low reference");
  plow_ref_Characteristic->addDescriptor(lowRef_Descriptor);
  plow_ref_Characteristic->setCallbacks(new lowRefCallback());
  pService->addCharacteristic(plow_ref_Characteristic);

  // add high read characteristic
  phigh_read_Characteristic = pService->createCharacteristic(
    HIGH_READ_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
  BLEDescriptor *highRead_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  highRead_Descriptor->setValue("high reading");
  phigh_read_Characteristic->addDescriptor(highRead_Descriptor);
  phigh_read_Characteristic->setCallbacks(new highReadCallback());
  pService->addCharacteristic(phigh_read_Characteristic);

  // add low read chracteristic
  plow_read_Characteristic = pService->createCharacteristic(
    LOW_READ_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
  BLEDescriptor *lowread_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  lowread_Descriptor->setValue("low reading");
  plow_read_Characteristic->addDescriptor(lowread_Descriptor);
  plow_read_Characteristic->setCallbacks(new lowReadCallback());
  pService->addCharacteristic(plow_read_Characteristic);

  // add version chracteristic
  pversion_Characteristic = pService->createCharacteristic(
    VERSION_UUID,
    BLECharacteristic::PROPERTY_READ
    );
  BLEDescriptor *version_Descriptor = new BLEDescriptor((uint16_t)0x2901);
  version_Descriptor->setValue("version");
  pversion_Characteristic->addDescriptor(version_Descriptor);
  pversion_Characteristic->setCallbacks(new versionCallback());
  pService->addCharacteristic(pversion_Characteristic);

  // start everthing
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void uFire_ISE_BLE::measuremV() {
  String smV = String(ISE_Probe::measuremV());

  pmV_Characteristic->setValue(smV.c_str());
  pmV_Characteristic->notify();
}

void uFire_ISE_BLE::measureTemp() {
  String s = String(ISE_Probe::measureTemp());

  ptemp_Characteristic->setValue(s.c_str());
  ptemp_Characteristic->notify();
}

#endif // if __has_include("BLEDevice.h")
#endif // ifdef ESP32
