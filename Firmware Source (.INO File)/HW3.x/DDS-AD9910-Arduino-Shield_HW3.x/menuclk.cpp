/*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *******************************************                                      
 */
 
#include "menuclk.h"

  int SetupMenuPos=0;
  #define CLOCK_MENU_SOURCE_INDEX         0
  #define CLOCK_MENU_FREQ_INDEX           1
  #define CLOCK_MENU_DIVIDER_INDEX        2
  #define CLOCK_MENU_DAC_CURRENT_INDEX    3
  #define CLOCK_MENU_DDS_CORE_CLOCK_INDEX 4
  #define CLOCK_MENU_CORE_CLOCK_OFFSET    5  
  #define CLOCK_MENU_SAVE_INDEX           6
  #define CLOCK_MENU_EXIT_INDEX           7
  #define CLOCK_MENU_LAST_INDEX           CLOCK_MENU_EXIT_INDEX
  
  int ClockSourceIndex=INIT_CLOCK_SOURCE_INDEX;
  //String ClockSourceName[3]={"XO", "TCXO/OCXO", "EXT OSC"};
  String ClockSourceName[4]={"XO", "TCXO", "ExtTCXO/OCXO", "EXT OSC"};
  /*#define CLOCK_SOURCE_XO_INDEX         0
  #define CLOCK_SOURCE_TCXO_INDEX  1
  #define CLOCK_SOURCE_EXT_TCXO_INDEX  2
  #define CLOCK_SOURCE_EXT_OSC_INDEX    3*/
  uint32_t XO_Values[2]={20000000, 25000000}; // Гц
  #define XO_QTY 2
  #define XO_MIN_VALUE 20000000 // Гц
  #define XO_MAX_VALUE 25000000 // Гц
  //uint32_t XO_Freq=XO_MIN_VALUE;
  int XO_Freq_Index=0;
  //uint32_t TCXO_Values[6]={5000000, 10000000, 20000000, 25000000, 40000000, 50000000}; // Гц
  #define TCXO_QTY 7
  uint32_t TCXO_Values[]={5000000, 10000000, 20000000, 25000000, 40000000, 50000000, 62500000}; // Гц
  
  #define TCXO_MIN_VALUE 5000000 // Гц
  //#define TCXO_MAX_VALUE 50000000 // Гц
  #define TCXO_MAX_VALUE 60000000 // Гц
  //uint32_t TCXO_Freq=TCXO_MIN_VALUE;
  int TCXO_Freq_Index=INIT_CLOCK_TCXO_INDEX;
  //uint32_t Freq_Value; //содержит 
  //int Freq_Index=0;
  int Max_Freq_Index=0;
  #define EXT_OSC_MIN_VALUE 60000000 //Гц
  #define EXT_OSC_MAX_VALUE 3000000000 //Гц
  //int EXT_OSC_Value=EXT_OSC_MIN_VALUE;
  String DividerName[2]={"OFF", "ON"};
  int DividerIndex=0;
  #define BASE_DDS_CORE_CLOCK 1000000000.0 //это частота на которую будет сбрасываться значение DDS_Core_Clock в любых "непонятных" случаях
  //#define BASE_DDS_CORE_CLOCK 999998000.0 //
  #define MAX_DDS_CORE_CLOCK 1520000000.0 //максимально допустимое значение DDS_CORE_CLOCK
  uint32_t EXT_OSC_Freq=BASE_DDS_CORE_CLOCK;
  uint32_t DDS_Core_Clock=BASE_DDS_CORE_CLOCK; // in MHZ ------> In HZ переделать в герцы
  uint32_t Ref_Clk=0; //реальная частота генератора в Герцах, хранит реальное значение частоты для любого источника и для кварца и для припаянных генераторов и для внешней источника, подключенного ко входу INOSC  
  String DACCurrentName[2]={"Normal", "HI"};
  int DACCurrentIndex=0;
  int16_t ClockOffset=0;

void DDS_Clock_Config_Menu()
{
    
  while(1)
  {
    curPos=0;
    curPos=myEnc.read();

    modeButton.Update(); // read state button encoder

    if (modeButton.clicks > 0) // short click button encoder
    {
      switch (SetupMenuPos)
      {
        case CLOCK_MENU_SAVE_INDEX:
          SaveClockSettings(); 
          MakeOut();
          DisplaySaved(); 
          delay(1000); 
          SetupMenuPos=CLOCK_MENU_EXIT_INDEX; 
        break;
        case CLOCK_MENU_EXIT_INDEX:
          LoadClockSettings();  
          MakeOut(); 
          return; 
        break;
        default: EditMode = !EditMode;
      }
    }

    if (EditMode == false) //перемещение по менню
    { 
      if (curPos > 0) 
      {
        SetupMenuPos++;
        if (SetupMenuPos > CLOCK_MENU_LAST_INDEX) SetupMenuPos=CLOCK_MENU_SOURCE_INDEX;
        if ((ClockSourceIndex!=CLOCK_SOURCE_EXT_OSC_INDEX) && (SetupMenuPos==CLOCK_MENU_DIVIDER_INDEX)) SetupMenuPos=CLOCK_MENU_DAC_CURRENT_INDEX; //пропускаем настройку делитля если не выбрано внешнее тактирование
        if ((ClockSourceIndex==CLOCK_SOURCE_EXT_OSC_INDEX) && (SetupMenuPos==CLOCK_MENU_DDS_CORE_CLOCK_INDEX)) SetupMenuPos=CLOCK_MENU_CORE_CLOCK_OFFSET; //пропускаем ручную натсройку частоты если выбрано внешнее тактирование
      }
      if (curPos < 0) 
      { 
        SetupMenuPos--;
        if (SetupMenuPos < CLOCK_MENU_SOURCE_INDEX) SetupMenuPos=CLOCK_MENU_LAST_INDEX;
        if ((ClockSourceIndex!=CLOCK_SOURCE_EXT_OSC_INDEX) && (SetupMenuPos==CLOCK_MENU_DIVIDER_INDEX)) SetupMenuPos=CLOCK_MENU_FREQ_INDEX; //пропускаем настройку делитля если не выбрано внешнее тактирование
        if ((ClockSourceIndex==CLOCK_SOURCE_EXT_OSC_INDEX) && (SetupMenuPos==CLOCK_MENU_DDS_CORE_CLOCK_INDEX)) SetupMenuPos=CLOCK_MENU_DAC_CURRENT_INDEX; //пропускаем ручную натсройку частоты если выбрано внешнее тактирование
      }
    } else //изменение параметров
    {
      if (curPos > 0)
      switch (SetupMenuPos)
      {
        case CLOCK_MENU_SOURCE_INDEX: 
          ClockSourceIndex++;
          if (ClockSourceIndex>CLOCK_SOURCE_EXT_OSC_INDEX) ClockSourceIndex=0;
          DDS_Core_Clock=BASE_DDS_CORE_CLOCK; 
          break;
        case CLOCK_MENU_FREQ_INDEX: 
          if (ClockSourceIndex == CLOCK_SOURCE_XO_INDEX) XO_Freq_Index++;
          if (ClockSourceIndex == CLOCK_SOURCE_TCXO_INDEX) TCXO_Freq_Index++;
          if (ClockSourceIndex == CLOCK_SOURCE_EXT_TCXO_INDEX) TCXO_Freq_Index++;
          if (ClockSourceIndex == CLOCK_SOURCE_EXT_OSC_INDEX) EXT_OSC_Freq=EXT_OSC_Freq+1000000*curPos;
          if (EXT_OSC_Freq>EXT_OSC_MAX_VALUE) EXT_OSC_Freq=EXT_OSC_MIN_VALUE;
          if (ClockSourceIndex != CLOCK_SOURCE_EXT_OSC_INDEX) DDS_Core_Clock=BASE_DDS_CORE_CLOCK; 
          break;
        case CLOCK_MENU_DIVIDER_INDEX: DividerIndex=!DividerIndex;
          break;
        case CLOCK_MENU_DAC_CURRENT_INDEX: DACCurrentIndex=!DACCurrentIndex; 
          break;
        case CLOCK_MENU_DDS_CORE_CLOCK_INDEX: DDS_Core_Clock=DDS_Core_Clock + Ref_Clk;
          if (DDS_Core_Clock > MAX_DDS_CORE_CLOCK) DDS_Core_Clock=BASE_DDS_CORE_CLOCK;
          break;
        case CLOCK_MENU_CORE_CLOCK_OFFSET: 
          ClockOffset=ClockOffset+curPos;
          if (ClockOffset>9999) ClockOffset=0;
          MakeOut();
          break;
      } //switch end
      if (curPos < 0)
      switch (SetupMenuPos)
      {
        case CLOCK_MENU_SOURCE_INDEX: 
          ClockSourceIndex--;
          if (ClockSourceIndex<CLOCK_SOURCE_XO_INDEX) ClockSourceIndex=CLOCK_SOURCE_EXT_OSC_INDEX;
          DDS_Core_Clock=BASE_DDS_CORE_CLOCK; 
        break;
        case CLOCK_MENU_FREQ_INDEX: 
          if (ClockSourceIndex == CLOCK_SOURCE_XO_INDEX) XO_Freq_Index--;
          if (ClockSourceIndex == CLOCK_SOURCE_TCXO_INDEX) TCXO_Freq_Index--;
          if (ClockSourceIndex == CLOCK_SOURCE_EXT_TCXO_INDEX) TCXO_Freq_Index--;
          if (ClockSourceIndex == CLOCK_SOURCE_EXT_OSC_INDEX) EXT_OSC_Freq=EXT_OSC_Freq+1000000*curPos;
          if (EXT_OSC_Freq<EXT_OSC_MIN_VALUE) EXT_OSC_Freq=EXT_OSC_MAX_VALUE;
          if (ClockSourceIndex != CLOCK_SOURCE_EXT_OSC_INDEX) DDS_Core_Clock=BASE_DDS_CORE_CLOCK;
        break;
        case CLOCK_MENU_DIVIDER_INDEX: DividerIndex=!DividerIndex; 
        break;
        case CLOCK_MENU_DAC_CURRENT_INDEX: DACCurrentIndex=!DACCurrentIndex; 
        break;
        case CLOCK_MENU_DDS_CORE_CLOCK_INDEX: DDS_Core_Clock=DDS_Core_Clock - Ref_Clk;
          if (DDS_Core_Clock < BASE_DDS_CORE_CLOCK) DDS_Core_Clock=trunc(MAX_DDS_CORE_CLOCK/Ref_Clk)*Ref_Clk; // lower DDS_Core_Clock to a value that is completely divided
        break;
        case CLOCK_MENU_CORE_CLOCK_OFFSET: 
          ClockOffset=ClockOffset+curPos;
          if (ClockOffset<-9999) ClockOffset=0;
          MakeOut();
        break;
      }
    }

    if (curPos != 0)
    {
      if (XO_Freq_Index>(XO_QTY-1)) XO_Freq_Index=0;
      if (TCXO_Freq_Index>(TCXO_QTY-1)) TCXO_Freq_Index=0;

      if (XO_Freq_Index<0) XO_Freq_Index=XO_QTY-1;
      if (TCXO_Freq_Index<0) TCXO_Freq_Index=TCXO_QTY-1;
        
      if (ClockSourceIndex == CLOCK_SOURCE_XO_INDEX) Ref_Clk=XO_Values[XO_Freq_Index];
      if (ClockSourceIndex == CLOCK_SOURCE_TCXO_INDEX) Ref_Clk=Ref_Clk=TCXO_Values[TCXO_Freq_Index];
      if (ClockSourceIndex == CLOCK_SOURCE_EXT_TCXO_INDEX) Ref_Clk=Ref_Clk=TCXO_Values[TCXO_Freq_Index];
      if (ClockSourceIndex == CLOCK_SOURCE_EXT_OSC_INDEX) Ref_Clk=EXT_OSC_Freq;
      if (ClockSourceIndex==CLOCK_SOURCE_EXT_OSC_INDEX) DDS_Core_Clock=EXT_OSC_Freq/(DividerIndex+1);
    }
    DisplayClockSetupMenu();
  }
}

void DisplayClockSetupMenu()
{
  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(WHITE); 
  //display.setCursor(40, 0);  
  display.setCursor(0, 0);     
  display.cp437(true);         
  display.println(F("SETUP"));

    //*********
  display.setTextSize(1); 
  display.setCursor(62, 0);
  display.println(F("DDS CORE"));
  display.setCursor(62, 8);
  //if (SetupMenuPos==CLOCK_MENU_DDS_CORE_CLOCK_INDEX) GetColor();//display.setTextColor(BLACK, WHITE); 
  display.print(F("CLOCK:")); 
  //display.print(" ");
  if (SetupMenuPos==CLOCK_MENU_DDS_CORE_CLOCK_INDEX) GetColor();
  display.print(DDS_Core_Clock/1000000);
  display.setTextColor(WHITE); 
  //***********
  
  display.setTextSize(1);
  display.setCursor(0, 17);
  //if (SetupMenuPos==CLOCK_MENU_SOURCE_INDEX) GetColor(); //display.setTextColor(BLACK, WHITE);
  display.print(F("Clock Src"));
  display.setCursor(52, 18);
  display.print(":");
  if (SetupMenuPos==CLOCK_MENU_SOURCE_INDEX) GetColor();
  if (ClockSourceName[ClockSourceIndex].length() < 12) display.setCursor(58, 17);
    else display.setCursor(56, 17);
  display.print(ClockSourceName[ClockSourceIndex]);
  display.setTextColor(WHITE);

  display.setCursor(0, 26);
  //if (SetupMenuPos==CLOCK_MENU_FREQ_INDEX) GetColor(); //display.setTextColor(BLACK, WHITE);
  display.print("Clock Freq");
  //display.print(" ");
  display.setCursor(58, 27);
  display.print(":");
  if (SetupMenuPos==CLOCK_MENU_FREQ_INDEX) GetColor();
  if (Ref_Clk < 1000000000) display.print(Ref_Clk/1000000.0, 1);
    else display.print(Ref_Clk/1000000);
  display.setTextColor(WHITE);
  //display.setCursor(101, 26);
  display.print(" MHZ");

  if (ClockSourceIndex==CLOCK_SOURCE_EXT_OSC_INDEX)
  {
    display.setCursor(0, 35);
    //if (SetupMenuPos==CLOCK_MENU_DIVIDER_INDEX) GetColor(); //display.setTextColor(BLACK, WHITE);
    display.print("Divider by 2");
    display.setCursor(70, 35);
    display.print(":");
    if (SetupMenuPos==CLOCK_MENU_DIVIDER_INDEX) GetColor();
    display.print(DividerName[DividerIndex]);
    display.setTextColor(WHITE);
  }

  display.setTextColor(WHITE);
  display.setCursor(0, 44);
  //if (SetupMenuPos==CLOCK_MENU_DAC_CURRENT_INDEX) GetColor(); //display.setTextColor(BLACK, WHITE);
  display.print("DAC current");
  //display.print(" ");
  display.setCursor(64, 45);
  display.print(":");
  if (SetupMenuPos==CLOCK_MENU_DAC_CURRENT_INDEX) GetColor();
  display.print(DACCurrentName[DACCurrentIndex]);
  display.setTextColor(WHITE);
  
  display.setCursor(0, 55);
  if (SetupMenuPos==CLOCK_MENU_SAVE_INDEX) GetColor(); //display.setTextColor(BLACK, WHITE);
  display.println("SAVE");

  display.setTextColor(WHITE);
  display.setCursor(103, 55);
  if (SetupMenuPos==CLOCK_MENU_EXIT_INDEX) GetColor(); //display.setTextColor(BLACK, WHITE);
  display.println("EXIT");

  display.setTextColor(WHITE);
  display.setCursor(30, 55);
  //if (SetupMenuPos==CLOCK_MENU_CORE_CLOCK_OFFSET) GetColor(); //display.setTextColor(BLACK, WHITE);
  display.print("Offset");
  if (ClockOffset>=0) display.print(" ");
    else {if (SetupMenuPos==CLOCK_MENU_CORE_CLOCK_OFFSET) GetColor(); display.print("-");}
  if (SetupMenuPos==CLOCK_MENU_CORE_CLOCK_OFFSET) GetColor();
  display.print(abs(ClockOffset));
  display.setTextColor(WHITE);

  display.display();
}

void LoadClockSettings()
{
  if (EEPROM.read(CLOCK_SETTINGS_FLAG_ADR)!=56)
  {
    ClockSourceIndex=INIT_CLOCK_SOURCE_INDEX;
    XO_Freq_Index=INIT_CLOCK_XO_INDEX; 
    TCXO_Freq_Index=INIT_CLOCK_TCXO_INDEX; 
    DividerIndex=INIT_DIVIDER;
    EXT_OSC_Freq=INIT_EXT_OSC_FREQ;
    DDS_Core_Clock=BASE_DDS_CORE_CLOCK;
    DACCurrentIndex=INIT_DAC_CURRENT_INDEX;
    ClockOffset=INIT_CLOCK_OFFSET; 
    
    #if DBG==1
    Serial.println(F("Setting default clock settings"));
    Serial.print(F("ClockSourceIndex="));
    Serial.println(ClockSourceIndex);
    Serial.print(F("XO_Freq_Index="));
    Serial.println(XO_Freq_Index);
    Serial.print(F("TCXO_Freq_Index="));
    Serial.println(TCXO_Freq_Index);
    Serial.print(F("DividerIndex="));
    Serial.println(DividerIndex);
    Serial.print(F("EXT_OSC_Freq="));
    Serial.println(EXT_OSC_Freq);
    Serial.print(F("DDS_Core_Clock="));
    Serial.println(DDS_Core_Clock);
    Serial.print(F("DACCurrentIndex="));
    Serial.println(DACCurrentIndex);
    Serial.print(F("ClockOffset="));
    Serial.println(ClockOffset);
    #endif
    
    SaveClockSettings();
  } else
  {
    EEPROM.get(CLOCK_SOURCE_INDEX_ADR, ClockSourceIndex);
    EEPROM.get(CLOCK_XO_FREQ_INDEX_ADR, XO_Freq_Index);
    EEPROM.get(CLOCK_TCXO_FREQ_INDEX_ADR, TCXO_Freq_Index);
    EEPROM.get(DIVIDER_ADR, DividerIndex);
    EEPROM.get(EXT_OSC_FREQ_ADR, EXT_OSC_Freq);
    EEPROM.get(DDS_CORE_CLOCK_ADR, DDS_Core_Clock);
    EEPROM.get(DAC_CURRENT_INDEX_ADR, DACCurrentIndex);
    EEPROM.get(CORE_CLOCK_OFFSET_ADR, ClockOffset);
    
    #if DBG==1
    Serial.println(F("Reading clock settings from EEPROM"));
    Serial.print(F("ClockSourceIndex="));
    Serial.println(ClockSourceIndex);
    Serial.print(F("XO_Freq_Index="));
    Serial.println(XO_Freq_Index);
    Serial.print(F("TCXO_Freq_Index="));
    Serial.println(TCXO_Freq_Index);
    Serial.print(F("DividerIndex="));
    Serial.println(DividerIndex);
    Serial.print(F("EXT_OSC_Freq="));
    Serial.println(EXT_OSC_Freq);
    Serial.print(F("DDS_Core_Clock="));
    Serial.println(DDS_Core_Clock);
    Serial.print(F("DACCurrentIndex="));
    Serial.println(DACCurrentIndex);
    Serial.print(F("ClockOffset="));
    Serial.println(ClockOffset);
    #endif

    //DDS_Core_Clock=DDS_Core_Clock+ClockOffset;
  }
  
  selectClockSrcPath(ClockSourceIndex);

  bool PLL=false;
  if (ClockSourceIndex!=CLOCK_SOURCE_EXT_OSC_INDEX) 
  {
    PLL=true;
    if (ClockSourceIndex==CLOCK_SOURCE_XO_INDEX) Ref_Clk=XO_Values[XO_Freq_Index];
    if (ClockSourceIndex==CLOCK_SOURCE_TCXO_INDEX) Ref_Clk=TCXO_Values[TCXO_Freq_Index];
    if (ClockSourceIndex==CLOCK_SOURCE_EXT_TCXO_INDEX) Ref_Clk=TCXO_Values[TCXO_Freq_Index];
  } else
  {
    Ref_Clk=EXT_OSC_Freq;
  }

  DDS_Init(PLL, DividerIndex, Ref_Clk);
}

/**************************************************************************
 * Одна из функций сохранения параметров в ЕЕПРОМ. 
 * Сохраняет настройки тактирвоания.
 * И устанавливает в ЕЕПРОМ флаг по адресу SETUP_FLAG_ADR равный 55, 
 * который говорит о том, что во ЕЕПРОМ хранятся валидные настройки
 *************************************************************************/
void SaveClockSettings()
{
  EEPROM.put(CLOCK_SOURCE_INDEX_ADR, ClockSourceIndex);
  EEPROM.put(CLOCK_XO_FREQ_INDEX_ADR, XO_Freq_Index);
  EEPROM.put(CLOCK_TCXO_FREQ_INDEX_ADR, TCXO_Freq_Index);
  EEPROM.put(DIVIDER_ADR, DividerIndex);
  EEPROM.put(EXT_OSC_FREQ_ADR, EXT_OSC_Freq);
  EEPROM.put(DDS_CORE_CLOCK_ADR, DDS_Core_Clock);
  EEPROM.put(DAC_CURRENT_INDEX_ADR, DACCurrentIndex);
  EEPROM.put(CORE_CLOCK_OFFSET_ADR, ClockOffset);

  selectClockSrcPath(ClockSourceIndex);
  
  bool PLL=false;
  if (ClockSourceIndex!=CLOCK_SOURCE_EXT_OSC_INDEX) 
  {
    PLL=true;
    if (ClockSourceIndex==CLOCK_SOURCE_XO_INDEX) Ref_Clk=XO_Values[XO_Freq_Index];
    if (ClockSourceIndex==CLOCK_SOURCE_TCXO_INDEX) Ref_Clk=TCXO_Values[TCXO_Freq_Index]; 
    if (ClockSourceIndex==CLOCK_SOURCE_EXT_TCXO_INDEX) Ref_Clk=TCXO_Values[TCXO_Freq_Index];
  } else
  {
    Ref_Clk=EXT_OSC_Freq;
  }

  EEPROM.write(CLOCK_SETTINGS_FLAG_ADR, 56);
  DDS_Init(PLL, DividerIndex, Ref_Clk);
}
