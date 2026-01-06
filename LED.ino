/*
  BLE_Peripheral.ino

  This program uses the ArduinoBLE library to set-up an Arduino Nano 33 BLE 
  as a peripheral device and specifies a service and a characteristic. Depending 
  of the value of the specified characteristic, an on-board LED gets on. 

  The circuit:
  - Arduino Nano 33 BLE. 

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include "Arduino_BMI270_BMM150.h"

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

int i = 0;
int plusThreshold = 30;
int minusThreshold = -30;
float x, y, z;
int previous = 0;

BLEService gestureService(deviceServiceUuid);
BLEByteCharacteristic gestureCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite | BLENotify);


void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1)
      ;
  }


  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");

  BLE.setLocalName("Nano 33 BLE");
  BLE.setAdvertisedService(gestureService);
  gestureService.addCharacteristic(gestureCharacteristic);
  BLE.addService(gestureService);
  gestureCharacteristic.writeValue(-1);
  BLE.advertise();

  Serial.println("Nano 33 BLE (Peripheral Device)");
  Serial.println(" ");
}

int readMotion() {
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
    // 0 = Front
    // 1 = Back
    // 2 = Right
    // 3 = Left
    // -1 = N.A.

    if (y > plusThreshold) {
      return 0;
    }

    if (y < minusThreshold) {
      return 1;
    }

    if (x < minusThreshold) {
      return 2;
    }

    if (x > plusThreshold) {
      return 3;
    }
  }
  return 4;
}

void loop() {
  BLEDevice central = BLE.central();
  Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      int x = readMotion();
      Serial.println(x == previous);
      if (x != previous){
        gestureCharacteristic.writeValue(x);
      }
      previous = x;
      delay(500);
    }

    Serial.println("* Disconnected to central device!");
  }
}