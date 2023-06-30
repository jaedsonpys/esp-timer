#include <Arduino.h>
#include "device.h"

Device::Device(String deviceName, int devicePin) {
    this->deviceName = deviceName;
    this->devicePin = devicePin;

    pinMode(devicePin, OUTPUT);
}