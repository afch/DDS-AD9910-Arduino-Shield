/*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *******************************************                                      
 */

#ifndef __MAIN_H
#define __MAIN_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ClickButton.h>

#include <Encoder.h>
#ifndef GRA_AND_AFCH_ENCODER_MOD3
  #error The "Encoder" library modified by GRA and AFCH must be used!
#endif

#include "AD9910.h"
#include "menuclk.h"
#include <Wire.h>
#include <EEPROM.h>

#define DBG 0 //debug messsages 0 - disabled, 1 - enabled

#define MODE_PIN     2
#define RFOUT_PIN    3
#define A_PIN     18
#define B_PIN     19

#define CLOCK_FROM_INTERNAL_TCXO 0
#define CLOCK_FROM_EXTERANL_SRC 1
#define TCXO_POWER_PIN A2 //REF_LED
#define TCXO_PATH_PIN A0 //V1
#define EXTERANL_SRC_PATH_PIN A1 //V2

#define TRG_OUT_PIN 48
#define TRG_LED_PIN A4

void DisplaySaved(void);
void UpdateDisplay();
void MakeOut();

void SingleProfileFreqOut(uint32_t freq_output, int16_t Amplitude_dB_output);
bool IsSweepTimeTooLong();

extern ClickButton modeButton;
extern ClickButton RFOutButton;
extern bool RFOut;

extern bool EditMode;

extern Adafruit_SSD1306 display;
extern int ModIndex;
extern int M, K, H, A, MenuPos;
extern int8_t curPos;
extern Encoder myEnc;
extern void GetColor();
extern void LoadModulationSettings();
extern void selectClockSrcPath(uint8_t path);

#endif
