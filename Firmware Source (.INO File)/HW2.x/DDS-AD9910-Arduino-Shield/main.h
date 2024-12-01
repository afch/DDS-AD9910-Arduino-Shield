/*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *******************************************                                      
 */

#ifndef __MAIN_H
#define __MAIN_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ClickButton.h>

#include "AD9910.h"
#include "menuclk.h"
#include <Wire.h>
#include <EEPROM.h>

#define DBG 0 //debug messsages 0 - disabled, 1 - enabled

#define MODE_PIN     A0
#define DOWN_PIN     A1
#define UP_PIN       A2

void DisplaySaved(void);
void UpdateDisplay();
void MakeOut();

void SingleProfileFreqOut(uint32_t freq_output, int16_t Amplitude_dB_output);
bool IsSweepTimeTooLong();


extern ClickButton modeButton;
extern ClickButton upButton;
extern ClickButton downButton;

extern Adafruit_SSD1306 display;
extern int ModIndex;
extern int M, K, H, A, MenuPos;
#endif
