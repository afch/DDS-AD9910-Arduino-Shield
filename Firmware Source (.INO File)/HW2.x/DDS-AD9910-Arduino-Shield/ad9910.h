/*                                        *******************************************
 *                                        *             GRA-AFCH.COM                *
 *                                        *******************************************                                      
 */

/******************************************************************************
 * AD9910 - DDS Power Supply 3.3V � 1.8V
 * Internal PLL 1GHz, Ref Crystal 10Mhz
 * Update Register - Hardware or Software
 * 26.12.2015
 * Author Grisha Anofriev e-mail: grisha.anofriev@gmail.com
******************************************************************************/
#ifndef __AD9910_H
#define __AD9910_H

#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <SPI.h>

#define GPIO_PIN_SET HIGH
#define GPIO_PIN_RESET LOW
#define GPIOA 0
#define GPIOB 0
#define GPIOC 0
#define GPIOD 0


//#define F_SYSCLK 1000000000.0 // defualt 1Ghz setting in PLL (used for XO, TCXO, VC-TCXO only)


/* Select SPI for DDS AD9910 */

/* Serial Programming Clock. Data clock for serial programming */
#define DDS_SPI_SCLK_PIN          13
#define DDS_SPI_SCLK_GPIO_PORT    GPIOA
//#define DDS_SPI_SCLK_GPIO_CLK     RCC_AHBPeriph_GPIOA
//#define DDS_SPI_SCLK_SOURCE       GPIO_PinSource5
//#define DDS_SPI_SCLK_AF           GPIO_AF_5

/* Serial Data Input/Output. When the device is in 3-wire mode, data is written 
via this pin. In 2-wire mode, data reads and writes both occur on this pin. 
There is no internal pull-up/pull-down resistor on this pin.*/
#define DDS_SPI_SDIO_PIN          11                                           
#define DDS_SPI_SDIO_GPIO_PORT    GPIOA
//#define DDS_SPI_SDIO_GPIO_CLK     RCC_AHBPeriph_GPIOA
//#define DDS_SPI_SDIO_SOURCE       GPIO_PinSource6
//#define DDS_SPI_SDIO_AF           GPIO_AF_5

/* Serial Data Output. When the device is in 3-wire mode, 
data is read on this pin. */ 
#define DDS_SPI_SDO_PIN          12 // Serial Data Output.                                          
#define DDS_SPI_SDO_GPIO_PORT    GPIOA
//#define DDS_SPI_SDO_GPIO_CLK     RCC_AHBPeriph_GPIOA
//#define DDS_SPI_SDO_SOURCE       GPIO_PinSource7
//#define DDS_SPI_SDO_AF           GPIO_AF_5

/* Chip Select. Digital input (active low). This pin allows the AD9910 to operate on a common
serial bus for the control data path. Bringing this pin low enables the AD9910 to detect
serial clock rising/falling edges. Bringing this pin high causes the AD9910 to ignore input
on the serial data pins.*/
#define DDS_SPI_CS_PIN          10 // chip Select. Active low.
#define DDS_SPI_CS_GPIO_PORT    GPIOA 
//#define DDS_SPI_CS_GPIO_CLK     RCC_AHBPeriph_GPIOA
//#define DDS_SPI_CSB_SOURCE       GPIO_PinSource4
//#define DDS_SPI_CSB_AF           GPIO_AF_5

/*I/O Update. A logic transition from 0 to 1 on this pin transfers data from 
the I/O port registers to the control registers (see the Write section).*/
#define DDS_IO_UPDATE_PIN       6
#define DDS_IO_UPDATE_GPIO_PORT GPIOA 
//#define DDS_IO_UPDATE_GPIO_CLK  RCC_AHBPeriph_GPIOA

/*On China Boards CS and IO_RESET Connected together!!!!
Input/Output Reset. Digital input (active high). This pin can be used when a serial I/O
communication cycle fails (see the I/O_RESET�Input/Output Reset section for details).
When not used, connect this pin to ground.*/
#define DDS_IO_RESET_PIN           9
#define DDS_IO_RESET_GPIO_PORT     GPIOA 
//#define DDS_RESET_GPIO_CLK      RCC_AHBPeriph_GPIOA

/*Master Reset, Digital Input (Active High). Master reset: clears all memory elements and sets
registers to default values.*/
#define DDS_MASTER_RESET_PIN           A3
#define DDS_MASTER_RESET_GPIO_PORT     GPIOC 


/*Profile Select Pins. Digital inputs (active high). Use these pins to select one of eight
phase/frequency profiles for the DDS. Changing the state of one of these pins transfers the
current contents of all I/O buffers to the corresponding registers. State changes should be
set up on the SYNC_CLK pin*/
#define DDS_PROFILE_0_PIN       4
#define DDS_PROFILE_0_GPIO_PORT GPIOC 

#define DDS_PROFILE_1_PIN       3
#define DDS_PROFILE_1_GPIO_PORT GPIOC 

#define DDS_PROFILE_2_PIN       2
#define DDS_PROFILE_2_GPIO_PORT GPIOC 

/*Output Shift Keying. Digital input (active high). When the OSK features are placed in either
manual or automatic mode, this pin controls the OSK function. In manual mode, it toggles
the multiplier between 0 (low) and the programmed amplitude scale factor (high). In
automatic mode, a low sweeps the amplitude down to zero, a high sweeps the amplitude
up to the amplitude scale factor*/
#define DDS_OSK_PIN       			40
#define DDS_OSK_GPIO_PORT 			GPIOC 

/*Parallel Data Clock. This is the digital output (clock). The parallel data clock provides a
timing signal for aligning data at the parallel inputs --INPUT!!!*/
#define DDS_PDCLK_PIN        		44
#define DDS_PDCLK_GPIO_PORT  		GPIOC

/*Transmit Enable. Digital input (active high). In burst mode communications, a high on this
pin indicates new data for transmission. In continuous mode, this pin remains high.*/
#define DDS_TxENABLE_PIN        38
#define DDS_TxENABLE_GPIO_PORT  GPIOC

/*Parallel Input Bus (Active High) */
#define DDS_Parallel_GPIO_PORT  GPIOB

/*Modulation Format Pins. Digital input to determine the modulation format*/
#define DDS_F0_PIN        			39
#define DDS_F0_GPIO_PORT  			GPIOA

/*Modulation Format Pins. Digital input to determine the modulation format*/
#define DDS_F1_PIN        			41
#define DDS_F1_GPIO_PORT  			GPIOA

#define DDS_DRHOLD_PIN          8
#define DDS_PWR_DWN_PIN         5
#define DDS_DRCTL_PIN           7

#define DDS_DROVER              42
#define DDS_SYNC_CLK            46
#define DDS_RAM_SWP_OVR         47
#define DDS_PLL_LOCK            45



//------------------------------------------------------------------

/*DDS AD9910 I/O REGISTER MAP and Mask*/
#define SPI_Read											0x80 // Mask for reading from SPI

#define CFR1_addr  										0x00 // CFR1�Control Function Register 1
#define RAM_enable              			0x80 // enables RAM functionality (required for both load/retrieve and playback operation)
//RAM playback destination
#define RAM_Playback_Frequency 				0x00
#define RAM_Playback_Phase 						0x20
#define RAM_Playback_Amplitude 				0x40
#define RAM_Playback_Polar 						0x60 //(phase and amplitude)
#define Manual_OSK_external_control 	0x80 // OSK pin enabled for manual OSK control 
#define Inverse_sinc_filter_enable  	0x40
// RAM Internal Profile Control Modes - Ineffective unless CFR1[31] = 1. These bits are effective without the need for an I/O update.
#define Internal_Profile_disabled     0x00 // Internal profile control disabled
// Waveform Type - Burst
#define Burst_Profile_0_1     				0x02 // Execute Profile 0, then Profile 1, then halt.
#define Burst_Profile_0_2       			0x04 // Execute Profile 0 to Profile 2, then halt.
#define Burst_Profile_0_3       			0x06 // Execute Profile 0 to Profile 3, then halt.
#define Burst_Profile_0_4       			0x08 // Execute Profile 0 to Profile 4, then halt.
#define Burst_Profile_0_5							0x0A // Execute Profile 0 to Profile 5, then halt.
#define Burst_Profile_0_6							0x0C // Execute Profile 0 to Profile 6, then halt.
#define Burst_Profile_0_7       			0x0E // Execute Profile 0 to Profile 7, then halt.
// Waveform Type - Continuous
#define Continuous_Profile_0_1 				0x10 // Execute Profile 0, then Profile 1, continuously.
#define Continuous_Profile_0_2  			0x12 // Execute Profile 0 to Profile 2, continuously.
#define Continuous_Profile_0_3  			0x14 // Execute Profile 0 to Profile 3, continuously.
#define Continuous_Profile_0_4  			0x16 // Execute Profile 0 to Profile 4, continuously.
#define Continuous_Profile_0_5  			0x18 // Execute Profile 0 to Profile 5, continuously.
#define Continuous_Profile_0_6  			0x1A // Execute Profile 0 to Profile 6, continuously.
#define Continuous_Profile_0_7  			0x1C // Execute Profile 0 to Profile 7, continuously.

#define Load LRR_IO_update 						0x80
#define Autoclear_digital_ramp_accumulator 	0x40
#define Autoclear_phase_accumulator 	0x20
#define Clear_digital_ramp_accumulator      0x10
#define Clear_phase_accumulator 			0x08
#define Load_ARR_IO_update      			0x04
#define OSK_enable										0x02
#define Select_auto_OSK         			0x01

#define Digital_power_down 						0x80
#define DAC_power_down 								0x40
#define REFCLK_input_power_down 			0x20
#define Aux_DAC_power_down 						0x10
#define External_power_down_control 	0x08
#define SDIO_input_only 							0x02
#define LSB_first 										0x01


#define CFR2_addr 											0x01 // CFR2�Control Function Register 1
#define Enable_amplitude_scale_from_single_tone_profiles 0x01
#define Internal_IO_update_active 			0x80
#define SYNC_CLK_enable 								0x40
#define Digital_Ramp_Destination_Frequency 	0x00 
#define Digital_Ramp_Destination_Phase 	0x10
#define Digital_Ramp_Destination_Amplitude  0x20
#define Digital_ramp_enable 						0x08
#define Digital_ramp_no_dwell_high 			0x04
#define Digital_ramp_no_dwell_low 			0x02
#define Read_effective_FTW 							0x01

// Ineffective unless CFR2[23] = 1. Sets the prescale ratio of the divider that clocks the auto I/O
#define IO_update_rate_divide_by_1 			0x00
#define IO_update_rate_divide_by_2 			0x40
#define IO_update_rate_divide_by_4 			0x80
#define IO_update_rate_divide_by_8 			0xC0

#define PDCLK_enable 										0x08
#define PDCLK_invert 										0x04
#define TxEnable_invert 								0x02

#define Matched_latency_enable 					0x80
#define Data_assembler_hold_last_value 	0x40
#define Sync_timing_validation_disable 	0x20
#define Parallel_data_port_enable 			0x10


#define CFR3_addr  											0x02 // CFR3�Control Function Register 1
// The REFCLK_OUT has programmable Buffer Control
#define DRV0_REFCLK_OUT_Disabled              0x00
#define DRV0_REFCLK_OUT_Low_output_current    0x10
#define DRV0_REFCLK_OUT_Medium_output_current 0x20
#define DRV0_REFCLK_OUT_High_output_current   0x30

//VCO Range Bit Settings 
#define VCO0 														0x00 // 400-460Mhz
#define VCO1 														0x01 // 455-530Mhz
#define VCO2 														0x02 // 530-615Mhz
#define VCO3 														0x03 // 650-790Mhz
#define VCO4 														0x04 // 760-875Mhz
#define VCO5 														0x05 // 920-1030Mhz
#define PLL_bypassed 										0x06

// Charge Pump Current, Icp
#define Icp212uA 												0x00
#define Icp237uA 												0x08
#define Icp262uA 												0x10
#define Icp287uA 												0x18
#define Icp312uA 												0x20
#define Icp337uA 												0x28
#define Icp363uA 												0x30
#define Icp387uA 												0x38

#define REFCLK_input_divider_bypass 		0x80
#define REFCLK_input_divider_ResetB 	  0x40
#define PFD_reset 											0x04
#define PLL_enable 											0x01

// REFCLK Multiplier, if PLL enable, SYSCLK= REF_CLK * N
#define N25 														50 // 
#define N40                             80 // 
#define N50 														100 //
#define N100 														200 // 

// [31:8] - Open, [7:0] number controls the full-scale output current of the main DAC
#define FSC_addr 												0x03


// FTW - Frequency Tuning Word [31:0]
#define FTW_addr 												0x07

// POW - Phase Offset Word [15:0]
#define POW_addr 												0x08

// ASF - Amplitude Scale Factor Register [31:0]
#define ASF_addr 												0x09
//Effective only if OSK_enable=1 and Select_auto_OSK=1; see the Output Shift Keying (OSK) section for details	CFR1[9:8] = 11b						 
#define Amplitude_Step_Size_1 				0x00
#define Amplitude_Step_Size_2 				0x01
#define Amplitude_Step_Size_4 				0x02
#define Amplitude_Step_Size_8  				0x03


// Multichip Sync [31:0]
#define Multichip_Sync_addr				    0x0A
#define Sync_receiver_enable 					0x08
#define Sync_generator_enable 				0x04
#define Sync generator polarity 			0x02

// Digital Ramp Limit [63:0]
#define Digital_Ramp_Limit 						0x0B

// Digital Ramp Step Size [63:0]
#define Digital_Ramp_Step_Size 				0x0C

// Digital Ramp Rate [31:0]
#define Digital_Ramp_Rate 						0x0D

// Single Tone Profile X and RAM Profile X [63:0]
#define Single_Tone_Profile_0     		0x0E
#define RAM_Profile_0 								0x0E

#define Single_Tone_Profile_1     		0x0F
#define RAM_Profile_1 								0x0F

#define Single_Tone_Profile_2     		0x10
#define RAM_Profile_2 								0x10

#define Single_Tone_Profile_3     		0x11
#define RAM_Profile_3 								0x11

#define Single_Tone_Profile_4     		0x12
#define RAM_Profile_4 								0x12

#define Single_Tone_Profile_5     		0x13
#define RAM_Profile_5 								0x13

#define Single_Tone_Profile_6     		0x14
#define RAM_Profile_6 								0x14

#define Single_Tone_Profile_7     		0x15
#define RAM_Profile_7 								0x15

// RAM word [31:0]
#define RAM_Start_Word               				0x16



// RAM Profile X mode control[2:0] - ETI DANNIE NE PROVERENNI!!!!!
#define Direct_switch 								0x00
#define Ramp_up                 			0x01
#define Bidirectional_ramp      			0x02
#define Continuous_bidirectional_ramp 0x03
#define Continuous_recirculate  			0x04

#define No_dwell_high 			0x20
#define Zero_crossing 			0x08
//*********

extern void DDS_Fout (uint32_t *F_OUT, int16_t ASF_Val, uint8_t Num_Prof);
extern void SingleProfileFreqOut(uint32_t freq_output, int16_t amplitude_dB_output);
extern void DDS_Init(bool PLL, bool Divider, uint32_t Ref_Clk);
void DDS_GPIO_Init(void);
void DDS_SPI_Init(void);
void DDS_Config(uint16_t Instr_Word, uint8_t Data);
void PrepRegistersToSaveWaveForm (uint64_t Step_Rate, uint16_t Step);
extern void SaveFMWavesToRAM (uint32_t F_carrier, uint32_t F_mod, uint32_t F_dev);
extern void SaveAMWavesToRAM(uint32_t F_carrier, uint32_t F_mod, uint32_t AM_DEPH, int16_t Amplitude_dB);
extern void PlaybackFMFromRAM(int16_t Amplitude_dB);
extern void PlaybackAMFromRAM(uint32_t F_carrier);

extern uint64_t strFTW[];
extern uint16_t strdBm[];
void DDS_Current(uint8_t DAC_Current);
void DDS_UPDATE(void);
extern void DDS_RAM (uint32_t *BUFF_RAM_DDS, uint16_t NumRAM_Words);
extern void Read_DDS (uint8_t ADR_DDS, uint16_t NUM_BYTE);
extern void PlaybackRAM (uint8_t RAM_enable_MODE, uint8_t RAM_Playback_MODE);
extern void DDS_FM (uint32_t F_carrier, uint32_t F_mod, uint32_t F_dev);
void HAL_GPIO_WritePin (int port, int pin, int mode);
void HAL_SPI_Transmit(int *blank, uint8_t *strBuffer, int nums, int pause);
void ASF_SET (uint16_t *Amplitude_ramp_rate1, uint16_t Amplitude_scale_factor1, uint8_t Amplitude_step_size1);
//extern void DDS_Init_OLD(void);
void calcBestStepRate(uint16_t *Step, uint64_t *Step_Rate, uint32_t F_mod);
uint32_t CalcRealDDSCoreClockFromOffset();
//void SaveDTMFToRAM (uint32_t Tone1, uint32_t Tone2, uint32_t F_dev);
//void SingleProfileSSBDTMF(uint32_t freq_output, int16_t amplitude_dB_output);
void DigitalRamp(uint32_t FTWStart, uint32_t FTWEnd, uint32_t FTWStepSize, uint16_t StepRate);
void Sweep(uint32_t StartSweepFreq, uint32_t StopSweepFreq, uint16_t SweepTime, uint8_t SweepTimeFormat);
uint32_t FreqToFTW(uint32_t Freq);
#endif
