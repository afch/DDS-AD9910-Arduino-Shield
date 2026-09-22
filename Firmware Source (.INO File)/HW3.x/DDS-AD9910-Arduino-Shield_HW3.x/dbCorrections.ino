#include <avr/pgmspace.h>

struct Point {
  uint16_t freq; //MHz
  float dB;
};

const Point ZXY_YB2F_458PT_21TL_AND_18nH_Filter[] PROGMEM = {
  {0, -1.0},
  {1, 0.5},
  {220, -0.75},
  {250, -0.6},
  {290, -1.7},
  {330, -1.1},
  {350, -1.5},
  {390, -0.65},
  {470, -2.9},
  {500, -3.0},
  {600, -5.5}
};

float GetOutputdBCorrection(uint16_t freq)
{
  Point p0, p1;
  for (size_t i = 0; i < sizeof(ZXY_YB2F_458PT_21TL_AND_18nH_Filter) / sizeof(ZXY_YB2F_458PT_21TL_AND_18nH_Filter[0]) - 1; i++) 
  {
    p0.freq = (uint16_t)pgm_read_word(&ZXY_YB2F_458PT_21TL_AND_18nH_Filter[i].freq);
    p1.freq = (uint16_t)pgm_read_word(&ZXY_YB2F_458PT_21TL_AND_18nH_Filter[i + 1].freq);

    if (freq >= p0.freq && freq <= p1.freq) 
    {
      p0.dB = (float)pgm_read_float(&ZXY_YB2F_458PT_21TL_AND_18nH_Filter[i].dB);
      p1.dB = (float)pgm_read_float(&ZXY_YB2F_458PT_21TL_AND_18nH_Filter[i + 1].dB);
      return p0.dB + (freq - p0.freq) * (p1.dB - p0.dB) / (p1.freq - p0.freq);
    }
  }
  return 0;
}