#ifndef DEVICE_h
#define DEVICE_h
#endif

#include <Arduino.h>

class Device {
    private:
        bool timerIsRunning, timerIsCreated = false;
        int secondsOnAfterStart = 0, lastDayTimer = 0, devicePin = 0;

        int timerStartHour = 22;
        int timerStartMinute = 00;
        int timerEndHour = 06;
        int timerEndMinute = 00;

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