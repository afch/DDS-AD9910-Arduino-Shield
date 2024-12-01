void GetColor()
{
  static uint32_t lastTimeColorChangedTime=millis();
  static uint8_t color=1; //1 - white (normal color), 0 - black;
  if (EditMode == false)
  {
    display.setTextColor(BLACK, WHITE);
    return;
  }
  if ((millis()-lastTimeColorChangedTime)>333)
      {
        lastTimeColorChangedTime=millis();
        color=!color;
      }
  if (color == 1)
  {
    display.setTextColor(WHITE);
  } else 
  {
    display.setTextColor(BLACK, WHITE);
  }
}

void DrawMainMenu()
{
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 1);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println(F("DDS"));
  display.setCursor(40, 1);
  display.print(F("9910"));

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setCursor(88, 8);
  display.print("v3");
  display.setCursor(104, 0);
  display.print(F("GRA&"));
  display.setCursor(104, 8);
  display.print(F("AFCH"));

  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 16);     // Start at top-left corner

  display.println(F("Frequency, [Hz]:"));

  display.setCursor(100, 16);     
  if (isPWR_DWN) display.print(F("OFF"));
    else display.print(F("ON"));

  display.setTextSize(2);      // Normal 1:1 pixel scale
  if (MenuPos == MAIN_MENU_MHZ_INDEX) GetColor();
    else display.setTextColor(WHITE);
  display.setCursor(1, 26);
  display.print(PreZero(M));

  display.setTextColor(WHITE);
  display.setCursor(34, 26);
  display.print(F("."));

  if (MenuPos == MAIN_MENU_KHZ_INDEX) GetColor();
    else display.setTextColor(WHITE);
  display.setCursor(45, 26);
  display.print(PreZero(K));

  display.setTextColor(WHITE);
  display.setCursor(78, 26);
  display.print(F("."));

  display.setCursor(89, 26);
  if (MenuPos == MAIN_MENU_HZ_INDEX) GetColor();
    else display.setTextColor(WHITE);
  display.println(PreZero(H));

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 42);     // Start at top-left corner

  display.print(F("Amplitude"));
  display.setCursor(52, 43);
  display.print(F(":"));

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 50);     // Start at top-left corner

  if (MenuPos == MAIN_MENU_AMPLITUDE_INDEX) GetColor();
    else display.setTextColor(WHITE);
  if (ModIndex != AM_MOD_TYPE) DBCorrection = 0;
    else DBCorrection = CalcDBCorrection();
  if (DACCurrentIndex == 0)
  {
    if ((A + DBCorrection) > 0) display.print(F("-"));
    display.setCursor(12, 50);
    display.print(PreZero2(abs(A + DBCorrection)));
  } else
  {
    if ((-1 * A + 4 - DBCorrection) > 0) display.print(F("+"));
    else if ((-1 * A + 4 - DBCorrection) < 0) display.print(F("-"));
    display.setCursor(12, 50);
    display.print(PreZero(abs(-1 * A + 4 - DBCorrection)));
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(36, 56);
  display.print(F("dBm"));

  display.setCursor(62, 42);     // Start at top-left corner
  display.print(F("Mod")); //NONE");
  display.setCursor(78, 42); 
  display.print(F(":"));

  if (MenuPos == MAIN_MENU_MOD_INDEX) GetColor();
    else display.setTextColor(WHITE);
  display.print(ModName[ModIndex]);

  display.drawLine(57, 42, 57, 64, WHITE);

  /*display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE);
  display.setCursor(72, 56);     // Start at top-left corner

  display.println(F("GRA&AFCH"));*/

  if (MenuPos == MAIN_MENU_LOCAL_OSC_INDEX) GetColor();
    else display.setTextColor(WHITE);
  display.setCursor(62, 56);
  display.print(F("Local Osc"));

  display.display();
}

String PreZero(int Digit)
{
  if ((Digit < 100) && (Digit >= 10)) return "0" + String(Digit);
  if (Digit < 10) return "00" + String(Digit);
  return String(Digit);
}

String PreZero2(int Digit)
{
  if (Digit < 10) return "0" + String(Digit);
  return String(Digit);
}