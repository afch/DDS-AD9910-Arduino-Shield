
static const unsigned char PROGMEM cos_values[] = {22, 22, 22, 21, 21, 20, 20, 19, 18, 17, 16, 15, 13, 12, 11, 10, 9, 7, 6, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 20, 21, 21, 22, 22};
static const unsigned char PROGMEM sin_values[] = {11, 12, 13, 15, 16, 17, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 22, 21, 21, 20, 20, 19, 18, 17, 16, 15, 13, 12, 11, 10, 9, 7, 6, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 9, 10, 11};
static const unsigned char PROGMEM hifreq[] = {22, 19, 11, 3, 0, 3, 10, 16, 18, 14, 8, 2, 0, 2, 5, 8, 9, 6, 3, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 3, 6, 9, 8, 6, 2, 0, 2, 8, 14, 18, 16, 10, 3, 0, 3, 11, 19};

const unsigned char sCurve_graph [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x01, 0xe0, 0xfc, 0x00, 0x00, 0x3c, 0x00, 0x00, 0xfe, 0xfe, 
	0x00, 0x00, 0x78, 0x00, 0x00, 0x7e, 0x07, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x01, 
	0xc0, 0x00, 0x00, 0x00, 0x01, 0x80, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0f, 0x3f, 0xff, 
	0xff, 0xfe, 0x00, 0xe0, 0x3c, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x7b, 0xf8, 0x30, 0x00, 0x00, 0x00, 
	0x00, 0x3f, 0xe0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x30, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 
	0x00, 0x00, 0x00
};

void DrawModulationMenu()
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

  if (ModIndex == S_CURVE_MOD_TYPE)
  {
    display.drawBitmap(35, 42, sCurve_graph, 55, 21, WHITE);
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
  if (ModMenuPos == 0) GetColor(); //display.setTextColor(BLACK, WHITE);
  display.print(ModName[ModIndex]);
  display.setTextColor(WHITE);

  if ((ModIndex == AM_MOD_TYPE) || ((ModIndex == FM_MOD_TYPE)))
  {
    display.setCursor(0, 24);
    display.print("MFreq:");
    display.setTextColor(WHITE);
    display.print(" ");
    if (ModMenuPos == MOD_MENU_MFREQ_K_INDEX) GetColor();
    display.print(PreZero(MFreqK));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_MFREQ_H_INDEX) GetColor();
    display.print(PreZero(MFreqH));
    display.setTextColor(WHITE);
    display.print("Hz");

    display.setCursor(0, 32);
    if (ModIndex == AM_MOD_TYPE)
    {
      display.print("Depth:");
      display.print(" ");
      if (ModMenuPos == MOD_MENU_DEPTH_DEV_K_INDEX) GetColor();
      display.print(AMDepth);
      display.setTextColor(WHITE);
      display.print("%");
    }
    if (ModIndex == FM_MOD_TYPE )
    {
      display.print("Deviation:");
      display.print(" ");
      if (ModMenuPos == MOD_MENU_DEPTH_DEV_K_INDEX) GetColor();
      display.print(PreZero(FMDevK));
      display.setTextColor(WHITE);
      if (ModMenuPos == MOD_MENU_FM_DEV_H_INDEX) GetColor();
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
    if (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_M_INDEX) GetColor();
    display.print(PreZero(SweepStartFreqM));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_K_INDEX) GetColor();
    display.print(PreZero(SweepStartFreqK));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_H_INDEX) GetColor();
    display.println(PreZero(SweepStartFreqH));
    display.setTextColor(WHITE);

    display.print("Stop Freq.:");
    display.print(" ");
    if (ModMenuPos == MOD_MENU_SWEEP_END_FREQ_M_INDEX) GetColor();
    display.print(PreZero(SweepEndFreqM));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_END_FREQ_K_INDEX) GetColor();
    display.print(PreZero(SweepEndFreqK));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_END_FREQ_H_INDEX) GetColor();
    display.println(PreZero(SweepEndFreqH));
    display.setTextColor(WHITE);

    display.print("Time:");
    display.print(" ");
    if (ModMenuPos == MOD_MENU_SWEEP_TIME_INDEX) GetColor();
    display.print(PreZero(SweepTime));
    display.setTextColor(WHITE);
    if (ModMenuPos == MOD_MENU_SWEEP_TIME_FORMAT_INDEX ) GetColor();
    display.print(TimeFormatsNames[SweepTimeFormat]);
    display.setTextColor(WHITE);

  }

  if (ModIndex == S_CURVE_MOD_TYPE)
  {
    display.setCursor(0, 24);
    display.print(F("Car"));
    display.setCursor(19, 24);
    display.print(F("rie"));
    display.setCursor(38, 24);
    display.print(F("r"));
    display.setCursor(44, 25);
    display.print(F(":"));
    
    display.setCursor(50, 24);
    if (ModMenuPos == MOD_MENU_CARRIER_M_INDEX ) GetColor();
    display.print(PreZero(S_CurveCarrier / 1000000)); // MHz
    display.setTextColor(WHITE);
    display.setCursor(67, 24);
    display.print(F("."));
    display.setCursor(73, 24);
    if (ModMenuPos == MOD_MENU_CARRIER_K_INDEX ) GetColor();
    display.print(PreZero((S_CurveCarrier / 1000) % 1000)); //kHz
    display.setTextColor(WHITE);
    display.setCursor(90, 24);
    display.print(F("."));
    display.setCursor(96, 24);
    if (ModMenuPos == MOD_MENU_CARRIER_H_INDEX ) GetColor();
    display.print(PreZero(S_CurveCarrier % 1000)); //Hz
    display.setTextColor(WHITE);
    display.setCursor(0, 32);
    display.print(F("Bandw"));
    display.setCursor(29, 32);
    display.print(F("i"));
    display.setCursor(33, 32);
    display.print(F("th"));
    display.setCursor(44, 32);
    display.print(F(":"));
    display.setCursor(50, 32);
    if (ModMenuPos == MOD_MENU_BW_INDEX ) GetColor();
    display.print(PreZero(S_CurveBandwith));
    display.setTextColor(WHITE);
    display.setCursor(69, 32);
    display.print(F("kHz"));
    display.setCursor(89, 32);
    display.print(F("="));
      
    display.setCursor(97, 32);
    display.print(PreZero(S_CurveBandwith));;
    display.setCursor(116, 32);
    display.print(F("mS"));
    
    display.setCursor(0, 41);
    display.print(F("RF"));
    display.write(0x1A);
    display.print(F("CH1"));

    display.setCursor(86, 41);
    display.print(F("TRG"));
    display.write(0x1A);
    display.print(F("CH2"));

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