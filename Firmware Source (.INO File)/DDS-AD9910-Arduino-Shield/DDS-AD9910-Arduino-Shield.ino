/*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *******************************************
    Для любой модуляции нужно сначала вызывать фнукцию calcBestStepRate перед PrepRegistersToSaveWaveForm,
    зачастую это так и сдлеано внутри функций SaveAMWavesToRAM и SaveFMWavesToRAM
    v.2.20 //29.10.2024 Улучшена стабильность при включении (подаче питания)
    v.2.19 //16.04.2024 Added Voltage Warning Messge At Startup
    v.2.18 //30.01.2024 VCO3 changed to VCO5
    v.2.17 //22.06.2023 Отображение на экране состояния выхода - OFF, если выключен
    v.2.16 //21.06.2023 Добавлены команды включения и выключения выхода (E, D)
    v.2.15 //20.06.2023 Ускорена обработка комманд
    v.2.14 //07.06.2023 Добавлена поддержка управления через последовательный порт
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
#define FIRMWAREVERSION 2.20

#define LOW_FREQ_LIMIT  100000
#define HIGH_FREQ_LIMIT  600000000

#include <GParser.h>

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

#define MAIN_SETTINGS_FLAG_ADR 100 // defualt settings 
// ADR 101 reserved for clock settings
#define MODULATION_SETTINGS_FLAG 102 // defualt settings

#define INIT_M 100
#define INIT_K 0
#define INIT_H 0
#define INIT_A 0

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

#define MOD_MENU_SAVE_INDEX 13
#define MOD_MENU_EXIT_INDEX 14

#define NONE_MOD_TYPE 0
#define AM_MOD_TYPE 1
#define FM_MOD_TYPE 2
#define SWEEP_MOD_TYPE 3
#define LSB_MOD_TYPE 4
#define USB_MOD_TYPE 5

//*********************************

int M, K, H, A, MenuPos;
int DBCorrection = 0;

bool isPWR_DWN = false;

void setup()
{
  display = Adafruit_SSD1306(128, 64, &Wire);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //display.clearDisplay();
  DisplayPowerWarning();
  delay(3000);
  DisplayHello();
  delay(3000);

  Serial.begin(115200);

  Serial.println(F("DDS AD9910 Arduino Shield by GRA & AFCH. (gra-afch.com)"));
  Serial.print(F("Firmware v.:"));
  Serial.println(FIRMWAREVERSION);

  downButton.Update();

  if (downButton.depressed == true) //если при включении была зажата кнопка DOWN, то затираем управляющие флаги в EEPROM, которые восстановят заводские значения всех параметров
  {
    EEPROM.write(CLOCK_SETTINGS_FLAG_ADR, 255); //flag that force save default clock settings to EEPROM
    EEPROM.write(MAIN_SETTINGS_FLAG_ADR, 255); //flag that force save default main settings to EEPROM
    EEPROM.write(MODULATION_SETTINGS_FLAG, 255); //flag that force save default modulation settings to EEPROM
  }

  LoadMain();
  LoadClockSettings();
  LoadModulationSettings();

  MakeOut();

  MenuPos = 0;
  modeButton.debounceTime   = 25;   // Debounce timer in ms
  modeButton.multiclickTime = 1;  // Time limit for multi clicks
  modeButton.longClickTime  = 1000; // time until "held-down clicks" register

  upButton.debounceTime   = 75;   // Debounce timer in ms*/
  upButton.multiclickTime = 1;  // Time limit for multi clicks*/
  upButton.longClickTime  = 1000; // time until "held-down clicks" register*/

  downButton.debounceTime   = 75;   // Debounce timer in ms
  downButton.multiclickTime = 1;  // Time limit for multi clicks
  downButton.longClickTime  = 1000; // time until "held-down clicks" register

  UpdateDisplay();
}

int ModMenuPos = 0;
int ModIndex = 0;
String ModName[4] = {"None", "AM", "FM", "Sweep"};
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

uint32_t UpButtonPressed = 0;
uint32_t DownButtonPressed = 0;
int increment = 1;
int decrement = 1;

void loop ()
{
  int functionUpButton = 0;
  int functionDownButton = 0;

  static int LastUpButtonState = 1;
  static int LastDownButtonState = 1;

  while (1)
  {
    ReadSerialCommands();
    LastUpButtonState = upButton.depressed;
    LastDownButtonState = downButton.depressed;
    modeButton.Update();
    upButton.Update();
    downButton.Update();

    if (upButton.clicks != 0) functionUpButton = upButton.clicks;

    if (upButton.depressed == true)
    {
      if ((millis() - UpButtonPressed) > 2000) increment = 10; else increment = 1;
    } else UpButtonPressed = millis();
    if ((functionUpButton == 1 && upButton.depressed == false) ||
        (functionUpButton == -1 && upButton.depressed == true))
    {
      if (MenuPos == 0) {
        if (Check (M + increment, K, H)) M = Inc(M);
      }
      if (MenuPos == 1) {
        if (Check (M, K + increment, H)) K = Inc(K);
      }
      if (MenuPos == 2) {
        if (Check (M, K, H + increment)) H = Inc(H);
      }
      if (MenuPos == 3)
      {
        A = A - 1;
        if (A < 0) A = 0;
      }
      if (MenuPos == 4) Modultaion_Menu();
      UpdateDisplay();
    }
    if (upButton.depressed == false) functionUpButton = 0;


    if (downButton.clicks != 0) functionDownButton = downButton.clicks;

    if (downButton.depressed == true)
    {
      if ((millis() - DownButtonPressed) > 2000) decrement = 10; else decrement = 1;
    } else DownButtonPressed = millis();

    if ((functionDownButton == 1 && downButton.depressed == false) ||
        (functionDownButton == -1 && downButton.depressed == true))
    {
      if (MenuPos == 0) {
        if (Check(M - decrement, K, H)) M = Dec(M);
      }
      if (MenuPos == 1) {
        if (Check(M, K - decrement, H)) K = Dec(K);
      }
      if (MenuPos == 2) {
        if (Check(M, K, H - decrement)) H = Dec(H);
      }
      if (MenuPos == 3)
      {
        A = A + 1;
        if (A > 72) A = 72; // Amplitude low limit
      }
      if (MenuPos == 4) Modultaion_Menu();
      UpdateDisplay();
    }
    if (downButton.depressed == false) functionDownButton = 0;

    if (modeButton.clicks > 0)
    {
      MenuPos++;
      if (MenuPos > 4) MenuPos = 0;
      UpdateDisplay();
      SaveMain();
    }

    if (modeButton.clicks < 0) DDS_Clock_Config_Menu();

    if ((LastUpButtonState != upButton.depressed) &&
        (upButton.depressed == false))
    {
      MakeOut();
      SaveMain();
    }

    if ((LastDownButtonState != downButton.depressed) &&
        (downButton.depressed == false))
    {
      MakeOut();
      SaveMain();
    }
  }
}

/************
   MakeOut - решает какую из функци генерации выбрать (в зависимости от значения ModIndex)
 ********/
void MakeOut()
{
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
  display.print(" Hold ");
  //display.setTextColor(BLACK, WHITE);
  display.println("MODE");
  display.setTextColor(WHITE);
  display.println(" to enter");
  display.println("   setup");

  display.display();
}

void UpdateDisplay()
{
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println(F("DDS AD9910"));

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 16);     // Start at top-left corner

  display.print(F("Frequency, [Hz]:"));
  if (isPWR_DWN) display.print(F(" OFF"));

  display.setTextSize(2);      // Normal 1:1 pixel scale
  if (MenuPos == 0) display.setTextColor(BLACK, WHITE);
  else display.setTextColor(WHITE);
  display.setCursor(1, 26);

  display.print(PreZero(M));
  if (MenuPos == 1) display.setTextColor(BLACK, WHITE);
  else display.setTextColor(WHITE);
  display.print(PreZero(K));
  if (MenuPos == 2) display.setTextColor(BLACK, WHITE);
  else display.setTextColor(WHITE);
  display.println(PreZero(H));

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 42);     // Start at top-left corner

  display.print(F("Amplitude:"));

  display.print("  Mod:"); //NONE");
  if (MenuPos == 4) display.setTextColor(BLACK, WHITE);
  else display.setTextColor(WHITE);
  display.println(ModName[ModIndex]);

  display.drawLine(68, 42, 68, 64, WHITE);

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(1, 50);     // Start at top-left corner

  if (MenuPos == 3) display.setTextColor(BLACK, WHITE);
  else display.setTextColor(WHITE);
  if (ModIndex != AM_MOD_TYPE) DBCorrection = 0;
  else DBCorrection = CalcDBCorrection();
  if (DACCurrentIndex == 0)
  {
    display.print("-");
    display.print(PreZero(abs(A + DBCorrection)));
  } else
  {
    if ((-1 * A + 4 - DBCorrection) > 0) display.print("+");
    else display.print("-");
    display.print(PreZero(abs(-1 * A + 4 - DBCorrection)));
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48, 56);
  display.println("dBM");

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE);
  display.setCursor(72, 56);     // Start at top-left corner

  display.println(F("GRA&AFCH"));
  display.display();
}

String PreZero(int Digit)
{
  if ((Digit < 100) && (Digit >= 10)) return "0" + String(Digit);
  if (Digit < 10) return "00" + String(Digit);
  return String(Digit);
}

int Inc(int val)
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

static const unsigned char PROGMEM cos_values[] = {22, 22, 22, 21, 21, 20, 20, 19, 18, 17, 16, 15, 13, 12, 11, 10, 9, 7, 6, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 20, 21, 21, 22, 22};
static const unsigned char PROGMEM sin_values[] = {11, 12, 13, 15, 16, 17, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 22, 21, 21, 20, 20, 19, 18, 17, 16, 15, 13, 12, 11, 10, 9, 7, 6, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 9, 10, 11};
static const unsigned char PROGMEM hifreq[] = {22, 19, 11, 3, 0, 3, 10, 16, 18, 14, 8, 2, 0, 2, 5, 8, 9, 6, 3, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 3, 6, 9, 8, 6, 2, 0, 2, 8, 14, 18, 16, 10, 3, 0, 3, 11, 19};

void displayModulationMenu()
{
  display.clearDisplay();

  if (ModIndex == AM_MOD_TYPE)
  {
    for (int x = 0; x < 56; x++)
    {
      display.drawPixel(x + 35, pgm_read_byte_near(cos_values + x) + 40, WHITE);
      display.drawPixel(x + 35, pgm_read_byte_near(cos_values + x) + 41, WHITE);

      display.drawPixel(x + 35, 40 + 22 - pgm_read_byte_near(cos_values + x), WHITE);
      display.drawPixel(x + 35, 41 + 22 - pgm_read_byte_near(cos_values + x), WHITE);

      //display.drawPixel(x+25, pgm_read_byte_near(hifreq + x)+40, WHITE);
      // if (x<55) display.drawLine(x+25, pgm_read_byte_near(hifreq + x)+40, x+25+1, pgm_read_byte_near(hifreq + x+1)+40, WHITE);
    }

    for (int x = 0; x < 56; x = x + 3)
    {
      display.drawLine(x + 35, pgm_read_byte_near(cos_values + x) + 40, x + 35, 40 + 22 - pgm_read_byte_near(cos_values + x), WHITE);
    }
  }

  if (ModIndex == FM_MOD_TYPE)
  {
    for (int x = 0; x < 56; x++)
    {
      display.drawPixel(x + 35, 40 + 22 - pgm_read_byte_near(sin_values + x), WHITE);
      display.drawPixel(x + 35, 41 + 22 - pgm_read_byte_near(sin_values + x), WHITE);
    }

    for (int x = 0; x < 53; x = x + 6 - (pgm_read_byte_near(sin_values + x) / 6))
    {
      display.drawLine(x + 35, 40, x + 35 + 3 - (pgm_read_byte_near(sin_values + x) / 6), 40 + 22, WHITE);
      display.drawLine(x + 35 + 3 - (pgm_read_byte_near(sin_values + x) / 6), 40 + 22, x + 35 + 6 - (pgm_read_byte_near(sin_values + x + 1) / 6), 40, WHITE);
    }
  }

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Modulation");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 16);
  display.print("Type:");
  display.print(" ");
  if (ModMenuPos == 0) display.setTextColor(BLACK, WHITE);
  display.print(ModName[ModIndex]);
  display.setTextColor(WHITE);

  if ((ModIndex == AM_MOD_TYPE) || ((ModIndex == FM_MOD_TYPE)))
  {
    display.setCursor(0, 24);
    display.print("MFreq:");
    display.setTextColor(WHITE);
    display.print(" ");
    if (ModMenuPos == MOD_MENU_MFREQ_K_INDEX) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(MFreqK));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_MFREQ_H_INDEX) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(MFreqH));
    display.setTextColor(WHITE);
    display.print("Hz");

    display.setCursor(0, 32);
    if (ModIndex == AM_MOD_TYPE)
    {
      display.print("Depth:");
      display.print(" ");
      if (ModMenuPos == MOD_MENU_DEPTH_DEV_K_INDEX) display.setTextColor(BLACK, WHITE);
      display.print(AMDepth);
      display.setTextColor(WHITE);
      display.print("%");
    }
    if (ModIndex == FM_MOD_TYPE )
    {
      display.print("Deviation:");
      display.print(" ");
      if (ModMenuPos == MOD_MENU_DEPTH_DEV_K_INDEX) display.setTextColor(BLACK, WHITE);
      display.print(PreZero(FMDevK));
      display.setTextColor(WHITE);
      if (ModMenuPos == MOD_MENU_FM_DEV_H_INDEX) display.setTextColor(BLACK, WHITE);
      display.print(PreZero(FMDevH));
      display.setTextColor(WHITE);
      display.print("Hz");
    }
  }

  if (ModIndex == SWEEP_MOD_TYPE)
  {
    display.setCursor(0, 24);
    display.setTextColor(WHITE);
    display.print("Start Freq.:");
    //display.print(" ");
    if (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_M_INDEX) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(SweepStartFreqM));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_K_INDEX) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(SweepStartFreqK));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_H_INDEX) display.setTextColor(BLACK, WHITE);
    display.println(PreZero(SweepStartFreqH));
    display.setTextColor(WHITE);

    display.print("Stop Freq.:");
    display.print(" ");
    if (ModMenuPos == MOD_MENU_SWEEP_END_FREQ_M_INDEX) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(SweepEndFreqM));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_END_FREQ_K_INDEX) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(SweepEndFreqK));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_END_FREQ_H_INDEX) display.setTextColor(BLACK, WHITE);
    display.println(PreZero(SweepEndFreqH));
    display.setTextColor(WHITE);

    display.print("Time:");
    display.print(" ");
    if (ModMenuPos == MOD_MENU_SWEEP_TIME_INDEX) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(SweepTime));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_TIME_FORMAT_INDEX ) display.setTextColor(BLACK, WHITE);
    display.print(TimeFormatsNames[SweepTimeFormat]);
    display.setTextColor(WHITE);

  }

  display.setCursor(0, 55);
  if (ModMenuPos == MOD_MENU_SAVE_INDEX) display.setTextColor(BLACK, WHITE);
  display.println("SAVE");

  display.setTextColor(WHITE);
  display.setCursor(103, 55);
  if (ModMenuPos == MOD_MENU_EXIT_INDEX) display.setTextColor(BLACK, WHITE);
  display.println("EXIT");

  display.display();
}

void Modultaion_Menu()
{
  int functionUpButton = 0;
  int functionDownButton = 0;

  static int LastUpButtonState = 1;
  static int LastDownButtonState = 1;

  upButton.debounceTime = 25;
  downButton.debounceTime = 25;

  while (1)
  {
    LastUpButtonState = upButton.depressed;
    LastDownButtonState = downButton.depressed;

    modeButton.Update();
    upButton.Update();
    downButton.Update();

    if (modeButton.clicks != 0) ModMenuPos++;
    if (ModMenuPos > MOD_MENU_EXIT_INDEX) ModMenuPos = MOD_MENU_TYPE_INDEX;
    if ((ModIndex == NONE_MOD_TYPE) && (ModMenuPos == MOD_MENU_MFREQ_K_INDEX)) ModMenuPos = MOD_MENU_SAVE_INDEX; //jump to save buton if modulation is none and MODE button was pressed
    if ((ModIndex == AM_MOD_TYPE) && (ModMenuPos == MOD_MENU_FM_DEV_H_INDEX)) ModMenuPos = MOD_MENU_SAVE_INDEX ; //jump to save buton if modulation is AM and in Depth position and MODE button was pressed
    if ((ModIndex == SWEEP_MOD_TYPE) && (ModMenuPos == MOD_MENU_MFREQ_K_INDEX)) ModMenuPos = MOD_MENU_SWEEP_START_FREQ_M_INDEX; //jump to Sweep Start Freq position when sweep enabled
    if ((ModIndex != SWEEP_MOD_TYPE) && (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_M_INDEX)) ModMenuPos = MOD_MENU_SAVE_INDEX; //jump to Save position when not in sweep mode

    if (upButton.clicks != 0) functionUpButton = upButton.clicks;
    if ((functionUpButton == 1 && upButton.depressed == false) ||
        (functionUpButton == -1 && upButton.depressed == true))
    {
      switch (ModMenuPos)
      {
        case MOD_MENU_TYPE_INDEX:
          ModIndex++;
          if (ModIndex > SWEEP_MOD_TYPE) ModIndex = 0;
          break;
        case MOD_MENU_MFREQ_K_INDEX :
          MFreqK++;
          if (MFreqK > 100) MFreqK = 0;
          if ((MFreqK == 0) && (MFreqH < 10)) MFreqK = 1;
          break;
        case MOD_MENU_MFREQ_H_INDEX :
          MFreqH++;
          if (MFreqH > 999) MFreqH = 0;
          if ((MFreqK == 0) && (MFreqH < 10)) MFreqH = 10;
          break;
        case MOD_MENU_DEPTH_DEV_K_INDEX :
          if (ModIndex == AM_MOD_TYPE)
          {
            AMDepth++;
            if (AMDepth > 100) AMDepth = 0;
          }
          if (ModIndex == FM_MOD_TYPE)
          {
            FMDevK++;
            if (FMDevK > 100) FMDevK = 0;
          }
          break;
        case MOD_MENU_FM_DEV_H_INDEX :
          if (ModIndex == FM_MOD_TYPE)
          {
            FMDevH++;
            if (FMDevH > 999) FMDevH = 0;
          }
          break;
        case MOD_MENU_SWEEP_START_FREQ_M_INDEX :
          SweepStartFreqM++;
          if (SweepStartFreqM > 600) SweepStartFreqM = 0;
          break;
        case MOD_MENU_SWEEP_START_FREQ_K_INDEX :
          SweepStartFreqK++;
          if (SweepStartFreqK > 999) SweepStartFreqK = 0;
          break;
        case MOD_MENU_SWEEP_START_FREQ_H_INDEX :
          SweepStartFreqH++;
          if (SweepStartFreqH > 999) SweepStartFreqH = 0;
          break;
        case MOD_MENU_SWEEP_END_FREQ_M_INDEX :
          SweepEndFreqM++;
          if (SweepEndFreqM > 600) SweepEndFreqM = 0;
          break;
        case MOD_MENU_SWEEP_END_FREQ_K_INDEX :
          SweepEndFreqK++;
          if (SweepEndFreqK > 999) SweepEndFreqK = 0;
          break;
        case MOD_MENU_SWEEP_END_FREQ_H_INDEX :
          SweepEndFreqH++;
          if (SweepEndFreqH > 999) SweepEndFreqH = 0;
          break;
        case MOD_MENU_SWEEP_TIME_INDEX :
          SweepTime++;
          if (SweepTime > 999) SweepTime = 1;
          break;
        case MOD_MENU_SWEEP_TIME_FORMAT_INDEX :
          SweepTimeFormat++;
          if (SweepTimeFormat > 3) SweepTimeFormat = 0;
          break;
        case MOD_MENU_SAVE_INDEX:
          if ((ModIndex == SWEEP_MOD_TYPE) && (IsSweepFreqsValid() == false)) break;
          if ((ModIndex == SWEEP_MOD_TYPE) && (IsSweepTimeTooLong() == true)) break;
          SaveModulationSettings(); DisplaySaved(); MakeOut(); delay(1000); ModMenuPos = MOD_MENU_EXIT_INDEX; break;
        case MOD_MENU_EXIT_INDEX: UpdateDisplay(); LoadModulationSettings(); MakeOut(); return; break;
      }
    }
    if (upButton.depressed == false) functionUpButton = 0;

    //******DOWN*******************
    if (downButton.clicks != 0) functionDownButton = downButton.clicks;
    if ((functionDownButton == 1 && downButton.depressed == false) ||
        (functionDownButton == -1 && downButton.depressed == true))
    {
      switch (ModMenuPos)
      {
        case 0:
          ModIndex--;
          if (ModIndex < 0) ModIndex = SWEEP_MOD_TYPE ;
          break;
        case 1:
          MFreqK--;
          if (MFreqK < 0) MFreqK = 100;
          if ((MFreqK < 1) && (MFreqH < 10)) MFreqK = 100;
          break;
        case 2:
          MFreqH--;
          if (MFreqH < 0) MFreqH = 999;
          if ((MFreqK < 1) && (MFreqH < 10)) MFreqH = 999;
          break;
        case 3:
          if (ModIndex == 1)
          {
            AMDepth--;
            if (AMDepth < 0) AMDepth = 100;
          }
          if (ModIndex == 2)
          {
            FMDevK--;
            if (FMDevK < 0) FMDevK = 100;
          }
          break;
        case 4:
          if (ModIndex == 2)
          {
            FMDevH--;
            if (FMDevH < 0) FMDevH = 999;
          }
          break;
        case MOD_MENU_SWEEP_START_FREQ_M_INDEX :
          SweepStartFreqM--;
          if (SweepStartFreqM < 0) SweepStartFreqM = 600;
          break;
        case MOD_MENU_SWEEP_START_FREQ_K_INDEX :
          SweepStartFreqK--;
          if (SweepStartFreqK < 0) SweepStartFreqK = 999;
          break;
        case MOD_MENU_SWEEP_START_FREQ_H_INDEX :
          SweepStartFreqH--;
          if (SweepStartFreqH < 0) SweepStartFreqH = 999;
          break;
        case MOD_MENU_SWEEP_END_FREQ_M_INDEX :
          SweepEndFreqM--;
          if (SweepEndFreqM < 0) SweepEndFreqM = 600;
          break;
        case MOD_MENU_SWEEP_END_FREQ_K_INDEX :
          SweepEndFreqK--;
          if (SweepEndFreqK < 0) SweepEndFreqK = 999;
          break;
        case MOD_MENU_SWEEP_END_FREQ_H_INDEX :
          SweepEndFreqH--;
          if (SweepEndFreqH < 0) SweepEndFreqH = 999;
          break;
        case MOD_MENU_SWEEP_TIME_INDEX :
          SweepTime--;
          if (SweepTime < 1) SweepTime = 999;
          break;
        case MOD_MENU_SWEEP_TIME_FORMAT_INDEX :
          SweepTimeFormat--;
          if (SweepTimeFormat < 0) SweepTimeFormat = 3;
          break;
        case MOD_MENU_SAVE_INDEX:
          if ((ModIndex == SWEEP_MOD_TYPE) && (IsSweepFreqsValid() == false)) break;
          if ((ModIndex == SWEEP_MOD_TYPE) && (IsSweepTimeTooLong() == true)) break;
          SaveModulationSettings(); DisplaySaved(); MakeOut(); delay(1000); ModMenuPos = MOD_MENU_EXIT_INDEX; break;
        case MOD_MENU_EXIT_INDEX: UpdateDisplay(); LoadModulationSettings(); MakeOut(); return; break;
      }
    }
    if (downButton.depressed == false) functionDownButton = 0;

    /// Updating DDS when button was released
    if ((LastUpButtonState != upButton.depressed) &&
        (upButton.depressed == false))
    {
      MakeOut();
    }

    if ((LastDownButtonState != downButton.depressed) &&
        (downButton.depressed == false))
    {
      MakeOut();
    }
    //***End Updating DDS
    displayModulationMenu();
  }
}

void SaveModulationSettings()
{
  EEPROM.put(MOD_INDEX_ADR, ModIndex);
  EEPROM.put(MOD_FREQK_ADR, MFreqK);
  EEPROM.put(MOD_FREQH_ADR, MFreqH);
  EEPROM.put(MOD_AM_DEPTH_ADR, AMDepth);
  EEPROM.put(MOD_FM_DEVK_ADR, FMDevK);
  EEPROM.put(MOD_FM_DEVH_ADR, FMDevH);

  //*******SWEEP VARIABLES
  EEPROM.put(SWEEP_START_FREQ_M_ADR, SweepStartFreqM);
  EEPROM.put(SWEEP_START_FREQ_K_ADR, SweepStartFreqK);
  EEPROM.put(SWEEP_START_FREQ_H_ADR, SweepStartFreqH);

  EEPROM.put(SWEEP_END_FREQ_M_ADR, SweepEndFreqM);
  EEPROM.put(SWEEP_END_FREQ_K_ADR, SweepEndFreqK);
  EEPROM.put(SWEEP_END_FREQ_H_ADR, SweepEndFreqH);

  EEPROM.put(SWEEP_TIME_ADR, SweepTime);
  EEPROM.put(SWEEP_TIME_FORMAT_ADR, SweepTimeFormat);


  EEPROM.write(MODULATION_SETTINGS_FLAG, 56);
}

void LoadModulationSettings()
{
  SweepStartFreqM = 100; // del this
  SweepEndFreqM = 200; // del this
  if (EEPROM.read(MODULATION_SETTINGS_FLAG) != 56)
  {
    ModIndex = INIT_MOD_INDEX;
    MFreqK = INIT_MFREQ_K;
    MFreqH = INIT_MFREQ_H;
    AMDepth = INIT_AM_DEPTH;
    FMDevK = INIT_FM_DEV_K;
    FMDevH = INIT_FM_DEV_H;
    //*********SWEEP**********
    SweepStartFreqM = INIT_SWEEP_START_FREQ_M;
    SweepStartFreqK = INIT_SWEEP_START_FREQ_K;
    SweepStartFreqH = INIT_SWEEP_START_FREQ_H;

    SweepEndFreqM = INIT_SWEEP_END_FREQ_M;
    SweepEndFreqK = INIT_SWEEP_END_FREQ_K;
    SweepEndFreqH = INIT_SWEEP_END_FREQ_H;

    SweepTime = INIT_SWEEP_TIME;
    SweepTimeFormat = INIT_SWEEP_TIME_FORMAT;

    SaveModulationSettings();
  } else
  {
    EEPROM.get(MOD_INDEX_ADR, ModIndex);
    EEPROM.get(MOD_FREQK_ADR, MFreqK);
    EEPROM.get(MOD_FREQH_ADR, MFreqH);
    EEPROM.get(MOD_AM_DEPTH_ADR, AMDepth);
    EEPROM.get(MOD_FM_DEVK_ADR, FMDevK);
    EEPROM.get(MOD_FM_DEVH_ADR, FMDevH);
    //**********SWEEP*****************
    EEPROM.get(SWEEP_START_FREQ_M_ADR, SweepStartFreqM);
    EEPROM.get(SWEEP_START_FREQ_K_ADR, SweepStartFreqK);
    EEPROM.get(SWEEP_START_FREQ_H_ADR, SweepStartFreqH);

    EEPROM.get(SWEEP_END_FREQ_M_ADR, SweepEndFreqM);
    EEPROM.get(SWEEP_END_FREQ_K_ADR, SweepEndFreqK);
    EEPROM.get(SWEEP_END_FREQ_H_ADR, SweepEndFreqH);

    EEPROM.get(SWEEP_TIME_ADR, SweepTime);
    EEPROM.get(SWEEP_TIME_FORMAT_ADR, SweepTimeFormat);
  }
}

void SaveMain()
{
  EEPROM.put(M_ADR, M);
  EEPROM.put(K_ADR, K);
  EEPROM.put(H_ADR, H);
  EEPROM.put(A_ADR, A);
  EEPROM.write(MAIN_SETTINGS_FLAG_ADR, 55);
}

/**************************************************************************

 *************************************************************************/
void LoadMain()
{
  if (EEPROM.read(MAIN_SETTINGS_FLAG_ADR) != 55)
  {
    M = INIT_M;
    K = INIT_K;
    H = INIT_H;
    A = INIT_A;
    SaveMain();
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

void DisplayPowerWarning()
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
