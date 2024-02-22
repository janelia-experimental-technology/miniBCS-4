// ----------------------------------------------------------------------------
// miniBCS
// 
// functions needed for the miniBCS
//
// Author: Steve Sawtelle
// HHMI Janelia ID&F
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "miniBCS-4.h"


//---------- constructor ----------------------------------------------------

// volatile uint8_t _pushButtons = 0;
volatile int32_t _position = 0;

volatile uint32_t	_analogStatus = 0;
volatile bool		_analogFlag;

miniBCS::miniBCS(void)
{
;
}
//------------------ private -----------------------------------------------


 SPISettings POT_SPI(2000000, MSBFIRST, SPI_MODE0);   // clk idle low, rising egde
 SPISettings DDS_SPI(5000000, MSBFIRST, SPI_MODE2);   // clk idle high, falling edge
// SPISettings MAX_SPI(5000000, MSBFIRST, SPI_MODE0);   // 


void EncoderA_interrupt_handler()
{
  // figure out the direction  
  if (digitalRead(ENCApin) == digitalRead(ENCBpin))
    _position--;
  else
    _position++;
}


// ---------------- public -----------------------------------------------------------

 void miniBCS::begin(void)
{
	// init all SPI chip selects 
	
    pinMode(MAXCSpin, OUTPUT);
    digitalWrite(MAXCSpin, HIGH);
	   

	pinMode( DDSCSpin, OUTPUT);
	digitalWrite( DDSCSpin, HIGH);	
	

	pinMode( POTCSpin, OUTPUT);	
	digitalWrite( POTCSpin, HIGH);	
	
	
	SPI.begin();


	pinMode(SOL1pin, OUTPUT);
	pinMode(SOL2pin, OUTPUT);	
	pinMode(SOL3pin, OUTPUT);
	pinMode(SOL4pin, OUTPUT);
	pinMode(SOL5pin, OUTPUT);
	pinMode(SOL6pin, OUTPUT);	
	pinMode(SOL7pin, OUTPUT);
	pinMode(SOL8pin, OUTPUT);	

	digitalWrite(SOL1pin, LOW);
	digitalWrite(SOL2pin, LOW);
	digitalWrite(SOL3pin, LOW);
	digitalWrite(SOL4pin, LOW);
	digitalWrite(SOL5pin, LOW);
	digitalWrite(SOL6pin, LOW);
	digitalWrite(SOL7pin, LOW);
	digitalWrite(SOL8pin, LOW);	
		
	pinMode(ENCApin, INPUT_PULLUP);  // encoder inputs
    pinMode(ENCBpin, INPUT_PULLUP);
    pinMode(IDXpin, INPUT_PULLUP);

	
	attachInterrupt( ENCApin, EncoderA_interrupt_handler, RISING);

	SPI.beginTransaction(DDS_SPI);
	digitalWrite( DDSCSpin, LOW);		
	SPI.transfer16(0x2100);     // write control reg
	digitalWrite( DDSCSpin, HIGH);	
	SPI.endTransaction();
  
} 


uint32_t miniBCS::getVersion(void)
{
    return(miniBCS_VERSION);	
}

void miniBCS::driverOn(uint8_t ch)
{
	 
	if( (ch >= 1) && (ch <= 8) )
    {		
		digitalWrite( driverPin[ch], HIGH);
		driverOnOff |= (1 << (ch-1));
	}	
	else if( ch == 0 )
	{	
		for( ch = 1; ch <= 8; ch++)	
			digitalWrite( driverPin[ch], HIGH);
		driverOnOff = 0xff;
    }
}

void miniBCS::driverOff(uint8_t ch)
{
	if( (ch >= 1) && (ch <= 8) )
	{
		digitalWrite( driverPin[ch], LOW);
		driverOnOff &= ~(1 << (ch-1));
    }		
	else if( ch == 0 )
	{	
		for( ch = 1; ch <= 8; ch++)
			digitalWrite( driverPin[ch], LOW);
		driverOnOff = 0x00;
	}	
}

int8_t miniBCS::getDriver(uint8_t ch)
{
	if((ch >= 1) && (ch <= 8) )
		return driverOnOff & (1 << (ch-1));
	else if( ch == 0 )
		return driverOnOff;
	else	
		return -1;
		
}

void miniBCS::toneReset(void)
{  
	SPI.beginTransaction(DDS_SPI);
	SPI.transfer(0xff);    // dead transfer to get clock rest level set right	
	digitalWrite( DDSCSpin, LOW);	
	SPI.transfer16(0x0100);     // write control reg HOB
//	SPI.transfer(freqMode & 0x00ff);     // write control reg LOB
	digitalWrite( DDSCSpin, HIGH);	
	SPI.endTransaction();
	SOUND_ON_OFF = 1; //D6 = 1 to be off
}

void miniBCS::setToneGain(uint8_t level)
{  
	SPI.beginTransaction(POT_SPI);
	digitalWrite(POTCSpin, LOW);   // select pot    			
	SPI.transfer(0xc0 | 0);        // wiper write - ch 0
	SPI.transfer(level);
	digitalWrite(POTCSpin, HIGH);
	SPI.endTransaction();		
}
/*
void miniBCS::setDACGain(uint8_t level)
{  		   			
	SPI.beginTransaction(POT_SPI);
	digitalWrite(POTCSpin, LOW);   // select pot 
	SPI.transfer(0xc0 | 1);        // wiper write - ch 0
	SPI.transfer(level);
	digitalWrite(POTCSpin, HIGH);
	SPI.endTransaction();	
}
*/
void miniBCS::toneOff(void)
{
	SPI.beginTransaction(DDS_SPI);
	SPI.transfer(0xff);    // dead transfer to get clock rest level set right
	digitalWrite( DDSCSpin, LOW);	
	SPI.transfer16(0x20c0);           // use reset to turn off
	digitalWrite( DDSCSpin, HIGH);	
	SPI.endTransaction();
	SOUND_ON_OFF = 1; //D6 = 1 to be off
}

void miniBCS::toneOn(void)
{
	freqMode &= 0xfe3f;   // turn off sleep bits and reset
	SPI.beginTransaction(DDS_SPI);
	SPI.transfer(0xff);    // dead transfer to get clock rest level set right	
	digitalWrite( DDSCSpin, LOW);	
	SPI.transfer16(freqMode);     // write control reg HOB
//	SPI.transfer(freqMode & 0x00ff);     // write control reg LOB
	digitalWrite( DDSCSpin, HIGH);	
	SPI.endTransaction();
	SOUND_ON_OFF = 0; //D6 = 0 to be on
}

int8_t miniBCS::setTone( float freq, uint16_t mode) 
{
 uint16_t freqword;
 
    freqMode = mode;
	freqMode |= DDS_CONTROL; // form complete control register value 
	if ( SOUND_ON_OFF == 1) //sound is off, keep off
		freqMode |= 0x00c0;  // turn on sleep
	else
		freqMode &= 0xff3f;   // turn off sleep bits			
	ifreq = uint32_t (10.737418 * freq );   // 10.737418  = 2^28/25e  
	SPI.beginTransaction(DDS_SPI);
	SPI.transfer(0xff);    // dead transfer to get clock rest level set right	
	digitalWrite( DDSCSpin, LOW);	
	SPI.transfer16(freqMode); // >> 8);     // write control reg HOB
	//SPI.transfer(freqMode & 0x00ff);     // write control reg LOB
	freqword = 0x4000 | (ifreq & 0x3fff); // 14 LSBs 
	fwordl = freqword;
	SPI.transfer16( freqword); // >> 8 );
	//SPI.transfer( freqword & 0x00ff );
	freqword = 0x4000 | (ifreq >> 14) ;   // 14 MSBs	
	fwordh  = freqword;
	SPI.transfer16( freqword); // >> 8 );
	//SPI.transfer( freqword & 0x00ff );		
	digitalWrite( DDSCSpin, HIGH);	
	SPI.endTransaction();
    return 0;
	
}

uint16_t miniBCS::getToneMode(void)
{
	return freqMode;
}

int32_t miniBCS::getPosition(void)
{
	return _position;
}


void miniBCS::setPosition(int32_t newpos)
{
	_position = newpos;
}

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
