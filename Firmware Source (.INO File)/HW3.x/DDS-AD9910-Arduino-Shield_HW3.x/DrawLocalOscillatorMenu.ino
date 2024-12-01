void DrawLocalOscillatorMenu()
{
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(1, 0);
  if (LOMenuPos == LO_MENU_MHZ_INDEX) GetColor();
    else display.setTextColor(WHITE);
  //display.print("144");
  display.print(PreZero((loSettings.LOCarrierFreq / (uint32_t)1e6)));
  display.setTextColor(WHITE);
  display.drawLine(0, 16, 126, 16, 1);
  display.setCursor(46, 0);
  if (LOMenuPos == LO_MENU_KHZ_INDEX) GetColor();
    else display.setTextColor(WHITE);
  //display.print("205");
  display.print(PreZero(round(loSettings.LOCarrierFreq / (uint32_t)1e3) % 1000));
  display.setTextColor(WHITE);
  display.setCursor(34, 1);
  display.print(".");
  display.setCursor(79, 1);
  display.print(".");
  display.setCursor(90, 1);
  if (LOMenuPos == LO_MENU_HZ_INDEX) GetColor();
    else display.setTextColor(WHITE);
  //display.print("000");
  display.print(PreZero(loSettings.LOCarrierFreq % 1000));
  display.setTextColor(WHITE);
  display.drawLine(0, 35, 126, 35, 1);
  display.setCursor(1, 18);
  display.setTextSize(1);
  display.print(F("LO Freq:"));
  //uint32_t RealCarrierFreq = LOCarrierFreq - IFFreqList[IFFreqListIndex];
  display.print(PreZero((loSettings.RealCarrierFreq / (uint32_t)1e6)));
  display.print(F("."));
  display.print(PreZero((loSettings.RealCarrierFreq / (uint32_t)1e3) % 1000));
  display.print(F("."));
  display.print(PreZero(loSettings.RealCarrierFreq % 1000));

  if (LOMenuPos == LO_MENU_STEP_INDEX) GetColor();
    else display.setTextColor(WHITE);
  //display.print("12.5kHz");
  LOStep = LOStepsList[loSettings.LOStepsIndex] / 1000;
  if (LOStep < 10) display.setCursor(42, 26);
    else if (LOStep < 100) display.setCursor(36, 26);
      else display.setCursor(30, 26);
  display.print(LOStep);
  display.print(F("kHz"));
  display.setTextColor(WHITE);
  display.setCursor(0, 26);
  display.print("Step:");
  display.drawLine(69, 26, 69, 32, 1);
  display.drawLine(1, 45, 126, 45, 1);
  //display.setCursor(89, 26);
  if (LOMenuPos == LO_MENU_IF_INDEX) GetColor();
    else display.setTextColor(WHITE);
  formatLOFreqText();

  display.setTextColor(WHITE);
  display.setCursor(72, 26);
  display.print("IF:");

  if (LOMenuPos == LO_MENU_IF_SIGN_INDEX) GetColor();
    else display.setTextColor(WHITE);
  if (loSettings.IFSign < 0) display.print(F("-"));
    else display.print(F("+"));

  display.drawRect(13, 47, 2, 7, 1);
  uint8_t MarkerXPos = map((loSettings.RealCarrierFreq / (uint32_t)1e6), 0, HIGH_FREQ_LIMIT / (uint32_t)1e6, 1, 125);
  display.drawRect(MarkerXPos, 38, 2, 6, 1);
  display.drawRect(1, 47, 2, 7, 1);
  display.drawRect(5, 47, 2, 7, 1);
  display.drawRect(9, 47, 2, 7, 1);
  display.drawRect(17, 47, 2, 7, 1);
  display.drawRect(21, 47, 2, 7, 1);
  display.drawRect(25, 47, 2, 7, 1);
  display.drawRect(29, 47, 2, 7, 1);
  display.drawRect(33, 47, 2, 7, 1);
  display.drawRect(37, 47, 2, 7, 1);
  display.drawRect(41, 47, 2, 7, 1);
  display.drawRect(45, 47, 2, 7, 1);
  display.drawRect(49, 47, 2, 7, 1);
  display.drawRect(53, 47, 2, 7, 1);
  display.drawRect(57, 47, 2, 7, 1);
  display.drawRect(61, 47, 2, 7, 1);
  display.drawRect(65, 47, 2, 7, 1);
  display.drawRect(69, 47, 2, 7, 1);
  display.drawRect(73, 47, 2, 7, 1);
  display.drawRect(77, 47, 2, 7, 1);
  display.drawRect(81, 47, 2, 7, 1);
  display.drawRect(85, 47, 2, 7, 1);
  display.drawRect(89, 47, 2, 7, 1);
  display.drawRect(93, 47, 2, 7, 1);
  display.drawRect(97, 47, 2, 7, 1);
  display.drawRect(101, 47, 2, 7, 1);
  display.drawRect(105, 47, 2, 7, 1); 
  display.drawRect(109, 47, 2, 7, 1);
  display.drawRect(113, 47, 2, 7, 1);
  display.drawRect(117, 47, 2, 7, 1);
  display.drawRect(121, 47, 2, 7, 1);
  display.drawRect(125, 47, 2, 7, 1);

  display.setCursor(104, 56);
  if (LOMenuPos == LO_MENU_EXIT_INDEX) GetColor();
    else display.setTextColor(WHITE);
  display.print("Exit");
  display.setTextColor(WHITE);

  display.setCursor(1, 56);
  display.print("GRA&AFCH");

  display.display();
}

void formatLOFreqText()
{
  display.setCursor(120, 26);
  IFFreq = IFFreqList[loSettings.IFFreqListIndex] / 1000;
  if (IFFreq >= 1000) 
  {
    display.print(F("M"));
    IFFreq = IFFreq / 100;
    if (IFFreq % 10 == 0)
    {
      if (IFFreq < 100) display.setCursor(114, 26);
        else if (IFFreq < 1000) display.setCursor(108, 26);
          else display.setCursor(102, 26);
      display.print(IFFreq / 10);
    } else
      {
        if (IFFreq < 100) display.setCursor(102, 26);
          else if (IFFreq < 1000) display.setCursor(96, 26);
            else display.setCursor(84, 26);
        display.print(IFFreq / 10);
        display.print(F("."));
        display.print(IFFreq % 10);
      }
  } else 
    {
      display.print(F("k"));
      if (IFFreq < 10) display.setCursor(114, 26);
          else if (IFFreq < 100) display.setCursor(108, 26);
            else display.setCursor(102, 26);
      display.print(IFFreq);
    }
}