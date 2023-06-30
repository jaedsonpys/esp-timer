#ifndef DEVICE_h
#define DEVICE_h
#endif

#include <Arduino.h>

class Device {
    private:
        bool timerIsRunning, timerIsCreated = false;
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

        bool timerIsActive();

        void powerOn();
        void powerOff();
        bool isON();

        String getTimerStart();
        String getTimerEnd();
};