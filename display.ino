#include <Arduino.h>
#include <TM1637TinyDisplay.h>

// Module connection pins (Digital Pins)
#define CLK 4
#define DIO 5
#define DIGITS 4
#define BUTTON 10

uint8_t dots = 0b01000000; // Add dots or colons (depends on display module)

TM1637TinyDisplay display(CLK, DIO);

/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

/* Useful Macros for time (s) */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

/* Global Variables in milliseconds */
unsigned long startTime;
unsigned long lastLoopTime;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 80;    // the debounce time; increase if the output flickers
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
bool active = false;
unsigned long accumulated = 5000;

void setup()
{
    display.setBrightness(0);
    startTime = millis();
    Serial.begin(9600);
    pinMode(BUTTON, INPUT);
    digitalWrite(BUTTON, HIGH);
    updateDisplay(accumulated);
}

void updateDisplay(unsigned long counter){

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

void loop()
{
    unsigned long timeNow = millis();

    int reading = digitalRead(BUTTON);

    // If the switch changed, due to noise or pressing reset the debouncing timer
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((timeNow - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == LOW) {
                active = !active;
                if(active){
                    Serial.print("START\n");
                    startTime = timeNow;
                }
                else{
                    Serial.print("STOP\n");
                    accumulated += (timeNow - startTime) / 1000;
                }
            }
        }
    }
    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonState = reading;

    //COUNTER
    if(active){

        unsigned long counter = accumulated + ((timeNow - startTime) / 1000);

        // Update Display
        if (timeNow - lastLoopTime >= 1000) {
            lastLoopTime = timeNow; // remember last time we displayed
            

            updateDisplay(counter);

        }
    }

}
    //display.clear();
