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

    if(timerIsCreated) {
        vTaskDelete(this->TimerTaskHandle);
    }

    xTaskCreate(
        startTimerTask,
        "timerTask",
        4000,
        this,
        1,
        &this->TimerTaskHandle
    );

    this->timerIsCreated = true;
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

void Device::deleteTimer() {
    if(this->timerIsCreated) {
        vTaskDelete(this->TimerTaskHandle);
        this->timerIsCreated = false;
    }
}

bool Device::timerIsActive() {
    return this->timerIsCreated;
}

void Device::powerOn() {
    digitalWrite(this->devicePin, HIGH);

    if(this->timerIsCreated) {    
        // if control device while a timer exists, timer is canceled
        this->deleteTimer();
    }
}

void Device::powerOff() {
    digitalWrite(this->devicePin, LOW);

    if(this->timerIsCreated) {    
        // if control device while a timer exists, timer is canceled
        this->deleteTimer();
    }
}

bool Device::isON() {
    return digitalRead(this->devicePin);
}

String Device::getTimerStart() {
    return String(timerStartHour) + ":" + String(timerStartMinute);
}

String Device::getTimerEnd() {
    return String(timerEndHour) + ":" + String(timerEndMinute);
}