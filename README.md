# DDS-AD9910-Arduino-Shield (Software Version 2.x)
DDS (Direct Digital Synthesis) Analog Devices AD9910 Arduino Shield by GRA &amp; AFCH

Web-site: https://gra-afch.com
Direct link to category:  https://gra-afch.com/product-category/rf-units/

# GA_Flasher can be used to upload compiled firmware from "Firmware Compiled (.HEX File)" folder: https://github.com/afch/GA_Flasher

"Firmware Compiled (.HEX File)" - folder contains pre-Compiled firmware (.HEX File), that can be uploaded via "AvrDude", GA_Flasher or any other software for flashing Atmega 2560

"Firmware Source (.INO File)" - folder contains source of firmware that can be compiled (modified if necessary) and uploaded via Arduino IDE: https://www.arduino.cc/en/Main/Software

"Libraries" - contains libraries that are only needed for compilation: place them to "C:\Users\[USER]\Documents\Arduino\libraries", where [USER] - your windows username.

This AD9910 Shield can be easily connected to Arduino Mega without additional wires and converters. All functions of the DDS AD9910 are brought to the contacts of the Arduino Mega thanks to this you can fully reveal all the capabilities of the DDS AD9910.

The program implements (involved) AD9910 technologies such as:
Sweep, DRG (Digital Ramp Generator), RAM, AM, FM modulation

Key Benefits:

*Low harmonics no more than -60dB. An output RF transformer is used for the correct operation of the current mirror.

*Small spur

*4 layer board. Signal lines TOP and Bottom, inner layers Ground and Power.

*Low Noise LDO Stabilizers

*Separate power supply for all analog and digital lines (1.8 and 3.3V), 5pcs IC voltage stabilizers are used. Additionally, there is an RF Ferrite bead interchange.

*High-speed decoupling Level converter and TTL 5V matching

*one of 4 types of clock source can be used/installed:

1. XO - Crystal 25Mhz 20ppm internal oscillator with PLL at 1 GHz,
2. TCXO - 10Mhz 1ppm external oscillator PLL at 1 GHz,
3. EGEN - external generator up to 1.5 GHz
4. OCXO - Oven Controlled Crystal Oscillators deliver the ultimate piezo electric performance with stabilities down to less than ± 1ppb.
(*) additionally used balancing transformer for TCXO, EGEN and OCXO options

* Easy to connect OLED display.
* Control buttons for control via the program menu.
* The synthesizer is capable to generate sine wave, AM or FM modulated signal.
* The software allows you to select and configure the frequency of the clock generator through the user menu (without the need to recompile the program).
* Any settings can be stored in non-volatile EEPROM memory (located at Arduino Mega).
* Basic settings are applied and saved automatically.
* This shield support overclocking the AD9910 core to 1.5 GHz (heatsink is recommended).
* DDS AD9910 Shield has ability to generate a signal up to 600 MHz with a core overclocking up to 1.5 GHz (to suppress harmonics, it is recommended to  overclock the AD9910 for frequencies above 400 MHz).
* Has ability to increase output power by +4 dBm when “DAC Current HI” is activated.

# The switching of clock sources is performed by the following components:
<pre>
|-------------------------------|-----------------------------------|-----------------|
|      Clock source             |            Capacitors             |    Resistors    |
|   (only one at a time)        | C20  |  C22  |  C18,C19 | C14,C17 |  XTAL | REF_CLK |
|-------------------------------------------------------------------------------------|
| XO - Crystal Oscillator (Z1)  |  -   |   -   |    V     |    X    |   V   |    X    |
| TCXO - Oscillator 1ppm (Z2)   |  V   |   X   |    X     |    V    |   X   |    V    |
| OCXO - Oscillator 0.1ppm (Z3) |  X   |   V   |    X     |    V    |   X   |    V    |
| EGEN - External Generator     |  X   |   X   |    X     |    V    |   X   |    V    |
|-------------------------------------------------------------------------------------|
</pre>

Where V means that the component must be installed (soldered), and X - means that the component must be removed

# List of Serial Port Commands:
Starting with version 2.14, the ability to control via the serial port has been added.

  F - Set Frequency in Hz (100000 — 600000000)  
  P - Set Output Power in dBm (-72 — 0 or -68 — +4, depending on "DAC current")  
  E - Enable Output  
  D - Disable Output  
  M - Get Model  
  V - Get Firmware Version  
  h - Help  
  ; - Commands Separator  
          
Example:  
  F100000;P-2  
Set Frequency to 100 kHz, and Output Power to -2 dBm.  
Any number of commands in any order is allowed.  

# Serial Port Settings:

  Speed - 115200 Bouds  
  Data Bits - 8  
  Stop Bits - 1  
  Parity - No  
  DTR - OFF  
# Windows:

An example of setting up a serial port in the Windows console:
  <pre>
  mode COM3 baud=115200 DTR=OFF Data=8
  </pre>
  
Usage example:
  <pre>
  echo F100000000 > COM3
  </pre>
# Ubuntu 22.04:

An example of setting up a serial port in the Ubuntu:
  <pre>
  sudo usermod -aG dialout $USER_NAME$
  sudo chmod a+rw /dev/ttyUSB0
  sudo stty -F /dev/ttyUSB0 115200 cs8 ixoff -hupcl -echo
  </pre>
  
Usage example:
  <pre>
  echo "F100000000" > /dev/ttyUSB0
  </pre>

