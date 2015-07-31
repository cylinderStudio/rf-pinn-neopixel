/**************************************************************************\
* Pinoccio Library                                                         *
* https://github.com/Pinoccio/library-pinoccio                             *
* Copyright (c) 2014, Pinoccio Inc. All rights reserved.                   *
* ------------------------------------------------------------------------ *
*  This program is free software; you can redistribute it and/or modify it *
*  under the terms of the MIT License as described in license.txt.         *
\**************************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <Scout.h>
#include <GS.h>
#include <bitlash.h>
#include <lwm.h>
#include <js0n.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "version.h"

#define PIN 7
// change these two values before deploying to each device
#define NUM_PIXELS 16
#define OTHER_BOARD 2

uint16_t currentPixel = 0;
unsigned long previousPixelTime = 0;
unsigned long pixelInterval = 50;
unsigned long previousFunctionTime = 0;
unsigned long functionInterval = 2000;
bool isConnected = true;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

numvar tellConnected(void) {
  isConnected = true;
}

void setup() {
  Scout.setup(SKETCH_NAME, SKETCH_REVISION, SKETCH_BUILD);
  addBitlashFunction("tell.connected", (bitlash_function) tellConnected);
  strip.begin();
  strip.setBrightness(64);
  strip.show();
}

void loop() {
  Scout.loop();
    
  if ((unsigned long)(millis() - previousPixelTime) >= pixelInterval) {
    previousPixelTime = millis();
    
    if (isConnected) {
      eraseStrip();
      strip.setPixelColor(currentPixel, 255, 0, 0);
      strip.show();
      currentPixel++;
      
      if (currentPixel >= NUM_PIXELS) {
        currentPixel = 0;
        isConnected = false;
      }
    } else {
      currentPixel = 0;
      eraseStrip();
    }
  }
  
  if ((unsigned long)(millis() - previousFunctionTime) >= functionInterval) {
    previousFunctionTime = millis();
    Shell.eval("command.scout", OTHER_BOARD, "tell.connected");
  }
}

void eraseStrip() {
  for (int led=0; led<NUM_PIXELS; led++) {
    strip.setPixelColor(led, 0, 0, 0);
  }
  strip.show();
}
