#include <Arduino.h>
#include <ezButton.h>
#include <TM1637TinyDisplay.h>

#define CLK1 D1
#define DIO1 D2
#define BTN1 D3
#define LED1 D4
#define CLK2 D5
#define DIO2 D6
#define BTN2 D7
#define LED2 D8

ezButton button1(D3);
ezButton button2(D7);

uint8_t dots = 0b01000000; // Add dots or colons (depends on display module)

TM1637TinyDisplay display1(CLK1, DIO1);
TM1637TinyDisplay display2(CLK2, DIO2);

/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

/* Useful Macros for time (s) */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

/* Global Variables in milliseconds */
unsigned long startTime1;
unsigned long startTime2;
unsigned long lastLoopTime1;
unsigned long lastLoopTime2;
int lastButtonState1 = LOW;
int lastButtonState2 = LOW;
bool active1 = false;
bool active2 = false;
unsigned long accumulated1 = 0;
unsigned long accumulated2 = 0;

void setup()
{
    display1.setBrightness(0);
    display2.setBrightness(0);
    startTime1 = millis();
    startTime2 = millis();
    Serial.begin(9600);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    updateDisplay(display1, accumulated1);
    updateDisplay(display2, accumulated2);

    button1.setDebounceTime(80);
    button2.setDebounceTime(80);
}

void loop()
{
    unsigned long timeNow = millis();

    button1.loop();
    if(button1.isPressed()){
        active1 = !active1;
        if(active1){
            Serial.print("START 1\n");
            startTime1 = timeNow;
            digitalWrite(LED1, HIGH);
        }
        else{
            Serial.print("STOP 1\n");
            accumulated1 += (timeNow - startTime1) / 1000;
            digitalWrite(LED1, LOW);
        }
    }

    if(active1){
        unsigned long counter1 = accumulated1 + ((timeNow - startTime1) / 1000);
        if (timeNow - lastLoopTime1 >= 1000) {
            lastLoopTime1 = timeNow; // remember last time we displayed
            updateDisplay(display1, counter1);
        }
    }

    button2.loop();
    timeNow = millis();
    if(button2.isPressed()){
        active2 = !active2;
        if(active2){
            Serial.print("START 2\n");
            startTime2 = timeNow;
            digitalWrite(LED2, HIGH);
        }
        else{
            Serial.print("STOP 2\n");
            accumulated2 += (timeNow - startTime2) / 1000;
            digitalWrite(LED2, LOW);
        }
    }

    if(active2){
        unsigned long counter2 = accumulated2 + ((timeNow - startTime2) / 1000);
        if (timeNow - lastLoopTime2 >= 1000) {
            lastLoopTime2 = timeNow; // remember last time we displayed
            updateDisplay(display2, counter2);
        }
    }
}

void updateDisplay(TM1637TinyDisplay display, unsigned long counter){
    //Serial.print(counter);
    unsigned long HOURS = numberOfHours(counter);
    unsigned long MINUTES = numberOfMinutes(counter);
    unsigned long SECONDS = numberOfSeconds(counter);

    if (HOURS == 0) {
        display.showNumberDec(MINUTES, dots, true, 2, 0);
        display.showNumberDec(SECONDS, dots, true, 2, 2);
    } else {
        display.showNumberDec(HOURS, dots, true, 2, 0);
        display.showNumberDec(MINUTES, dots, true, 2, 2);
    }
}

