// ----------------------------------------------------------------------------
// miniBCS
// 
// functions needed for the miniBCS
//  set up solenoid pins, pushbutton pins 
//
// Author: Steve Sawtelle
// HHMI Janelia ID&F
// ----------------------------------------------------------------------------

// ---- VERSIONS  ----
#define  miniBCS_VERSION ((uint32_t) 20231129) 

// 0231129 sws
// - remove depracted Button code
// - changes for 'M' board with Teensy 4.1

// 20230518 sws
// - SOL7PIN was set to pin 4 and SOL8 to pin 3, but hardware is opposite, fix in FW

// 20221205 sws
// - solenoid pins 5-8 on Teesny 3.5 cannot be used as digital out
//   so set them the same as the T4.1. This will interfere with CAN 

// 20220822 sws
//  - add GPO pins 16 to 19 NOTE: Should also extend GPI and analog as well

// 20220613 sws 
// - alt pins for Teensy 4.1 - here and .cpp

// 20190322 sws
// - add CS setups for TFT and touchscreen so BCS can init properly

// 20190315 sws
// - mods for REV G boards
//  	- LCDCS now SPAREpin
//		- pin 15 now TFT CS
//		- SCL now TFT reset
//		- SDA now touchscreen CS
// 		- SW1 is TS int
// 		- 21 is TFT data/command

// 20170906 sws
// - add MAX code
// - workaround for problem with SPI settings
// 20170824 sws
// - change PB to use HW debounce
// - add encoder

// 20170817
// - started


#ifndef miniBCS_H
#define miniBCS_H

#include <SPI.h>

//volatile uint8_t _pushButtons = 0;

class miniBCS
{
 public:
 
//=======================================
// === P I N    D E F I N I T I O N S ===
// ======================================

#define RX1pin    0   // Serial 1 RX 
#define TX1pin    1   // Serial 1 TX
#define ENCApin   2   // encoder 'A' pin
#define SOL7pin   3 // Solenoid 7 pin
#define SOL8pin   4 // Solenoid 8 pin
#define ENCBpin   5   // encoder 'B' pin
#define MAXCSpin  6   // Chip Select for MAX112300
#ifdef ARDUINO_TEENSY41
#define RX2pin    7   // 
#else
#define RX3pin    7   // Serial 3 RX or 
#endif
#define PIN7      7   //   IO pin, with PWM
#ifdef ARDUINO_TEENSY41
#define TX2pin    8   // 
#else
#define TX3pin    8   // Serial 3 TX or
#endif
#define PIN8      8   //   IO pin with PWM
#define PIN9      9   //   IO pin with PWM
#ifndef ARDUINO_TEENSY41
#define RX2pin    9   // Serial 2 RX or
#define TX2pin    10  // Serial 3 TX or
#endif
#define PIN10     10  //   IO pin with PWM
#define ETHCSpin  10  // or alternate ethernet CS
#define MOSIpin   11  // SPI MOSI
#define MISOpin   12  // SPI MISO
#define SCLKpin   13  // SPI Clock
#ifdef ARDUINO_TEENSY41
#define TX3pin    14   // 
#endif
#define A0pin     14  // Analog in A0
#ifdef ARDUINO_TEENSY41
#define RX3pin    15
#else
#define TFTCSpin  15  // TFT chip select 
#endif
#define SOL4pin   16  // Solenoid 4 pin
#define SOL3pin   17  // Solenoid 3 pin
#define SOL5pin   18  // Solenoid 5 pin
#define DDSCSpin  19  // Chip Select for DDS
#define A6pin     20  // Analog in A6 
#define PIN20     20  //	or IO 20
#define TFTDCpin  21  // TFT Display command/data	or IO 21
#define SOL2pin   22  // Solenoid 2 pin
#define SOL1pin   23  // Solenoid 1 pin
#define ETH_RSTpin  24  // reserved for Ethernet
#define ETH_CS    25  // reserved for Ethernet
#define ETH26pin  26  // reserved for Ethernet
#define EXP8pin   27  // to Expansion Connection 8
#define EXP7pin   28  // to Expansion Connection 7
#define MAXINTpin 29  // interrupt in from MAX11300
#define MAXCVTpin 30  // ADC Convert signal to MAX11300
#define POTCSpin  31  // Chip Select for digital pot
#define TSINTpin  32  // touch screen interrupt	
#define PIN33     33  // I/O pin 33 w PWM
#define PIN34     34  // I/O pin 34 
#define IDXpin    35  // Encoder Index in
#ifdef ARDUINO_TEENSY41
#define TFTCSpin  36   // 
#else
#define LED1pin   36  // LED pin, Auxiliary I/O, analog, PWM for auxiliary connector - depracted 'M' rev and later 
#endif
#define TFTRSTpin 37  // TFT RESET  analog, PWM , SCL
#define TSCSpin   38  // Touchscreen CS, SDA
#define SOL6pin   39 // Solenoid 6 pin
#ifndef ARDUINO_TEENSY41
#define DAC0pin   A21 // DAC0 out pin (must be jumpered)
#define DAC1pin   A22 // DAC1 out pin (must be jumpered)
#endif


// DDS output waveform types
#define TONE_SINE     0x0000
#define TONE_TRIANGLE 0x0002  
#define TONE_SQUARE   0x0028   
#define TONE_DIVIDE   0x0020  


// MAX11300 defines
#define DAC_DATA_PORT_00 0x60
#define GPO_DATA_15_TO_0 0x0d
#define GPO_DATA_19_TO_16 0x0e
#define ADC_DATA_PORT_00 0x40
#define GPI_DATA_15_TO_0 0x0b
#define INT_TEMP		 0x08
#define EXT_TEMP1		 0x09
#define EXT_TEMP2		 0x0a
#define MAX_INT				0x01
#define MAX_ADCFLAG_MASK	0x0001
#define MAX_ADCST_L			0x02	
#define MAX_ADCST_H			0x03
#define MAX_ADCDM_MASK		0x0004
#define MAX_ADCDR_MASK		0x0002	
#define MAX_ADCFLAG_MASK	0x0001


  miniBCS(void);
  
  void begin();
  void driverOn(uint8_t ch);					// turn on driver channel 'ch'
  void driverOff(uint8_t ch);					// turn off driver channel 'ch'
  int8_t getDriver(uint8_t ch);					// get status of channel 'ch' (false = off)
  uint32_t getVersion(void);					// get library version (year,month,date )
  void setToneGain(uint8_t level);				// set tone output level (0-255)
  void toneReset(void);							// reset tone generator chip
  int8_t setTone( float freq, uint16_t mode);	// set tone frequency and waveform type	
  void toneOn(void);							// enable tone (that has already been set up)
  void toneOff(void);							// disable tone without changing parameters	
  int32_t getPosition(void);					// get encoder position (cumulative counts)
  void  setPosition(int32_t newpos);			// set encoder position to a value
  uint16_t getToneMode(void); 					// read waveform type set on tone generator
 
   
 private:
 
  const static uint16_t DDS_CONTROL =  0x2000;   // 0010 0000 0000 0000
  const static uint32_t DDS_MCLK = 25000000;  
  const static uint32_t DDS_2_28 = 268435456; 
 
  // Private Constants
  volatile uint32_t encoderCount;
  volatile float distancePerTick;
  volatile float speedPerTickPerSec;
  volatile uint32_t zeroMillis;
  
  uint16_t freqMode; 
  uint32_t ifreq;
  uint16_t fwordl;
  uint16_t  fwordh;
  uint8_t SOUND_ON_OFF = 1; //each channel is off to begin with, correlating to a value of 1 for D6
  const uint8_t driverPin[9] = {0, SOL1pin, SOL2pin, SOL3pin, SOL4pin, SOL5pin, SOL6pin, SOL7pin, SOL8pin};
  int8_t driverOnOff = 0x00;

};  // endclass miniBCS


#endif // miniBCS