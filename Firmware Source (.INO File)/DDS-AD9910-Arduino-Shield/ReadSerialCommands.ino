
#define SERIAL_PACKAGE_MAX_LENGTH 20
char SerialBuffer[SERIAL_PACKAGE_MAX_LENGTH];

const char HELP_STRING [] PROGMEM = 
          "F - Set Frequency in Hz (100000 — 600000000)\n"
          "P - Set Output Power in dBm (-72 — 0 OR -68 — +4, depending on \"DAC current\")\n"
          "M - Get Model\n"
          "V - Get Firmware Version\n"
          "E - Enable Output\n"
          "D - Disable Output\n"
          "h - This Help\n"
          "; - Commands Separator"
          "\n"
          "Example:\n"
          "F100000;P-2\n"
          "Set Frequency to 100 kHz, and Output Power to -2 dBm.\n"
          "Any number of commands in any order is allowed.";

void ReadSerialCommands()
{
  if (!Serial.available()) return;
  int RcvCounter=0;
  RcvCounter = Serial.readBytesUntil('\n', SerialBuffer, 110);
  if (RcvCounter == 0) return;
  SerialBuffer[RcvCounter]='\0';

  int32_t value=0;
  char command;

  GParser data(SerialBuffer, ';');
  int commandsCounter = data.split();

    for (int i=0; i < commandsCounter; i++)
    {
      sscanf(data[i], "%c%ld", &command, &value);
      switch (command)
      {
        case 'F': //RF Frequency
          if (inRange(value, LOW_FREQ_LIMIT, HIGH_FREQ_LIMIT))
          {
            Serial.print(F("Set freq.: "));
            Serial.println(value);
            H = value % 1000;
            value = value / 1000;
            K = value % 1000;
            value = value / 1000;
            M = value % 1000;
          } else Serial.println("Frequency is OUT OF RANGE (" + String(LOW_FREQ_LIMIT) + " — " + String(HIGH_FREQ_LIMIT) + ")");
        break;

        case 'P': //Power, dBm
          int lowPowerLimit, highPowerLimit;
          if (DACCurrentIndex == 0) 
          {
            lowPowerLimit = -72;
            highPowerLimit = 0;
          } else
          {
            lowPowerLimit = -68;
            highPowerLimit = 4;
          }
          if (inRange(value, lowPowerLimit, highPowerLimit))
          {
            Serial.print(F("Set Power.: "));
            Serial.println(value);
            if (DACCurrentIndex == 0) A = -1 * value; else A = -1 * (value - 4);
          } else Serial.println("Power is OUT OF RANGE (" + String(lowPowerLimit) + " — " + String(highPowerLimit) + ")");
        break;

        case 'D': 
          Serial.println(F("Output Disabled"));
          digitalWrite(DDS_PWR_DWN_PIN, HIGH);
          isPWR_DWN = true;
        break;

        case 'E': 
          Serial.println(F("Output Enabled"));
          digitalWrite(DDS_PWR_DWN_PIN, LOW);
          isPWR_DWN = false;
        break;
        
        case 'V': //Firmware Version request
          Serial.println(FIRMWAREVERSION);
          //Serial.println(value);
        break;

        case 'M': //Model request
          Serial.println(F("DDS9910 v2.0"));
          //Serial.println(value);
        break;

        case 'h': //Model request
          Serial.println((const __FlashStringHelper *) HELP_STRING);
        break;

        default:
        Serial.print(F("Unknown command:"));
        Serial.println(command);
        Serial.println((const __FlashStringHelper *) HELP_STRING);
      } //switch
    } //for

    MakeOut();
    UpdateDisplay();
}

bool inRange(int32_t val, int32_t minimum, int32_t maximum)
{
  return ((minimum <= val) && (val <= maximum));
}