 /*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *          ONLY FOR Version 3.5+           *
 *                                        *******************************************
 *                                                (ТОЛЬКО ДЛЯ ПЛАТ ВЕРСИИ 3.5+)
 *  v.3.20 //11.04.2025 Исправлена ошибка когда параметры не применялись после выключения и включения выхода, если они были изменены в этот период
 *  v.3.19 //10.02.2025 Исправлена ошибка которая не позволяла регулировать частоту после достижения одного из лимитов
 *  v.3.18 //23.09.2024 Добавлены коммандды через UART (Serial), инвертирована внутренняя логика работы кнопки ON/OFF
 *  v.3.17 //18.09.2024 Улучшена стабильность при включении (подаче питания)
 *  v.3.16 //16.09.2024 Добавлено новое значение частоты TCXO - 62.5 МГц
 *  v.3.15 //13.09.2024 TCXO на 50 мгц, является настройкой тактирования по-умолчанию
 *  v.3.14 //11.09.2024 Добавлен S-Curve
 *  v.3.13 //05.09.2024 Внедряем S-Curve...
 *  v.3.12 //01.07.2024 добавление сохранение в EEPROM
 *  v.3.11 //28.06.2024 Mеню Local Oscillator работает, но нужно добавить сохранение в EEPROM
 *  v.3.10 //26.06.2024 Начато внедрение меню Local Oscillator 
 *  v.3.09 //25.06.2024 TCXO(ext) переименовано в EXT TCXO. Исправлено изменение частота EXT TCXO в setup 
 *  v.3.08 Добавляем новый тип источника тактирования TCXO(ext) (не менятеся частота TCXO в setup, нужно исправлять)
 *  v.3.07 //14.06.2024 Включение и выключение выхода кнопкой OUTPUT ON/OFF
 *  v.3.06 //12.06.2024 добавлено измерение входного напряжения, возможность отключения питания TCXO и коммутация источников тактирования 
 *  v.3.05 //01.04.2024 меню модуляции адаптировано для работы с помошью энкодера
 *  v.3.04 //27.03.2024 изменение параметров в меню тактирования с помошью энкодера 
 *  v.3.03 //20.03.2024 изменение параметров в главном меню с помошью энкодера 
 *  v.3.02 //17.05.2022 перемещение по меню с использованием с энкодера  
 *  v.3.0 //11.11.2020 Начата переработка под использование с энкодером (только для плат версии 3)                                      
    Для любой модуляции нужно сначала вызывать фнукцию calcBestStepRate перед PrepRegistersToSaveWaveForm,
    зачастую это так и сдлеано внутри функций SaveAMWavesToRAM и SaveFMWavesToRAM
    v.2.13 //06.06.2020 закончени работа над sweep (и проверено)
    v.2.12 //01.07.2020 добавлен функционал уменшаеющий установленное пользователем время для свипа, но не проверено
    v.2.11 //25.06.2020 отключаем DRG для AM и FM модуляции, и делаем все что прописано в версии 2.10...сделано все, кроме установки времени в минимально допустимое
    v.2.10 //17.06.2020 делаем функции сброса частоты sweep в минимум или максимум и установка времени в минимально возможное,
    //а также вывод соответстующих уведомлений для пользователя
    v.2.09 //12.06.2020 продолжаем внедрение sweep функционала в меню
    v.2.08 //11.06.2020 добавлены функции Sweep, FreqToFTW, DigitalRamp
    v.2.07 //02.06.2020
    Added: offset paramter at Core Clock Menu
    v.2.06 //29.05.2020
    шумы были из-за того что внутри DDS_INIT была явная запись частоты 100 мгц в область памяти сингл профайла
    выбор сингл профайла перенесен из функции SingleProfileFreqOut в функцию DDS_FOUT
    в функции DDS_FOUT реализован алгоритм установления значения на выходах для выбора заданного профайла
    v.2.05 //27.05.2020
    найдена проблема с шумом суть в неправильном расчете FTW из-за проблем с округлением
    для решения использовалась библиотека FLOAT64
   v.2.04 //26.05.2020
   продолжаем работу над Core Clock offset
   было принято решение прибалять ClockOffset к DDS_Core_Clock уже непосредственно во время расчетов (но это еще не сделано!)
   добавлено сохранение и загрузка ClockOffset в и из EEPROM
   Что-то пошло не так: частота 100 МГЦ, SPAN 1МГЦ, BW 100 HZ, на анализаторе много шумов, некоторые переходят уровень -40 дцБ
   нужно сравнить с тем как было с прошивко 2.03
   v.2.03 //23.05.2020
   Added: Core Clock offset !!!!!!!!!!недоделали !!!!!!!!!! нет сохранение в EEPROM и значение залазит на слово exit
   Fixed: dBm value display when AM enabled (+-)
   v.2.02 //20.05.2020
   Fixed: Wrong Amplitude calculation for FM modulation
   Fixed: bug with core clock value
   Added: frequency change was accelerated
   v.2.01 //19.05.2020
   Fixed: const 0.01745 changed to PI/180.0
   Added: CalcDBCorrection function to correct dBm value on display when AM enabled
   Fixed: bug with no output when FM deviation freq lower than 1000 kHz
*/
#include "main.h"
#include "ad9910.h"
#include "GyverTimers.h"
#include <GParser.h>
#define FIRMWAREVERSION 3.20

Encoder myEnc(B_PIN, A_PIN);

#define LOW_FREQ_LIMIT  100000
#define HIGH_FREQ_LIMIT  600000000
// 0 - 20, 64 used for clock settings
#define M_ADR 24
#define K_ADR 28
#define H_ADR 32
#define A_ADR 36

#define MOD_INDEX_ADR 40
#define MOD_FREQK_ADR 44
#define MOD_FREQH_ADR 48
#define MOD_AM_DEPTH_ADR 52
#define MOD_FM_DEVK_ADR 56
#define MOD_FM_DEVH_ADR 60

#define SWEEP_START_FREQ_M_ADR 66
#define SWEEP_START_FREQ_K_ADR 68
#define SWEEP_START_FREQ_H_ADR 70

#define SWEEP_END_FREQ_M_ADR 72
#define SWEEP_END_FREQ_K_ADR 74
#define SWEEP_END_FREQ_H_ADR 76

#define SWEEP_TIME_ADR 78
#define SWEEP_TIME_FORMAT_ADR 80

#define S_CURVE_CARRIER_ADR 82
#define S_CURVE_BANDWITH_ADR 86

#define MAIN_SETTINGS_FLAG_ADR 100 // defualt settings 
// ADR 101 reserved for clock settings
#define MODULATION_SETTINGS_FLAG_ADR 102 // defualt settings

#define LO_SETTINGS_ADR 104
#define LO_SETTINGS_FLAG_ADR LO_SETTINGS_ADR + 11 //sizeof(loSettings)

#define INIT_M 100
#define INIT_K 0
#define INIT_H 0
#define INIT_A 0

#define NONE_MOD_TYPE     0
#define AM_MOD_TYPE       1
#define FM_MOD_TYPE       2
#define SWEEP_MOD_TYPE    3
#define S_CURVE_MOD_TYPE  4
#define LO_MOD_TYPE       5
//#define LSB_MOD_TYPE      6
//#define USB_MOD_TYPE      7

#define INIT_MOD_INDEX 0
#define INIT_MFREQ_K 1
#define INIT_MFREQ_H 0
#define INIT_AM_DEPTH 50
#define INIT_FM_DEV_K 3
#define INIT_FM_DEV_H 0

//************SWEEP INITs********************
#define INIT_SWEEP_START_FREQ_M 100
#define INIT_SWEEP_START_FREQ_K 0
#define INIT_SWEEP_START_FREQ_H 0

#define INIT_SWEEP_END_FREQ_M 200
#define INIT_SWEEP_END_FREQ_K 0
#define INIT_SWEEP_END_FREQ_H 0

#define INIT_SWEEP_TIME 1
#define INIT_SWEEP_TIME_FORMAT 0
//****************************************

#define INIT_S_CURVE_CARRIER 144000000
#define INIT_S_CURVE_BANDWITH 3 //in kHz !!!!

#define MOD_MENU_TYPE_INDEX 0
#define MOD_MENU_MFREQ_K_INDEX 1
#define MOD_MENU_MFREQ_H_INDEX 2
#define MOD_MENU_DEPTH_DEV_K_INDEX 3
#define MOD_MENU_FM_DEV_H_INDEX 4

#define MOD_MENU_SWEEP_START_FREQ_M_INDEX 5
#define MOD_MENU_SWEEP_START_FREQ_K_INDEX 6
#define MOD_MENU_SWEEP_START_FREQ_H_INDEX 7

#define MOD_MENU_SWEEP_END_FREQ_M_INDEX 8
#define MOD_MENU_SWEEP_END_FREQ_K_INDEX 9
#define MOD_MENU_SWEEP_END_FREQ_H_INDEX 10

#define MOD_MENU_SWEEP_TIME_INDEX 11
#define MOD_MENU_SWEEP_TIME_FORMAT_INDEX 12

#define MOD_MENU_CARRIER_M_INDEX 13
#define MOD_MENU_CARRIER_K_INDEX 14
#define MOD_MENU_CARRIER_H_INDEX 15

#define MOD_MENU_BW_INDEX 16

#define MOD_MENU_SAVE_INDEX 17
#define MOD_MENU_EXIT_INDEX 18

//*********************************
#define MAIN_MENU_MHZ_INDEX         0
#define MAIN_MENU_KHZ_INDEX         1
#define MAIN_MENU_HZ_INDEX          2
#define MAIN_MENU_AMPLITUDE_INDEX   3
#define MAIN_MENU_MOD_INDEX         4
#define MAIN_MENU_LOCAL_OSC_INDEX   5

#define MAIN_MENU_LAST_INDEX MAIN_MENU_LOCAL_OSC_INDEX

//*********************************

int M, K, H, A, MenuPos;
int DBCorrection = 0;

#define VOLTAGE_DIVIDER_FACTOR 0.1754
#define ANALOG_REFERENCE_VOLTAGE 4950
#define INPUT_VOLTAGE_PIN A3 //VIN_MEAS 

bool isPWR_DWN = false;
//bool needsUpdateDisplay = false;

void setup()
{
  pinMode(TCXO_POWER_PIN, OUTPUT);
  pinMode(TCXO_PATH_PIN, OUTPUT);
  pinMode(EXTERANL_SRC_PATH_PIN, OUTPUT);
  digitalWrite(TCXO_POWER_PIN, LOW);
  digitalWrite(TCXO_PATH_PIN, LOW);
  digitalWrite(EXTERANL_SRC_PATH_PIN, LOW);

  pinMode(TRG_OUT_PIN, OUTPUT);
  pinMode(TRG_LED_PIN, OUTPUT);
  digitalWrite(TRG_OUT_PIN, LOW);
  digitalWrite(TRG_LED_PIN, LOW);

  display = Adafruit_SSD1306(128, 64, &Wire);
  //display.setRotation(2);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  DisplayHello();
  delay(3000);

  Serial.begin(115200);

  Serial.println(F("DDS AD9910 Arduino Shield v3.5 by GRA & AFCH. (gra-afch.com)"));
  Serial.print(F("Firmware v.:"));
  Serial.println(FIRMWAREVERSION);

  Serial.print(F("Input voltage = "));
  Serial.println(getInputVoltage());

  if ((getInputVoltage() < 4) ||
      (getInputVoltage() > 8)) 
      {
        displayPowerWarning();
        while(1);
      }

  RFOutButton.debounceTime   = 25;   // Debounce timer in ms
  RFOutButton.multiclickTime = 1;  // Time limit for multi clicks
  RFOutButton.longClickTime  = 1000; // time until "held-down clicks" register
  //RFOutButton.Update();

  modeButton.Update();
  if (modeButton.depressed == true) //если при включении была зажата ручка энкодера, то затираем управляющие флаги в EEPROM, которые восстановят заводские значения всех параметров
  {
    EEPROM.write(CLOCK_SETTINGS_FLAG_ADR, 255); //flag that force save default clock settings to EEPROM
    EEPROM.write(MAIN_SETTINGS_FLAG_ADR, 255); //flag that force save default main settings to EEPROM
    EEPROM.write(MODULATION_SETTINGS_FLAG_ADR, 255); //flag that force save default modulation settings to EEPROM
    EEPROM.write(LO_SETTINGS_FLAG_ADR, 255); 
  }

  LoadMainSettings();
  LoadClockSettings();
  LoadModulationSettings();
  LoadLOSettings();

  MakeOut();

  MenuPos = 0;
  modeButton.debounceTime   = 25;   // Debounce timer in ms
  modeButton.multiclickTime = 1;  // Time limit for multi clicks
  modeButton.longClickTime  = 1000; // time until "held-down clicks" register

  DrawMainMenu();
}

int ModMenuPos = 0;
int ModIndex = 0;
bool EditMode = false;
String ModName[5] = {"None", "AM", "FM", "Sweep", "S-Curve"};
int MFreqK = 0;
int MFreqH = 0;
int AMDepth = 0;
int FMDevK = 0;
int FMDevH = 0;

#define DEFUALT_SWEEP_START_FREQ 100000
#define DEFUALT_SWEEP_END_FREQ 200000

#define MAX_SWEEP_FREQ 600000000UL
#define MIN_SWEEP_FREQ 100000

#define MIN_SWEEP_TIME 10 //10 nS

int SweepStartFreqM = 0;
int SweepStartFreqK = 0;
int SweepStartFreqH = 0;
int SweepStartFreq = DEFUALT_SWEEP_START_FREQ;

int SweepEndFreqM = 0;
int SweepEndFreqK = 0;
int SweepEndFreqH = 0;
int SweepEndFreq = DEFUALT_SWEEP_END_FREQ;

int SweepTime = 1;
int SweepTimeFormat = 0;
String TimeFormatsNames[4] = {"S", "mS", "uS", "nS"};

int32_t S_CurveCarrier = 250000; //144000000; 
int32_t S_CurveBandwith = 300; //3; //in KhZ!!!

#define MAX_S_CURVE_CARRIER HIGH_FREQ_LIMIT
#define MIN_S_CURVE_CARRIER LOW_FREQ_LIMIT

#define MAX_S_CURVE_BANDWITH 300
#define MIN_S_CURVE_BANDWITH 3

volatile int TRG_STATE = 0;

uint32_t UpButtonPressed = 0;
uint32_t DownButtonPressed = 0;
int increment = 1;
int decrement = 1;

int8_t curPos=0;

#define LO_MENU_MHZ_INDEX       0
#define LO_MENU_KHZ_INDEX       1
#define LO_MENU_HZ_INDEX        2
#define LO_MENU_STEP_INDEX      3
#define LO_MENU_IF_SIGN_INDEX   4
#define LO_MENU_IF_INDEX        5
#define LO_MENU_EXIT_INDEX      6

#define LO_MENU_LAST_INDEX LO_MENU_EXIT_INDEX

//int32_t LOCarrierFreq = 144000000;
uint32_t LOStep = 0;

int8_t LOMenuPos = 0;

uint32_t LOStepsList[] = {1000, 5000, 25000, 50000, 100000};
//int8_t LOStepsIndex = 1;
const uint8_t LOStepsQty = sizeof(LOStepsList) / sizeof(LOStepsList[0]);
uint32_t IFFreqList[] = {450000, 455000, 465000, 5500000, 6500000, 10700000, 21400000, 21700000, 38400000, 70000000, 90000000, 100000000};
//int8_t IFFreqListIndex = 0;
const uint8_t IFFreqQty = sizeof(IFFreqList) / sizeof(IFFreqList[0]);
uint32_t IFFreq = 0;
//int32_t RealCarrierFreq = 0; 
//int8_t IFSign = -1;
bool needToSaveLOSettings = false;
uint32_t lastTimeLOSettingsChanged = 0;

struct LOSettings
{
  int32_t LOCarrierFreq = 144000000;
  int8_t LOStepsIndex = 1;
  int8_t IFFreqListIndex = 0;
  int32_t RealCarrierFreq = 0; 
  int8_t IFSign = -1;
} loSettings;
  
void loop ()
{
  int functionUpButton = 0;
  int functionDownButton = 0;

  static int LastUpButtonState = 1;
  static int LastDownButtonState = 1;

  while (1)
  {
    ReadSerialCommands();
    curPos=0;
    curPos=myEnc.read();

    RFOutButton.Update();
    modeButton.Update(); // read state button encoder

/**************************************************************************/
    if (modeButton.clicks > 0) // short click button encoder
    {
      switch(MenuPos)
      {
      case MAIN_MENU_MOD_INDEX: 
        Modultaion_Menu();
      break;
      case MAIN_MENU_LOCAL_OSC_INDEX: 
        LocalOscillator_Menu();
      break;
      default:
        EditMode = !EditMode;
      }
    }
/**************************************************************************/

/**************************************************************************/
    if (modeButton.clicks < 0) DDS_Clock_Config_Menu(); // long click button encoder
/**************************************************************************/

    if (RFOutButton.clicks > 0) // short click 
    {
      isPWR_DWN = !isPWR_DWN;
      if (isPWR_DWN) TurnDDSPowerOFF(); //digitalWrite(DDS_PWR_DWN_PIN, HIGH);
        else TurnDDSPowerON();//digitalWrite(DDS_PWR_DWN_PIN, LOW);
    }

/********************Menu Moving Cursor **************************************/
    if (EditMode == false) //перемещение по менню
    { 
      if (curPos > 0) 
      {
        MenuPos++;
        if (MenuPos > MAIN_MENU_LAST_INDEX) MenuPos = MAIN_MENU_MHZ_INDEX;
      }
      if (curPos < 0) 
      {
        MenuPos--;
        if (MenuPos < MAIN_MENU_MHZ_INDEX) MenuPos = MAIN_MENU_LAST_INDEX;
      }
    } else //изменение параметров
    {
      if (curPos > 0) //в большую сторону
      {
        //Serial.print("curPos=");
        //Serial.println(curPos);
        if (MenuPos == 0) {
          if (Check(M + curPos, K, H)) M = Inc(M, curPos);
        }
        if (MenuPos == 1) {
          if (Check(M, K + curPos, H)) K = Inc(K, curPos);
        }
        if (MenuPos == 2) {
          if (Check(M, K, H + curPos)) H = Inc(H, curPos);
        }
        if (MenuPos == 3)
        {
          A = A - curPos;
          if (A < 0) A = 0;
        }
       // if (MenuPos == 4) Modultaion_Menu();
      }
      if (curPos < 0) //в меньшую сторону
      {
        //Serial.print("curPos=");
        //Serial.println(curPos);
        if (MenuPos == 0) {
          if (Check(M + curPos, K, H)) M = Dec(M, curPos);
        }
        if (MenuPos == 1) {
          if (Check(M, K + curPos, H)) K = Dec(K, curPos);
        }
        if (MenuPos == 2) {
          if (Check(M, K, H + curPos)) H = Dec(H, curPos);
        }
        if (MenuPos == 3)
        {
          A = A + curPos*-1;
          if (A > 72) A = 72; // Amplitude low limit
        }
        //if (MenuPos == 4) Modultaion_Menu();
      }
    }

/**************************************************************************/
    if (curPos != 0) 
    {
      SaveMainSettings();
      MakeOut(); // <-- сделать задержку для AM и FM модуляции и для сохранения настроек!!!!!!
    }

    //UpdateDisplay();
    DrawMainMenu();

  }
}

/************
   MakeOut - решает какую из функци генерации выбрать (в зависимости от значения ModIndex)
 ********/
void MakeOut()
{ 
  StopSCurve();
  switch (ModIndex)
  {
    case NONE_MOD_TYPE: //модуляция отключена
      SingleProfileFreqOut(M * 1000000L + K * 1000L + H, A * -1);
      //SingleProfileFreqOut(1000, A*-1); //DELETE THIS
      break;
    case AM_MOD_TYPE: // AM амплитудная модуляция
      SaveAMWavesToRAM(M * 1000000L + K * 1000L + H, MFreqK * 1000L + MFreqH, AMDepth, A * -1);
      break;
    case FM_MOD_TYPE: // FM частотная модуляция
      SaveFMWavesToRAM(M * 1000000L + K * 1000L + H, MFreqK * 1000L + MFreqH, FMDevK * 1000L + FMDevH);
      break;
    case SWEEP_MOD_TYPE:
      Sweep(GetSweepStartFreq(), GetSweepEndFreq(), SweepTime, SweepTimeFormat);
      break;
    case LO_MOD_TYPE:
      SingleProfileFreqOut(loSettings.RealCarrierFreq, A * -1);
      break;
    case S_CURVE_MOD_TYPE:
      digitalWrite(TRG_LED_PIN, HIGH);
      Sweep(S_CurveCarrier - (S_CurveBandwith * 1000) / 2, S_CurveCarrier + (S_CurveBandwith * 1000) / 2, S_CurveBandwith, 1, false);
      //Sweep(100000, 400000, 3, 1, false);
      //Sweep(GetSweepStartFreq(), GetSweepEndFreq(), SweepTime, SweepTimeFormat);
      StartSCurve();
      break;
  }
}

void DisplayHello()
{
  display.clearDisplay();
  display.cp437(true);
  display.setTextSize(2);
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);
  display.print("Hint:");
  display.setTextSize(1);
  display.setCursor(62, 0);
  display.print("Firmware");
  display.setCursor(62, 8);
  display.print("ver.: ");
  display.print(FIRMWAREVERSION);
  display.setCursor(0, 16);

  display.setTextSize(2);
  display.setCursor(6, 16);
  display.print("Push");
  display.setCursor(58, 16);
  display.print("&");
  display.setCursor(74, 16);
  display.print("hold");
  display.setCursor(22, 33);
  display.print("ENCODER");
  display.setCursor(17, 49);
  display.print("to SETUP");

  display.display();
}



/*int Inc(int val)
{
  uint32_t FreqVal = M * 1000000 + K * 1000 + H;
  val = val + increment;
  if (val > 999) val = 999;
  return val;
}

int Dec(int val)
{
  uint32_t FreqVal = M * 1000000 + K * 1000 + H;
  val = val - decrement;
  if (val < 0) val = 0;
  return val;
}*/

int32_t Inc(int32_t val, int incr)
{
  //val = val + increment;
  //val = val + incrValue;
  val = val + incr;
  if (val > 999) val = 999;
  return val;
}

int32_t Dec(int32_t val, int decr)
{
  //val = val - decrement;
  //val = val - decrValue;
  val = val + decr;
  if (val < 0) val = 0;
  return val;
}


/*****************************************************************

 * **************************************************************/


bool Check (int _M, int _K, int _H)
{
  long F_Val;
  F_Val = _M * 1000000L + _K * 1000L + _H;
  if ((F_Val >= LOW_FREQ_LIMIT) && (F_Val <= HIGH_FREQ_LIMIT)) return 1;
  else return 0;
}



void SaveMainSettings()
{
  EEPROM.put(M_ADR, M);
  EEPROM.put(K_ADR, K);
  EEPROM.put(H_ADR, H);
  EEPROM.put(A_ADR, A);
  EEPROM.write(MAIN_SETTINGS_FLAG_ADR, 55);
}

/**************************************************************************

 *************************************************************************/
void LoadMainSettings()
{
  if (EEPROM.read(MAIN_SETTINGS_FLAG_ADR) != 55)
  {
    M = INIT_M;
    K = INIT_K;
    H = INIT_H;
    A = INIT_A;
    SaveMainSettings();
#if DBG==1
    Serial.println(F("Loading init values"));
    Serial.print("M=");
    Serial.println(M);
    Serial.print("K=");
    Serial.println(K);
    Serial.print("H=");
    Serial.println(H);
    Serial.print("A=");
    Serial.println(A);
#endif
  }
  else
  {
    EEPROM.get(M_ADR, M);
    EEPROM.get(K_ADR, K);
    EEPROM.get(H_ADR, H);
    EEPROM.get(A_ADR, A);
#if DBG==1
    Serial.println(F("Value from EEPROM"));
    Serial.print("M=");
    Serial.println(M);
    Serial.print("K=");
    Serial.println(K);
    Serial.print("H=");
    Serial.println(H);
    Serial.print("A=");
    Serial.println(A);
#endif
  }
}

/*
   Расчет корректировки в dBm используется только для AM модуляции, значение зависит ТОЛЬКО от глубины модуляции
*/
int CalcDBCorrection()
{
  float ArmsREAL = (1 + AMDepth / 100.0) * 0.2236;
  float P_REAL = ArmsREAL * ArmsREAL / 50.0;
  DBCorrection = round(10 * log10(P_REAL / 0.001));
  return DBCorrection;
}

//**********
// Return SweepStartFreq in HZ
//**********
uint32_t GetSweepStartFreq()
{
#if DBG==1
  Serial.print("SweepStartFreqM=");
  Serial.println(SweepStartFreqM);
  Serial.print("SweepStartFreqK=");
  Serial.println(SweepStartFreqK);
  Serial.print("SweepStartFreqH=");
  Serial.println(SweepStartFreqH);
#endif
  return SweepStartFreqM * 1000000UL + SweepStartFreqK * 1000UL + SweepStartFreqH;
}

//**********
// Return SweepEndFreq in HZ
//**********
uint32_t GetSweepEndFreq()
{
  return SweepEndFreqM * 1000000UL + SweepEndFreqK * 1000UL + SweepEndFreqH;
}

void DisplayMessage(String Title, String Message)
{
  display.clearDisplay();
  display.cp437(true);
  display.setTextSize(2);
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);
  display.print(Title);
  display.setTextSize(1);
  display.setCursor(5, 28);
  display.print(Message);

  display.display();
}

void SetSweepStartFreq(uint32_t freq)
{
  SweepStartFreqH = freq % 1000;
  freq = freq / 1000;
  SweepStartFreqK = freq % 1000;
  freq = freq / 1000;
  SweepStartFreqM = freq;
}

void SetSweepEndFreq(uint32_t freq)
{
  SweepEndFreqH = freq % 1000;
  freq = freq / 1000;
  SweepEndFreqK = freq % 1000;
  freq = freq / 1000;
  SweepEndFreqM = freq;
}

bool IsSweepFreqsValid() //проверяет правильность введеных частот для свипа, и в случае обнаружения проблем исправляет их, в том числе меняет местами началььную и конечные частоты если, начальная больше конечной
{
  bool IsValid = true;
  uint32_t tempFreq;

  if (GetSweepStartFreq() > GetSweepEndFreq())
  {
    tempFreq = GetSweepStartFreq();
    SetSweepStartFreq(GetSweepEndFreq());
    SetSweepEndFreq(tempFreq);
    DisplayMessage("SWEEP", "Starting Frequency\r\n Higher Than\r\n Stop Frequency!"); //\r\n
    delay(3000);
    IsValid = false;
  }

  if (GetSweepStartFreq() > MAX_SWEEP_FREQ)
  {
    SetSweepStartFreq(MAX_SWEEP_FREQ);
    DisplayMessage("SWEEP", "Too High\r\n Start Frequency!");
    delay(2500);
    //ModMenuPos=MOD_MENU_SWEEP_START_FREQ_M_INDEX;
    IsValid = false;
  }

  if (GetSweepStartFreq() < MIN_SWEEP_FREQ)
  {
    SetSweepStartFreq(MIN_SWEEP_FREQ);
    DisplayMessage("SWEEP", "Too Low\r\n Start Frequency!");
    delay(2500);
    //ModMenuPos=MOD_MENU_SWEEP_START_FREQ_M_INDEX;
    IsValid = false;
  }

  if (GetSweepEndFreq() > MAX_SWEEP_FREQ)
  {
    SetSweepEndFreq(MAX_SWEEP_FREQ);
    DisplayMessage("SWEEP", "Too High\r\n Stop Frequency!");
    delay(2500);
    //ModMenuPos=MOD_MENU_SWEEP_END_FREQ_M_INDEX;
    IsValid = false;
  }

  if (GetSweepEndFreq() < MIN_SWEEP_FREQ)
  {
    SetSweepEndFreq(MIN_SWEEP_FREQ);
    DisplayMessage("SWEEP", "Too Low\r\n Stop Frequency!");
    delay(2500);
    //ModMenuPos=MOD_MENU_SWEEP_END_FREQ_M_INDEX;
    IsValid = false;
  }

  if (GetSweepStartFreq() == GetSweepEndFreq())
  {
    //SetSweepEndFreq(MIN_SWEEP_FREQ);
    DisplayMessage("SWEEP", "Frequencies\r\n are Equal!");
    delay(2500);
    IsValid = false;
  }
  return IsValid;
}

bool IsSweepTimeTooLong()
{
  //GetSweepStartFreq(), GetSweepEndFreq(), SweepTime, SweepTimeFormat
  uint32_t DeltaFTW = FreqToFTW(GetSweepEndFreq()) - FreqToFTW(GetSweepStartFreq());
  //float GHZ_CoreClock=DDS_Core_Clock/1E9; //незабыть заменит на CalcRealDDSCoreClockFromOffset();
  float GHZ_CoreClock = CalcRealDDSCoreClockFromOffset() / 1E9;
  uint64_t MaxPossibleNanoSweepTime = (4 / GHZ_CoreClock * DeltaFTW) * 0xFFFF; //умножаем на 0xFFFF (максимальное значение StepRate) для того чтобы узнать максиамльно возможное время свипа для заданного интервала частот (при текущей частоте ядра)
#if DBG==1
  Serial.print("MaxPossibleNanoSweepTime=");
  print64(MaxPossibleNanoSweepTime);
  Serial.print("GetSweepTime()=");
  print64(GetSweepTime());
#endif
  if (GetSweepTime() > MaxPossibleNanoSweepTime) //проверяем чтобы не получилось так чтобы введеное время не оказалось дольше максимально возможного для заданного диапазона частот и текущей частоты ядра
  {
    SetSweepTime(MaxPossibleNanoSweepTime);
    DisplayMessage("SWEEP", F("Too Long Time"));
    delay(2500);
    return true;
  }
  else
  {
    return false;
  }
}

uint64_t GetSweepTime() // возвращает время в наносекунадх указанное пользователем в меню
{
  if (SweepTimeFormat == 0) return SweepTime * 1E9;
  else if (SweepTimeFormat == 1) return SweepTime * 1E6;
  else if (SweepTimeFormat == 2) return SweepTime * 1E3;
}

void SetSweepTime(uint64_t NanoSweepTime)
{
  if (NanoSweepTime < 1E3) //1000
  {
    SweepTime = NanoSweepTime;
    SweepTimeFormat = 3; //nS
  } else if (NanoSweepTime < 1E6) //1000000
  {
    SweepTime = NanoSweepTime / 1E3;
    SweepTimeFormat = 2; //uS
  } else if (NanoSweepTime < 1E9) //1000000000
  {
    SweepTime = NanoSweepTime / 1E6;
    SweepTimeFormat = 1; //uS
  } else
  {
    SweepTime = NanoSweepTime / 1E9;
    SweepTimeFormat = 0; //S
  }
}

#if DBG==1
void print64(uint64_t value)
{
  const int NUM_DIGITS = log10(value) + 1;

  char sz[NUM_DIGITS + 1];

  sz[NUM_DIGITS] =  0;
  for ( size_t i = NUM_DIGITS; i--; value /= 10)
  {
    sz[i] = '0' + (value % 10);
  }

  Serial.println(sz);
}
#endif

float getInputVoltage()
{
  uint16_t voltage=0; //mV
  analogReference(DEFAULT); 
  delay(10);
  voltage = map(analogRead(INPUT_VOLTAGE_PIN), 0, 1023, 0, ANALOG_REFERENCE_VOLTAGE); //4.95
  return float(voltage) / 1000.0 / VOLTAGE_DIVIDER_FACTOR;
}

void displayPowerWarning()
{
  static const unsigned char PROGMEM image_alert_bicubic_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x07,0xe0,0x00,0x00,0x00,0x00,0x07,0xe0,0x00,0x00,0x00,0x00,0x0f,0xf0,0x00,0x00,0x00,0x00,0x0e,0x70,0x00,0x00,0x00,0x00,0x1c,0x38,0x00,0x00,0x00,0x00,0x3c,0x3c,0x00,0x00,0x00,0x00,0x38,0x1c,0x00,0x00,0x00,0x00,0x78,0x1e,0x00,0x00,0x00,0x00,0x70,0x0e,0x00,0x00,0x00,0x00,0xf0,0x0f,0x00,0x00,0x00,0x00,0xe3,0xc7,0x00,0x00,0x00,0x01,0xc3,0xc3,0x80,0x00,0x00,0x03,0xc3,0xc3,0xc0,0x00,0x00,0x03,0x83,0xc1,0xc0,0x00,0x00,0x07,0x83,0xc1,0xe0,0x00,0x00,0x07,0x03,0xc0,0xe0,0x00,0x00,0x0e,0x03,0xc0,0xf0,0x00,0x00,0x0e,0x03,0xc0,0x70,0x00,0x00,0x1c,0x03,0xc0,0x38,0x00,0x00,0x3c,0x03,0xc0,0x3c,0x00,0x00,0x38,0x03,0xc0,0x1c,0x00,0x00,0x78,0x03,0xc0,0x1e,0x00,0x00,0x70,0x03,0xc0,0x0e,0x00,0x00,0xe0,0x03,0xc0,0x07,0x00,0x01,0xe0,0x03,0xc0,0x07,0x80,0x01,0xc0,0x03,0xc0,0x03,0x80,0x03,0xc0,0x01,0x80,0x03,0xc0,0x03,0x80,0x00,0x00,0x01,0xc0,0x07,0x80,0x00,0x00,0x01,0xe0,0x07,0x00,0x01,0x80,0x00,0xe0,0x0e,0x00,0x03,0xc0,0x00,0x70,0x1e,0x00,0x03,0xc0,0x00,0x78,0x1c,0x00,0x01,0x80,0x00,0x38,0x3c,0x00,0x00,0x00,0x00,0x3c,0x38,0x00,0x00,0x00,0x00,0x1c,0x38,0x00,0x00,0x00,0x00,0x1c,0x3f,0xff,0xff,0xff,0xff,0xfc,0x1f,0xff,0xff,0xff,0xff,0xf8,0x01,0xff,0xff,0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(23, 1);
  display.print("WARNING");
  display.setTextWrap(false);
  display.setCursor(43, 17);
  display.print("7.5 VDC");
  display.setTextSize(1);
  display.setCursor(49, 35);
  display.print("POWER SUPPLY");
  display.setTextSize(2);
  display.setCursor(72, 45);
  display.print("USB");
  display.drawBitmap(1, 11, image_alert_bicubic_bits, 48, 48, 1);
  display.setTextSize(1);
  display.setCursor(55, 48);
  display.print("or");
  display.display();
}

void selectClockSrcPath(uint8_t path)
{
  switch (path)
  {
    case CLOCK_SOURCE_XO_INDEX:
      digitalWrite(TCXO_POWER_PIN, HIGH);
      digitalWrite(TCXO_PATH_PIN, LOW);
      digitalWrite(EXTERANL_SRC_PATH_PIN, LOW);
    break;
    case CLOCK_SOURCE_TCXO_INDEX:
      digitalWrite(TCXO_POWER_PIN, HIGH);
      digitalWrite(EXTERANL_SRC_PATH_PIN, LOW);
      digitalWrite(TCXO_PATH_PIN, HIGH);
    break;
    case CLOCK_SOURCE_EXT_TCXO_INDEX:
      digitalWrite(TCXO_POWER_PIN, LOW);
      digitalWrite(TCXO_PATH_PIN, LOW);
      digitalWrite(EXTERANL_SRC_PATH_PIN, HIGH);
    break;
    case CLOCK_SOURCE_EXT_OSC_INDEX:
      digitalWrite(TCXO_POWER_PIN, LOW);
      digitalWrite(TCXO_PATH_PIN, LOW);
      digitalWrite(EXTERANL_SRC_PATH_PIN, HIGH);
    break;    
  }
}

// Прерывание А таймера 1
ISR(TIMER1_A) {  
  
  switch (TRG_STATE)
  {
    case 0:
      // отправляем настройки в DDS по SPI (значения нужно просчитать заранее)
      // иди дергаем ножкой 
      Timer1.pause();
      RestartDRG();
      Timer1.restart();
      PORTL &= ~(1 << PORTL1); //OFF TRG_OUT
    break;
    case 1:
      //ничего не отправляем по SPI
      PORTL |= (1 << PORTL1); //ON RG_OUT
    break;
  }

  TRG_STATE =! TRG_STATE;
  /*PORTL |= (1 << PORTL1); //ON
  PORTL &= ~(1 << PORTL1); //OFF*/
}

void StartSCurve()
{
  Timer1.setPeriod(S_CurveBandwith * 1000 / 2);               // Высокоточный таймер 1 для первого прерывания
  //Timer1.setFrequency(SYNC_PULSE_FREQ);
  Timer1.enableISR(CHANNEL_A);
  //PORTL |= (1 << PORTL1); //on
  //TRG_STATE = 1; //SyncPulse
}

void StopSCurve()
{
  Timer1.stop();
  PORTL &= ~(1 << PORTL1); //OFF
}

#if DBG==1
void readRegs()
{
    //*********DBG*********
    // DDS_GPIO_Init();
    // HAL_GPIO_WritePin(DDS_MASTER_RESET_GPIO_PORT, DDS_MASTER_RESET_PIN, GPIO_PIN_SET);   // RESET = 1
    // //HAL_Delay(10);
    // delay(100);
    // HAL_GPIO_WritePin(DDS_MASTER_RESET_GPIO_PORT, DDS_MASTER_RESET_PIN, GPIO_PIN_RESET); // RESET = 0
    // HAL_GPIO_WritePin(DDS_IO_UPDATE_GPIO_PORT, DDS_IO_UPDATE_PIN, GPIO_PIN_RESET);       // IO_UPDATE = 0   
    // HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_SET);               // CS = 1
    // HAL_GPIO_WritePin(DDS_OSK_GPIO_PORT, DDS_OSK_PIN, GPIO_PIN_SET);                     // OSK = 1
    // HAL_GPIO_WritePin(DDS_PROFILE_0_GPIO_PORT, DDS_PROFILE_0_PIN, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(DDS_PROFILE_1_GPIO_PORT, DDS_PROFILE_1_PIN, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(DDS_PROFILE_2_GPIO_PORT, DDS_PROFILE_2_PIN, GPIO_PIN_RESET);
    // DDS_SPI_Init();
    // HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_RESET);    
    // SPI.transfer(CFR1_addr);
    // SPI.transfer(0); //strBuffer[1] = 0;// RAM_enable;//RAM_Playback_Amplitude;// | RAM_enable;//0x00; 
    // SPI.transfer(0); //strBuffer[2] = 0;//Inverse_sinc_filter_enable;//0; //Continuous_Profile_0_1; //0;//0x80;//0x00;
    // SPI.transfer(0); //strBuffer[3] = 0; //OSK_enable | Select_auto_OSK;//0x00;
    // SPI.transfer(SDIO_input_only); //strBuffer[4] = SDIO_input_only ;
    // //  HAL_SPI_Transmit(&hspi1, (uint8_t*)strBuffer, 5, 1000);
    // HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_SET);
    // DDS_UPDATE();
    // readRegs();
  //********************
  HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_RESET);    
      //Serial.println(SPI.transfer(CFR1_addr | B10000000));
      Serial.println("CFR1");
      SPI.transfer(CFR1_addr | B10000000);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
  HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_SET); 
  HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_RESET);    
      //Serial.println(SPI.transfer(CFR2_addr | B10000000));
      Serial.println("CFR2");
      SPI.transfer(CFR2_addr | B10000000);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
  HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_SET); 
  HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_RESET);    
      //Serial.println(SPI.transfer(CFR3_addr | B10000000));
      Serial.println("CFR3");
      SPI.transfer(CFR3_addr | B10000000);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
      Serial.println(SPI.transfer(0), HEX);
  HAL_GPIO_WritePin(DDS_SPI_CS_GPIO_PORT, DDS_SPI_CS_PIN, GPIO_PIN_SET); 
  // 19:40:00.913 -> CFR1
  // 19:40:00.913 -> 0
  // 19:40:00.913 -> 0
  // 19:40:00.913 -> 0
  // 19:40:00.913 -> 2
  // 19:40:00.913 -> CFR2
  // 19:40:00.913 -> 1
  // 19:40:00.913 -> 0
  // 19:40:00.913 -> 0
  // 19:40:00.913 -> 20
  // 19:40:00.913 -> CFR3
  // 19:40:00.913 -> 3
  // 19:40:00.913 -> 38
  // 19:40:00.913 -> 41
  // 19:40:00.913 -> 28
}

#endif

void TurnDDSPowerOFF()
{
  Serial.println(F("Output Disabled"));
  digitalWrite(DDS_PWR_DWN_PIN, HIGH);
  isPWR_DWN = true;
}

void TurnDDSPowerON()
{
  Serial.println(F("Output Enabled"));
  digitalWrite(DDS_PWR_DWN_PIN, LOW);
  delay(500);
  MakeOut();
  isPWR_DWN = false;
}
