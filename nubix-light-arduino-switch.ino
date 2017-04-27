#include <SPI.h>
#include "RF24.h"
#include <avr/sleep.h>

int wakePin = 2;                 // pin used for waking up
int led=13;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9,10);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

void wakeUpNow() {
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
}

void sleepNow() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();          // enables the sleep bit in the mcucr register
    attachInterrupt(0,wakeUpNow, CHANGE); // use interrupt 0 (pin 2) and run function
    sleep_mode();            // here the device is actually put to sleep!!
    // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
    sleep_disable();         // first thing after waking from sleep: disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the wakeUpNow code will not be executed during normal running time.
}

int getChannel () {
  int ret = 0;
  ret += digitalRead (8);
  ret += digitalRead (7) * 2;
  ret += digitalRead (6) * 4;
  ret += digitalRead (5) * 8;
  ret += digitalRead (4) * 16;
  ret += digitalRead (3) * 32;
  return ret;
}

int getAddress () {
  int ret = 0;
  ret += digitalRead (A7);
  ret += digitalRead (A6) * 2;
  ret += digitalRead (A3) * 4;
  ret += digitalRead (A2) * 8;
  ret += digitalRead (A1) * 16;
  ret += digitalRead (A0) * 32;
  return ret;
}

void setup() {
  Serial.begin(115200);
  pinMode(wakePin, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT_PULLUP);
  pinMode(A7, INPUT_PULLUP);

  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  //radio.openWritingPipe(addresses[1]);
 
  
  // Start the radio listening for data
  //radio.startListening();
}

void loop() {
  sleepNow();     // sleep function called here
  radio.powerUp(); 
  int channel = getChannel ();  
  int address = getAddress ();  

  


} // Loop

