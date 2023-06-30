#ifndef DEVICE_h
#define DEVICE_h
#endif

#include <Arduino.h>

class Device {
    private:
        bool timerEnabled, timerIsRunning;
        int secondsOnAfterStart, lastDayTimer, devicePin;
        int timerStartHour, timerStartMinute, timerEndHour, timerEndMinute;

        String deviceName;
        TaskHandle_t TimerTaskHandle;

    public:
        Device(String deviceName, int devicePin);

        void setTimer(int startHour, int startMinute, int endHour, int endMinute);
        static void timerTask(void *parameter);
        void deleteTimer();

        void powerOn();
        void powerOff();

        String getTimerStart();
        String getTimerEnd();
};