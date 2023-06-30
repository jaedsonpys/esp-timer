#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "device.h"

WiFiUDP wifiUDP;
NTPClient ntp(wifiUDP);

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
        startTimerTask,
        "timerTask",
        4000,
        this,
        1,
        &this->TimerTaskHandle
    );
}

void Device::startTimerTask(void *parameter) {
    Device *deviceObject = (Device *)parameter;
    deviceObject->timerTask();
}

void Device::timerTask() {
    int hours, minutes, seconds;
    unsigned long cEpoch, waitAtEpoch;

    for(;;) {
        ntp.forceUpdate();
        hours = ntp.getHours();
        minutes = ntp.getMinutes();
        seconds = ntp.getSeconds();

        if(ntp.getDay() != this->lastDayTimer) {
            if(hours >= this->timerStartHour && minutes >= this->timerStartMinute) {
                this->timerIsRunning = true;

                cEpoch = ntp.getEpochTime() - seconds;
                waitAtEpoch = cEpoch + this->secondsOnAfterStart;
                this->lastDayTimer = ntp.getDay();

                digitalWrite(this->devicePin, HIGH);

                while(ntp.getEpochTime() < waitAtEpoch) {
                    delay(1000);
                }

                digitalWrite(this->devicePin, LOW);
                this->timerIsRunning = false;
            }
        }

        delay(1000);
    }
}