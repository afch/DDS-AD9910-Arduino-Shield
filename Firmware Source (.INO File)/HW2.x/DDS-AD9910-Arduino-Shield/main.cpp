/*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *******************************************                                      
 */

#include <Arduino.h>
#include "main.h"

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
ClickButton modeButton(MODE_PIN, LOW, CLICKBTN_PULLUP);
ClickButton upButton(UP_PIN, LOW, CLICKBTN_PULLUP);
ClickButton downButton(DOWN_PIN, LOW, CLICKBTN_PULLUP);

void DisplaySaved(void)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40, 0);
  //display.setCursor(0, 0);
  display.cp437(true);
  display.println("SETUP");
  display.setCursor(40, 32);
  display.print("SAVED");
  display.display();
}
