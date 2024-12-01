
void LocalOscillator_Menu()
{
  EditMode = false;
  LOMenuPos = LO_MENU_MHZ_INDEX;
  ModIndex = LO_MOD_TYPE;
  setRealCarrier();
  MakeOut();
  while(1)
  {
    curPos=0;
    curPos=myEnc.read();

    modeButton.Update(); // read state button encoder

    if (modeButton.clicks > 0) // short click button encoder
    {
      if (LOMenuPos == LO_MENU_EXIT_INDEX) 
      {
        ModIndex = NONE_MOD_TYPE;
        MakeOut();
        return;
      }
      if (LOMenuPos == LO_MENU_IF_SIGN_INDEX) 
      {
        loSettings.IFSign = loSettings.IFSign * -1;
        setRealCarrier();
        SaveLOSettinngs();
        MakeOut();
      }
        else EditMode = !EditMode;
    }

    if (EditMode == false) //перемещение по менню
    { 
      if (curPos > 0) 
      {
        LOMenuPos++;
        if (LOMenuPos > LO_MENU_LAST_INDEX) LOMenuPos = LO_MENU_MHZ_INDEX;
      }
      if (curPos < 0) 
      { 
        LOMenuPos--;
        if (LOMenuPos < LO_MENU_MHZ_INDEX) LOMenuPos = LO_MENU_LAST_INDEX;
      }
    } else //изменение параметров
      {
        switch (LOMenuPos)
        {
          case LO_MENU_MHZ_INDEX:
            //if (inRange(LOCarrierFreq + curPos * uint32_t(1e6), LOW_FREQ_LIMIT, HIGH_FREQ_LIMIT)) LOCarrierFreq += curPos * uint32_t(1e6);
            loSettings.LOCarrierFreq = isLOCarrierInRange(loSettings.LOCarrierFreq + (int32_t(curPos) * int32_t(1e6)));
          break;
          case LO_MENU_KHZ_INDEX:
            //if (inRange(LOCarrierFreq + curPos * LOStep, LOW_FREQ_LIMIT, HIGH_FREQ_LIMIT)) LOCarrierFreq += curPos * LOStep;
            //LOCarrierFreq = isLOCarrierInRange(LOCarrierFreq + curPos * LOStep);
            loSettings.LOCarrierFreq = isLOCarrierInRange(loSettings.LOCarrierFreq + curPos * LOStepsList[loSettings.LOStepsIndex]);
          break;
          case LO_MENU_HZ_INDEX:
            //if (inRange(LOCarrierFreq + curPos, LOW_FREQ_LIMIT, HIGH_FREQ_LIMIT)) LOCarrierFreq += curPos;
            loSettings.LOCarrierFreq = isLOCarrierInRange(loSettings.LOCarrierFreq + curPos);
          break;
          case LO_MENU_STEP_INDEX:
            if (curPos > 0) loSettings.LOStepsIndex += 1;
            if (curPos < 0) loSettings.LOStepsIndex -= 1;
            if (loSettings.LOStepsIndex > LOStepsQty - 1) loSettings.LOStepsIndex = LOStepsQty - 1;
            if (loSettings.LOStepsIndex < 0) loSettings.LOStepsIndex = 0;
          break;
          case LO_MENU_IF_INDEX:
            if (curPos > 0) loSettings.IFFreqListIndex += 1;
            if (curPos < 0) loSettings.IFFreqListIndex -= 1;
            if (loSettings.IFFreqListIndex > IFFreqQty - 1) loSettings.IFFreqListIndex = IFFreqQty - 1;
            if (loSettings.IFFreqListIndex < 0) loSettings.IFFreqListIndex = 0;
            setRealCarrier();
          break;
        }
        if (curPos != 0) 
        {
          // Serial.println(LOCarrierFreq);
          // Serial.println(curPos);
          // Serial.println(curPos * 1e6);
          //RealCarrierFreq = LOCarrierFreq + IFSign * IFFreqList[IFFreqListIndex];
          //ModIndex = LO_MOD_TYPE;
          setRealCarrier();
          needToSaveLOSettings = true;
          lastTimeLOSettingsChanged = millis();
          MakeOut();
        }
      }
    //RealCarrierFreq = LOCarrierFreq + IFSign * IFFreqList[IFFreqListIndex];
    setRealCarrier();
    DrawLocalOscillatorMenu();
    if ((millis() - lastTimeLOSettingsChanged > 3000)
        && (needToSaveLOSettings == true)) 
        {
          needToSaveLOSettings = false;
          SaveLOSettinngs();
          Serial.println(F("LO Settings Saved"));
        }
  }
}

int32_t isLOCarrierInRange(int32_t newLOCarrierValue)
{
  int32_t DynamicLowLOCarrierLimit = LOW_FREQ_LIMIT - loSettings.IFSign * IFFreqList[loSettings.IFFreqListIndex];
  //Serial.println(DynamicLowLOCarrierLimit);
  if (DynamicLowLOCarrierLimit < 0) DynamicLowLOCarrierLimit = 0;
  int32_t DynamicHighLOCarrierLimit = HIGH_FREQ_LIMIT - loSettings.IFSign * IFFreqList[loSettings.IFFreqListIndex];
  if (newLOCarrierValue < DynamicLowLOCarrierLimit) return DynamicLowLOCarrierLimit;
    else if (newLOCarrierValue > DynamicHighLOCarrierLimit) return DynamicHighLOCarrierLimit;
      else return newLOCarrierValue;
}

void setRealCarrier()
{
  loSettings.RealCarrierFreq = loSettings.LOCarrierFreq + loSettings.IFSign * IFFreqList[loSettings.IFFreqListIndex];
  if (loSettings.RealCarrierFreq < LOW_FREQ_LIMIT) 
  {
    loSettings.RealCarrierFreq = LOW_FREQ_LIMIT;
    loSettings.LOCarrierFreq = LOW_FREQ_LIMIT - loSettings.IFSign * IFFreqList[loSettings.IFFreqListIndex];
  }
  if (loSettings.RealCarrierFreq > HIGH_FREQ_LIMIT) 
  {
    loSettings.RealCarrierFreq = HIGH_FREQ_LIMIT;
    loSettings.LOCarrierFreq = HIGH_FREQ_LIMIT - loSettings.IFSign * IFFreqList[loSettings.IFFreqListIndex];
  }
}

void SaveLOSettinngs()
{
  EEPROM.put(LO_SETTINGS_ADR, loSettings);
  EEPROM.put(LO_SETTINGS_FLAG_ADR, 55);
}

void LoadLOSettings()
{
  if (EEPROM.read(LO_SETTINGS_FLAG_ADR) == 55) EEPROM.get(LO_SETTINGS_ADR, loSettings);
    else SaveLOSettinngs();
}
