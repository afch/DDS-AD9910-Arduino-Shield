
void Modultaion_Menu()
{
  uint32_t modParamsTime = millis();
  bool needUpdateOut = true;

  ModMenuPos = MOD_MENU_TYPE_INDEX;

  while (1)
  {
    curPos=0;
    curPos=myEnc.read();

    modeButton.Update(); // read state button encoder

    if (modeButton.clicks > 0) // short click button encoder
    {
      switch (ModMenuPos)
      {
        case MOD_MENU_SAVE_INDEX:
          if ((ModIndex == SWEEP_MOD_TYPE) && (IsSweepFreqsValid() == false)) break;
          if ((ModIndex == SWEEP_MOD_TYPE) && (IsSweepTimeTooLong() == true)) break;
          SaveModulationSettings(); 
          DisplaySaved(); 
          MakeOut(); 
          delay(1000); 
          ModMenuPos = MOD_MENU_EXIT_INDEX; 
        break;
        case MOD_MENU_EXIT_INDEX:
          //UpdateDisplay(); 
          LoadModulationSettings(); 
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
        ModMenuPos++;
        if (ModMenuPos > MOD_MENU_EXIT_INDEX) ModMenuPos = MOD_MENU_TYPE_INDEX;
        if ((ModIndex == NONE_MOD_TYPE) && (ModMenuPos == MOD_MENU_MFREQ_K_INDEX)) ModMenuPos = MOD_MENU_SAVE_INDEX; //jump to save buton if modulation is none and MODE button was pressed
        if ((ModIndex == AM_MOD_TYPE) && (ModMenuPos == MOD_MENU_FM_DEV_H_INDEX)) ModMenuPos = MOD_MENU_SAVE_INDEX ; //jump to save buton if modulation is AM and in Depth position and MODE button was pressed
        if ((ModIndex == SWEEP_MOD_TYPE) && (ModMenuPos == MOD_MENU_MFREQ_K_INDEX)) ModMenuPos = MOD_MENU_SWEEP_START_FREQ_M_INDEX; //jump to Sweep Start Freq position when sweep enabled
        if ((ModIndex != SWEEP_MOD_TYPE) && (ModMenuPos == MOD_MENU_SWEEP_START_FREQ_M_INDEX)) ModMenuPos = MOD_MENU_SAVE_INDEX; //jump to Save position when not in sweep mode
        if ((ModIndex == SWEEP_MOD_TYPE) && (ModMenuPos == MOD_MENU_SWEEP_TIME_FORMAT_INDEX + 1)) ModMenuPos = MOD_MENU_SAVE_INDEX; //jump to Save position
        if ((ModIndex == S_CURVE_MOD_TYPE) && (ModMenuPos == MOD_MENU_MFREQ_K_INDEX)) ModMenuPos = MOD_MENU_CARRIER_M_INDEX;
      }
      if (curPos < 0) 
      { 
        ModMenuPos--;
        if (ModMenuPos < MOD_MENU_TYPE_INDEX) ModMenuPos = MOD_MENU_EXIT_INDEX; 
        if ((ModIndex == NONE_MOD_TYPE) && (ModMenuPos == MOD_MENU_SAVE_INDEX - 1)) ModMenuPos = MOD_MENU_TYPE_INDEX; //jump to TYPE position if modulation is none afer SAVE button
        if ((ModIndex == AM_MOD_TYPE) && (ModMenuPos == MOD_MENU_SAVE_INDEX - 1)) ModMenuPos = MOD_MENU_DEPTH_DEV_K_INDEX ; //jump to AM DEV postion if modulation is AM after SAVE button
        if ((ModIndex == SWEEP_MOD_TYPE) && (ModMenuPos == MOD_MENU_FM_DEV_H_INDEX)) ModMenuPos = MOD_MENU_TYPE_INDEX; //jump to TYPE position when sweep enabled
        if ((ModIndex == SWEEP_MOD_TYPE) && (ModMenuPos == MOD_MENU_SAVE_INDEX - 1)) ModMenuPos = MOD_MENU_SWEEP_TIME_FORMAT_INDEX;
        if ((ModIndex == FM_MOD_TYPE) && (ModMenuPos == MOD_MENU_SAVE_INDEX - 1)) ModMenuPos = MOD_MENU_FM_DEV_H_INDEX; //jump to Save position when not in sweep mode
        if ((ModIndex == S_CURVE_MOD_TYPE) && (ModMenuPos == MOD_MENU_CARRIER_M_INDEX - 1)) ModMenuPos = MOD_MENU_TYPE_INDEX;
      }
    } else //изменение параметров
    {
      if (curPos > 0) 
      {
        switch (ModMenuPos)
        {
          case MOD_MENU_TYPE_INDEX:
            ModIndex++;
            if (ModIndex > S_CURVE_MOD_TYPE) ModIndex = 0;
            /*if (ModIndex == S_CURVE_MOD_TYPE) digitalWrite(TRG_LED_PIN, HIGH);
              else digitalWrite(TRG_LED_PIN, LOW);*/
            break;
          case MOD_MENU_MFREQ_K_INDEX :
            //MFreqK++;
            MFreqK = MFreqK + curPos;
            if (MFreqK > 100) MFreqK = 0;
            if ((MFreqK == 0) && (MFreqH < 10)) MFreqK = 1;
            break;
          case MOD_MENU_MFREQ_H_INDEX :
            //MFreqH++;
            MFreqH = MFreqH + curPos;
            if (MFreqH > 999) MFreqH = 0;
            if ((MFreqK == 0) && (MFreqH < 10)) MFreqH = 10;
            break;
          case MOD_MENU_DEPTH_DEV_K_INDEX :
            if (ModIndex == AM_MOD_TYPE)
            {
              //AMDepth++;
              AMDepth = AMDepth + curPos;
              if (AMDepth > 100) AMDepth = 0;
            }
            if (ModIndex == FM_MOD_TYPE)
            {
              //FMDevK++;
              FMDevK = FMDevK + curPos;
              if (FMDevK > 100) FMDevK = 0;
            }
            break;
          case MOD_MENU_FM_DEV_H_INDEX :
            if (ModIndex == FM_MOD_TYPE)
            {
              //FMDevH++;
              FMDevH = FMDevH + curPos;
              if (FMDevH > 999) FMDevH = 0;
            }
            break;
          case MOD_MENU_SWEEP_START_FREQ_M_INDEX :
            //SweepStartFreqM++;
            SweepStartFreqM = SweepStartFreqM + curPos;
            if (SweepStartFreqM > 600) SweepStartFreqM = 0;
            break;
          case MOD_MENU_SWEEP_START_FREQ_K_INDEX :
            //SweepStartFreqK++;
            SweepStartFreqK = SweepStartFreqK + curPos;
            if (SweepStartFreqK > 999) SweepStartFreqK = 0;
            break;
          case MOD_MENU_SWEEP_START_FREQ_H_INDEX :
            //SweepStartFreqH++;
            SweepStartFreqH = SweepStartFreqH + curPos;
            if (SweepStartFreqH > 999) SweepStartFreqH = 0;
            break;
          case MOD_MENU_SWEEP_END_FREQ_M_INDEX :
            //SweepEndFreqM++;
            SweepEndFreqM = SweepEndFreqM + curPos;
            if (SweepEndFreqM > 600) SweepEndFreqM = 0;
            break;
          case MOD_MENU_SWEEP_END_FREQ_K_INDEX :
            //SweepEndFreqK++;
            SweepEndFreqK = SweepEndFreqK + curPos;
            if (SweepEndFreqK > 999) SweepEndFreqK = 0;
            break;
          case MOD_MENU_SWEEP_END_FREQ_H_INDEX :
            //SweepEndFreqH++;
            SweepEndFreqH = SweepEndFreqH + curPos;
            if (SweepEndFreqH > 999) SweepEndFreqH = 0;
            break;
          case MOD_MENU_SWEEP_TIME_INDEX :
            //SweepTime++;
            SweepTime = SweepTime + curPos;
            if (SweepTime > 999) SweepTime = 1;
            break;
          case MOD_MENU_SWEEP_TIME_FORMAT_INDEX :
            SweepTimeFormat++;
            if (SweepTimeFormat > 3) SweepTimeFormat = 0;
            break;
          case MOD_MENU_CARRIER_M_INDEX:
            S_CurveCarrier = S_CurveCarrier + (uint32_t)curPos * 1000000;
            if (S_CurveCarrier > MAX_S_CURVE_CARRIER) S_CurveCarrier = MAX_S_CURVE_CARRIER;
            break;
          case MOD_MENU_CARRIER_K_INDEX:
            S_CurveCarrier = S_CurveCarrier + (uint32_t)curPos * 1000;
            if (S_CurveCarrier > MAX_S_CURVE_CARRIER) S_CurveCarrier = MAX_S_CURVE_CARRIER;
            break;
          case MOD_MENU_CARRIER_H_INDEX:
            S_CurveCarrier = S_CurveCarrier + curPos;
            if (S_CurveCarrier > MAX_S_CURVE_CARRIER) S_CurveCarrier = MAX_S_CURVE_CARRIER;
            break;
          case MOD_MENU_BW_INDEX:
            S_CurveBandwith = S_CurveBandwith + curPos;
            if (S_CurveBandwith > MAX_S_CURVE_BANDWITH) S_CurveBandwith = MAX_S_CURVE_BANDWITH;
            break;
        }
      } // curPos > 0
      if (curPos < 0)
      {
        switch (ModMenuPos)
        {
          case MOD_MENU_TYPE_INDEX:
            ModIndex--;
            if (ModIndex < 0) ModIndex = S_CURVE_MOD_TYPE ;
            if (ModIndex == S_CURVE_MOD_TYPE) digitalWrite(TRG_LED_PIN, HIGH);
              else digitalWrite(TRG_LED_PIN, LOW);
            break;
          case MOD_MENU_MFREQ_K_INDEX:
            //MFreqK--;
            MFreqK = MFreqK + curPos;
            if (MFreqK < 0) MFreqK = 100;
            if ((MFreqK < 1) && (MFreqH < 10)) MFreqK = 100;
            break;
          case MOD_MENU_MFREQ_H_INDEX:
            //MFreqH--;
            MFreqH = MFreqH + curPos;
            if (MFreqH < 0) MFreqH = 999;
            if ((MFreqK < 1) && (MFreqH < 10)) MFreqH = 999;
            break;
          case MOD_MENU_DEPTH_DEV_K_INDEX:
            if (ModIndex == AM_MOD_TYPE)
            {
              //AMDepth--;
              AMDepth = AMDepth + curPos;
              if (AMDepth < 0) AMDepth = 100;
            }
            if (ModIndex == FM_MOD_TYPE)
            {
              //FMDevK--;
              FMDevK = FMDevK + curPos;
              if (FMDevK < 0) FMDevK = 100;
            }
            break;
          case MOD_MENU_FM_DEV_H_INDEX:
            if (ModIndex == FM_MOD_TYPE)
            {
              //FMDevH--;
              FMDevH = FMDevH + curPos;
              if (FMDevH < 0) FMDevH = 999;
            }
            break;
          case MOD_MENU_SWEEP_START_FREQ_M_INDEX :
            //SweepStartFreqM--;
            SweepStartFreqM = SweepStartFreqM + curPos;
            if (SweepStartFreqM < 0) SweepStartFreqM = 600;
            break;
          case MOD_MENU_SWEEP_START_FREQ_K_INDEX :
            //SweepStartFreqK--;
            SweepStartFreqK = SweepStartFreqK + curPos;
            if (SweepStartFreqK < 0) SweepStartFreqK = 999;
            break;
          case MOD_MENU_SWEEP_START_FREQ_H_INDEX :
            //SweepStartFreqH--;
            SweepStartFreqH = SweepStartFreqH + curPos;
            if (SweepStartFreqH < 0) SweepStartFreqH = 999;
            break;
          case MOD_MENU_SWEEP_END_FREQ_M_INDEX :
            //SweepEndFreqM--;
            SweepEndFreqM = SweepEndFreqM + curPos;
            if (SweepEndFreqM < 0) SweepEndFreqM = 600;
            break;
          case MOD_MENU_SWEEP_END_FREQ_K_INDEX :
            //SweepEndFreqK--;
            SweepEndFreqK = SweepEndFreqK + curPos;
            if (SweepEndFreqK < 0) SweepEndFreqK = 999;
            break;
          case MOD_MENU_SWEEP_END_FREQ_H_INDEX :
            //SweepEndFreqH--;
            SweepEndFreqH = SweepEndFreqH + curPos;
            if (SweepEndFreqH < 0) SweepEndFreqH = 999;
            break;
          case MOD_MENU_SWEEP_TIME_INDEX :
            //SweepTime--;
            SweepTime = SweepTime + curPos;
            if (SweepTime < 1) SweepTime = 999;
            break;
          case MOD_MENU_SWEEP_TIME_FORMAT_INDEX :
            SweepTimeFormat--;
            if (SweepTimeFormat < 0) SweepTimeFormat = 3;
            break;
          case MOD_MENU_CARRIER_M_INDEX:
            S_CurveCarrier = S_CurveCarrier + (uint32_t)curPos * 1000000;
            if (S_CurveCarrier < MIN_S_CURVE_CARRIER) S_CurveCarrier = MIN_S_CURVE_CARRIER;
            break;
          case MOD_MENU_CARRIER_K_INDEX:
            S_CurveCarrier = S_CurveCarrier + (uint32_t)curPos * 1000;
            if (S_CurveCarrier < MIN_S_CURVE_CARRIER) S_CurveCarrier = MIN_S_CURVE_CARRIER;
            break;
          case MOD_MENU_CARRIER_H_INDEX:
            S_CurveCarrier = S_CurveCarrier + curPos;
            if (S_CurveCarrier < MIN_S_CURVE_CARRIER) S_CurveCarrier = MIN_S_CURVE_CARRIER;
            break;
          case MOD_MENU_BW_INDEX:
            S_CurveBandwith = S_CurveBandwith + curPos;
            if (S_CurveBandwith < MIN_S_CURVE_BANDWITH) S_CurveBandwith = MIN_S_CURVE_BANDWITH;
            break;
        }
      } //(curPos < 0)
    }

    if ((curPos != 0) && (EditMode == true)) 
    {
      modParamsTime = millis();
      needUpdateOut = true;
    } else
    {
      if ((millis() - modParamsTime > 500) && (needUpdateOut == true))
      {
        needUpdateOut = false;
        MakeOut();
      }
    }

    DrawModulationMenu();
    
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

  EEPROM.put(S_CURVE_CARRIER_ADR, S_CurveCarrier);
  EEPROM.put(S_CURVE_BANDWITH_ADR, S_CurveBandwith);

  EEPROM.write(MODULATION_SETTINGS_FLAG_ADR, 56);
}

void LoadModulationSettings()
{
  SweepStartFreqM = 100; // del this
  SweepEndFreqM = 200; // del this
  if (EEPROM.read(MODULATION_SETTINGS_FLAG_ADR) != 56)
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

    S_CurveCarrier = INIT_S_CURVE_CARRIER;
    S_CurveBandwith = INIT_S_CURVE_BANDWITH;

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

    EEPROM.get(S_CURVE_CARRIER_ADR, S_CurveCarrier);
    EEPROM.get(S_CURVE_BANDWITH_ADR, S_CurveBandwith);
  }
}