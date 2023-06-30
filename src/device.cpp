#include <Arduino.h>
#include "device.h"

Device::Device(String deviceName, int devicePin) {
    this->deviceName = deviceName;
    this->devicePin = devicePin;

    pinMode(devicePin, OUTPUT);
}

void Device::setTimer(int startHour, int startMinute, int endHour, int endMinute) {
    this->timerStartHour = startHour;
    this->timerStartMinute = startMinute;
    this->timerEndHour = endHour;
    this->timerEndMinute = endMinute;

    int timeDiff, minutesDiff;

    if(startHour > endHour) {
        int startHourSub = 24 - startHour;
        timeDiff = endHour + startHourSub;
    } else {
        timeDiff = endHour - startHour;
    }

    minutesDiff = endMinute - startMinute;

    if(minutesDiff < 0) {
        minutesDiff *= -1;
    }

    this->secondsOnAfterStart = (timeDiff * 3600) + (minutesDiff * 60);

    xTaskCreate(
        timerTask,
        "timerTask",
        4000,
        NULL,
        1,
        &this->TimerTaskHandle
    );
}