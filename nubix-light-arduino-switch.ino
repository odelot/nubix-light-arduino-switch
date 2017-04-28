#include <SPI.h>
#include "RF24.h"
#include <avr/sleep.h>
#include <Arduino.h>

//#define DEBUG_NUBIX(...) Serial.println( __VA_ARGS__ )

#ifndef DEBUG_NUBIX
#define DEBUG_NUBIX(...)
#define NODEBUG_NUBIX
#endif


int wakePin = 2;                 // pin used for waking up


/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9,10);
/**********************************************************/

byte master[6] = {"N"};


long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

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

byte getChannel () {
  byte ret = 0;
  ret += digitalRead (8);
  ret += digitalRead (7) * 2;
  ret += digitalRead (6) * 4;
  ret += digitalRead (5) * 8;
  ret += digitalRead (4) * 16;
  ret += digitalRead (3) * 32;
  return ret;
}

byte getAddress () {
  byte ret = 0;
  ret += digitalRead (A0);
  ret += digitalRead (A1) * 2;
  ret += digitalRead (A2) * 4;
  ret += digitalRead (A3) * 8;
  ret += digitalRead (A4) * 16;
  ret += digitalRead (A5) * 32;
 
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
  pinMode(A1, INPUT_PULLUP);
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
  
 }

void loop() {
  sleepNow();     // sleep function called here
  radio.powerUp(); 
  byte channel = getChannel ();  
  byte address = getAddress ();
  DEBUG_NUBIX (channel);
  DEBUG_NUBIX (address);
  
  memset (master,0,6);
  master[0]='N';
  master[1]=address;
  radio.setChannel (channel);
  radio.openWritingPipe(master);
  long battery = readVcc();
  DEBUG_NUBIX (battery);
  radio.write( &battery, sizeof(long) );
  radio.powerDown();
  
} // Loop

