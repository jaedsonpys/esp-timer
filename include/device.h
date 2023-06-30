#ifndef DEVICE_h
#define DEVICE_h
#endif

#include <Arduino.h>

class Device {
    private:
        bool timerEnabled, timerIsRunning, taskIsCreated = false;
        int secondsOnAfterStart, lastDayTimer, devicePin;
        int timerStartHour, timerStartMinute, timerEndHour, timerEndMinute;

        String deviceName;
        TaskHandle_t TimerTaskHandle;

    public:
        Device(String deviceName, int devicePin);

        void setTimer(int startHour, int startMinute, int endHour, int endMinute);
        static void startTimerTask(void *parameter);
        void timerTask();
        void deleteTimer();

        void powerOn();
        void powerOff();

        String getTimerStart();
        String getTimerEnd();
};