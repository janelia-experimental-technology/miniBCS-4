// ==============================================
//  This is an example sketch for the miniBCS with Teensy 4.1 rev M Board
//  TFT Display, changes in Teensy pinouts used for display and I/O
//
//  Simple interface that:
//    - sets up board
//    - displays info on screen
//    -  takes commands to:
//        - pulse a pin 
//        - test solenoid outputs
//        - send out tone
//  
//  2024 HHMI Janelia Steve Sawtelle
//
// ===============================================
// NON STANDARD LIBRARIES REQUIRED:
//     miniBCS-4
//     ILI9341_t3
//     XPT2046_Touchscreen
//     Cmd   

#define DEBUG 

#define VERSION "maxio-2"

// 20191316 sws
// - touch working, tft working
//   - needed to fix order and setup of chip selects

#include <miniBCS-4.h>              // miniBSC functions
#include <XPT2046_Touchscreen.h>      // Touchscreen functions
#include <ILI9341_t3.h>               // TFT Display functions
#include <font_Arial.h>               // from ILI9341_t3     
#include <SPI.h>                      // SPI needed for TFT display
#include <Cmd.h>
#include <MAX11300.h>

//
//#define TS_CS 38                      // touchscreen chip select
//#define TFT_DC  21                    // TFT data/command
//#define TFT_CS 15                     // TFT chip select 
//#define TFT_RST 37                    // TFT rseest pin
// MOSI=11, MISO=12, SCK=13
#define LED_PIN 36


// --- pin and constant assignments:

// assume Host serial interface is via USB

#define TONE_FREQ 500         // test tone frequency
#define MINDELAY 0            // min/max pulse delay in msec
#define MAXDELAY 5000
#define MINON  100            // min/max pulse time in msec
#define MAXON  5000

volatile int8_t  pulsePin = LED_PIN;   // Teensy pin for pulse out 
volatile int32_t pulseDelay = 0;
volatile int32_t pulseTime = 100000;
volatile boolean runFlag = false;
volatile float freq = 500;
volatile uint8_t level = 255;

// --- instantiate objects

IntervalTimer pulseTimer;    // create a timer for TTLpulse out
IntervalTimer delayTimer;    //  and for the delayed start


MAX11300 maxIO(MAXCVTpin, MAXCSpin) ;
miniBCS bcs;  // instantiate a miniBCS object


ILI9341_t3 tft = ILI9341_t3(TFTCSpin, TFTDCpin, TFTRSTpin);    // instantiate TFT display
XPT2046_Touchscreen ts(TSCSpin);        // set chip select for touchscreen
// Touchscreen alternate setups
//#define TIRQ_PIN  32
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling

// =========================================
//  ===  P U L S E    I N T E R R U P T  === 
//  ========================================

// Example of use Interval Timer
// This is the interrupt called whenever the timer times out

volatile boolean TTLstate = false;

void pulseInterrupt()
{
  if( TTLstate == false) 
  {
     digitalWrite(pulsePin, HIGH);  // turn on pulse
     TTLstate = true;
  }   
  else   
  {
     digitalWrite(pulsePin, LOW);  // turn off pulse
     TTLstate = false;
  }    
}

// ============================================
//  === P U L S E   D E L A Y    T I M E R  === 
//  ===========================================

void pulseStart()
{ 
     delayTimer.end();    // just the one pulse
     pulseTimer.begin( pulseInterrupt, pulseTime);  // and start pulse timer
}


// ==========================
// === S T A R T   C M D ====
// ==========================
//  START p, d, o 

//    p - TTL pin
//    d - pulse delay (ms)
//    o - pulse on/off time (ms)
// ==========================

void startCmd(int arg_cnt, char **args)
{

  
#ifdef DEBUG
Serial.print("Arg cnt:");
Serial.println(arg_cnt);
for ( int i = 0; i < arg_cnt; i++ )
    Serial.println( args[i]);
#endif


 if( arg_cnt >= 4 )  // arg 0 = cmd, 1= pin, 2 = delay, 3 = time => 4 args
 {
     int8_t pulsePin = cmdStr2Num(args[1], 10);
     pinMode(pulsePin, OUTPUT);   // set direction 
     digitalWrite(pulsePin, LOW); // and start it low
     
     int32_t delay  = cmdStr2Num(args[2], 10);    // delay, base 10
     if( (delay >= MINDELAY) && (delay <= MAXDELAY) )
          pulseDelay = delay * 1000;  // need usecs;
            
     int32_t ontime  = cmdStr2Num(args[3], 10);    // delay, base 10
     if( (ontime >= MINDELAY) && (ontime <= MAXDELAY)  )
          pulseTime = ontime * 1000; // need usec;    

     #ifdef DEBUG
        Serial.print(pulsePin);
        Serial.print(",");
        Serial.print(pulseDelay);
        Serial.print(",");
        Serial.println(pulseTime);
     #endif   
  }   
  runFlag = true;    
  Serial.println("Started");   
}

// =========================
// === S T O P    C M D ====
// =========================

void stopCmd(int arg_cnt, char **args)
{   
    Serial.println("Stopped");
       
    runFlag = false;
    pulseTimer.end();   
    digitalWrite(pulsePin, LOW);  // turn off pulse
    TTLstate = false;
    bcs.toneOff();
}

// ==========================================
// === S O L E N O I D   T E S T   C M D ====
//  
//  TEST SOLENOIDS
// ==========================================

void driverCmd(int arg_cnt, char **args)
{   
int ch;

  if( arg_cnt >= 2 )
   {
       ch = cmdStr2Num(args[1], 10);
       Serial.print( "SOL CH:");
       Serial.println(ch);
       bcs.driverOn(ch);
       delay(1000);
       bcs.driverOff(ch);
  }
}

// =========================
// === T O N E    C M D ====
// =========================

void toneCmd(int arg_cnt, char **args)
{   
         
  if( arg_cnt >= 3 )  // arg 0 = cmd, 1= freq, 2 = level
  {
     freq = cmdStr2Float(args[1]);    // frequency
     level = cmdStr2Num(args[2], 10);    // level

     bcs.setTone(freq, TONE_SINE);        // set up tone frequency and waveform 
     bcs.setToneGain(level);    // tone gain is 0 to 255
     bcs.toneOn();

     #ifdef DEBUG
        Serial.print("5 seconds: freq:");
        Serial.print(freq);
        Serial.print(" level:");
        Serial.println(level);  
     #endif   

     delay(5000);
     bcs.toneOff();
     #ifdef DEBUG
         Serial.println("off");   
     #endif       
  }   

}

// =========================
// === T M P     C M D ====
// =========================

void tmpCmd(int arg_cnt, char **args)
{   
   Serial.print("tmp= ");
   Serial.println(maxIO.readInternalTemp());
}


// =========================
// === C M D   H E L P  ====
// =========================

void cmdHelp(int arg_cnt, char **args)
{   

    Serial.print( "Version:");
    Serial.println(VERSION);
    cmdList();
}

uint16_t dacval = 0;

// ====================
//  ===  S E T U P  === 
//  ===================
void setup()
{  
    bcs.begin();   // set up miniBCS hardwware 
    maxIO.begin();  // start up maxim interfcae
                          
//    // basic display setups
    tft.begin();                      // start up display      
    tft.setRotation(1);         // rotate 180 for miniBCS mechanical attachment 
    tft.fillScreen(ILI9341_BLACK);          
    tft.setTextColor(ILI9341_YELLOW);    
    tft.setTextSize(2);       
    tft.println("      miniBCS w/ maxIO");
    tft.println("    with TFT touchscreen");    
    tft.println("    demo for REV I Board"); 
    tft.print  ("      VERSION:");   
    tft.println(VERSION);
    tft.println( maxIO.getID());
    
//    // touchscreen 
//    ts.begin();
//    // Serial.println( ts.bufferSize());
//    ts.setRotation(3);  
   
  // set up command port and list 
    Serial.begin(115200);       // USB serial startup
    while(!Serial);             // wait for serial port connection 

    Serial.print("miniBCS REVI maxIO - V:");
    Serial.println(VERSION);  

    Serial.print(TFTCSpin);
    Serial.print(" ");
    Serial.print(TFTRSTpin);
    Serial.print(" ");
    Serial.println(TFTDCpin);    

    // --- experiment setups:
    bcs.toneReset();   // clear out any previous setups
    bcs.toneOff();         // be sure we start with tone off
    
    bcs.setTone(1000, TONE_SINE);
    bcs.setToneGain(150);
    bcs.toneOn();

    // The cmd.h library is used to set up and parse commands and parameters
    cmdInit(&Serial);        // set up for command parsing
    // add commands
 //   cmdAdd("TTL_OUT",TTLoutCmd);     
    cmdAdd("STOP", stopCmd);
    cmdAdd("DRIVER", driverCmd);
    cmdAdd("TONE", toneCmd);
    cmdAdd("TMP", tmpCmd);
    cmdAdd("???", cmdHelp);

    maxIO.digitalRange(0, 4.0);
    maxIO.digitalRange(1, 8.0);
    if(maxIO.pinMode( 0, digitalOut) == false) Serial.println("bad 0");
    maxIO.DACreference(DACInternal);
    delay(1);
    maxIO.digitalWrite(0, LOW);
    if( maxIO.pinMode( 1, digitalOut) == false) Serial.println("bad 1");
    maxIO.digitalWrite(1, HIGH);
    maxIO.pinMode( 2, analogOut);
    maxIO.DACrange( 2, DACZeroTo10);
    Serial.print( maxIO.readInternalTemp() ); 
   Serial.print(" maxid: ");
   Serial.println( maxIO.getID(), HEX );  
  
}

int32_t pos = 0; 
int32_t oldpos = 0;


boolean wastouched = true;

// ===========================
//  ===  M A I N  L O O P  === 
//  ==========================
void loop()
{

 
  cmdPoll();   // look for commands


//  Serial.println( maxIO.readInternalTemp() );
//  delay(500);

    
  

}
