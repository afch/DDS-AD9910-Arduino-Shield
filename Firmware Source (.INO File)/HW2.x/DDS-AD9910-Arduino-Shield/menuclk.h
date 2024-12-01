/*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *******************************************                                      
 */
#ifndef __MENUCLK_H
#define __MENUCLK_H

#include "main.h"

#define CLOCK_SOURCE_INDEX_ADR 0
#define CLOCK_XO_FREQ_INDEX_ADR 2
#define CLOCK_TCXO_FREQ_INDEX_ADR 4
#define DIVIDER_ADR 6
#define EXT_OSC_FREQ_ADR 8 //and 9, 10, 11
#define DDS_CORE_CLOCK_ADR 12 //and 13, 14, 15
#define DAC_CURRENT_INDEX_ADR 20
// Addresses from 24 to 60 used for main settings !!!!!!!!!!!!!!
#define CORE_CLOCK_OFFSET_ADR 64

#define CLOCK_SETTINGS_FLAG_ADR 101 // defualt settings flag address

//******* EEPROM INIT VALUES ******
#define INIT_CLOCK_SOURCE_INDEX CLOCK_SOURCE_XO_INDEX
#define INIT_CLOCK_XO_INDEX 0
#define INIT_CLOCK_TCXO_INDEX 0
#define INIT_DIVIDER 0
#define INIT_EXT_OSC_FREQ 1000000000
#define INIT_DAC_CURRENT_INDEX 0
#define INIT_CLOCK_OFFSET 0

void DDS_Clock_Config_Menu();
void DisplayClockSetupMenu();
void SaveClockSettings();
void LoadClockSettings();

  extern int SetupMenuPos;//=0;
  extern int ClockSourceIndex;//=0;
  extern uint32_t EXT_OSC_Freq;//=1000;
  #define CLOCK_SOURCE_XO_INDEX         0
  #define CLOCK_SOURCE_TCXO_INDEX       1
  #define CLOCK_SOURCE_EXT_OSC_INDEX    2
  
  extern uint32_t DDS_Core_Clock;
  extern int DACCurrentIndex;
  extern int16_t ClockOffset;
  
#endif
