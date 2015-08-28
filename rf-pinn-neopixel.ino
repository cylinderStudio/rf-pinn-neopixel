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

#define BTN_PIN 7
#define STRAND_LED_PIN 4      // there's a strand of 10 LEDs in the wearable, referred to here as STRAND/strand in variable names
#define SINGLE_LED_PIN 5      // there's a single LED as button press indicator, referred to here as SINGLE/single in variable names
#define NUM_STRAND_PIXELS 10
#define NUM_STRAND_REPEATS 5
#define BRIGHTNESS 64
#define STRAND_INTERVAL 50
#define BUTTON_INTERVAL 200
#define SINGLE_INTERVAL 1000
#define OTHER_BOARD 2          // the ID of the other pinnocio board of the pair in the RF group

uint8_t current_strand_pixel = 0;
uint8_t crawl_repeats = NUM_STRAND_REPEATS;
uint8_t blink_repeats = NUM_STRAND_REPEATS;
unsigned long previous_strand_time = 0;
unsigned long current_strand_interval = STRAND_INTERVAL;
byte* c;
unsigned long previous_button_time = 0;
int current_button_state = 1;
int last_button_state = 1;
unsigned long previous_single_time = 0;

bool animation = false;
bool crawl_anim = true;
bool blink_anim = false;
bool blink_on = true;
bool single_on = false;

Adafruit_NeoPixel strand = Adafruit_NeoPixel(NUM_STRAND_PIXELS, STRAND_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel single = Adafruit_NeoPixel(1, SINGLE_LED_PIN, NEO_GRB + NEO_KHZ800);

numvar tellAnimate(void) {
  animation = true;
}

byte * getRandomColor() {
  byte * c = new byte[3];
  
  c[0] = random(0, 255);
  c[1] = random(0, 255);
  c[2] = random(0, 255);
  
  return c;
}

void setNewColor() {
  c = getRandomColor();
}

void deleteColor() {
  delete c;
}

void setup() {
  Scout.setup(SKETCH_NAME, SKETCH_REVISION, SKETCH_BUILD);
  addBitlashFunction("tell.animate", (bitlash_function) tellAnimate);
  pinMode(BTN_PIN, INPUT_PULLUP);
  setNewColor();
  strand.begin();
  strand.setBrightness(BRIGHTNESS);
  strand.show();
  single.begin();
  single.setBrightness(BRIGHTNESS);
  single.show();
}

void loop() {
  Scout.loop();
  
  /* === HANDLE BUTTON === */
  current_button_state = digitalRead(BTN_PIN);
  
  if ((current_button_state == 0) && (last_button_state == 1) && (unsigned long)(millis() - previous_button_time) >= BUTTON_INTERVAL) {
    previous_button_time = millis();
    Shell.eval("command.scout", OTHER_BOARD, "tell.animate");
    single_on = true;
    previous_single_time = millis();
  }
  
  last_button_state = current_button_state;
  
  /* === HANDLE INDICATOR LED (SINGLE) === */
  if (single_on) {
    if ((unsigned long)(millis() - previous_single_time) >= SINGLE_INTERVAL) {
      single.setPixelColor(0, 0, 0, 0);
      single_on = false;
    } else {
      single.setPixelColor(0, 255, 0, 0);
    }
    single.show();
  }
  
  /* === ANIMATION (STRAND) === */
  if (animation) {
    if ((unsigned long)(millis() - previous_strand_time) >= current_strand_interval) {
      previous_strand_time = millis();
      if (crawl_anim) {
        clearStrand();
        strand.setPixelColor(current_strand_pixel, strand.Color(c[0], c[1], c[2]));
        strand.show();
        current_strand_pixel++;
        if (current_strand_pixel >= NUM_STRAND_PIXELS) {
          crawl_repeats--;
          current_strand_pixel = 0;
          if (crawl_repeats > 0) {
            current_strand_interval = crawl_repeats * 10; 
          } else {
            current_strand_interval = STRAND_INTERVAL;
            blink_anim = true;
            crawl_anim = false;
            crawl_repeats = NUM_STRAND_REPEATS;
          }
        }
      } else if (blink_anim) {
        if (blink_on) {
          for (uint8_t i=0; i<NUM_STRAND_PIXELS; i++) {
           strand.setPixelColor(i, strand.Color(c[0], c[1], c[2]));
          }
          strand.show();
          blink_on = false;
        } else {
          clearStrand();
          blink_repeats--;
          blink_on = true;
          if (blink_repeats <= 0) {
            crawl_anim = true;
            blink_anim = animation = false;
            blink_repeats = NUM_STRAND_REPEATS;
            deleteColor();
            setNewColor();
          }
        }
      }
    }
  }
  
}

void clearStrand() {
  for (uint8_t i=0; i<NUM_STRAND_PIXELS; i++) {
    strand.setPixelColor(i, 0, 0, 0);
  }
  strand.show();
}
