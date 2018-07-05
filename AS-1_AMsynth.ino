/* 
_____/\\\\\\\\\________/\\\\\\\\\\\______________________/\\\_        
 ___/\\\\\\\\\\\\\____/\\\/////////\\\________________/\\\\\\\_       
  __/\\\/////////\\\__\//\\\______\///________________\/////\\\_      
   _\/\\\_______\/\\\___\////\\\__________/\\\\\\\\\\\_____\/\\\_     
    _\/\\\\\\\\\\\\\\\______\////\\\______\///////////______\/\\\_    
     _\/\\\/////////\\\_________\////\\\_____________________\/\\\_   
      _\/\\\_______\/\\\__/\\\______\//\\\____________________\/\\\_  
       _\/\\\_______\/\\\_\///\\\\\\\\\\\/_____________________\/\\\_ 
        _\///________\///____\///////////_______________________\///_ 

  5 Knob AM Synth

  Based on Mozzi example
  AMsynth
  
  Andrew Buckie 20180426
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
//#include <mozzi_utils.h>
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

// Declare variables for analog and digital inputs
int  Pot1Val;  // Potentiometer 1 Value
int  Pot2Val;  // Potentiometer 2 Value
int  Pot3Val;  // Potentiometer 3 Value
int  Pot4Val;  // Potentiometer 4 Value
int  Pot5Val;  // Potentiometer 5 Value
int  ExpVal;   // Expression Pedal Value
bool ExpPot1;  // If DIP switch 1 is on, Expression pedal will control Pot1 variable
bool ExpPot2;  // If DIP switch 2 is on, Expression pedal will control Pot2 variable
bool ExpPot3;  // If DIP switch 3 is on, Expression pedal will control Pot3 variable
bool ExpPot4;  // If DIP switch 4 is on, Expression pedal will control Pot4 variable
bool ExpPot5;  // If DIP switch 5 is on, Expression pedal will control Pot5 variable
bool ExpDetct; // Expression pedal plug is inserted

// Map Digital Inputs
#define DIP_1   (2)
#define DIP_2   (3)
#define DIP_3   (4)
#define DIP_4   (5)
#define DIP_5   (6)
#define ExpPlug (7) // This input is wired to the expression pedal jack

// Map Analogue channels
#define Pot1 (2)
#define Pot2 (0)
#define Pot3 (4)
#define Pot4 (1)
#define Pot5 (3)
#define Exp  (5)

#define CONTROL_RATE 64 // powers of 2 please

// audio oscils
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModDepth(COS2048_DATA);

// for scheduling note changes in updateControl()
EventDelay  kNoteChangeDelay;

// synthesis parameters in fixed point formats
Q8n8 ratio; // unsigned int with 8 integer bits and 8 fractional bits
Q24n8 carrier_freq; // unsigned long with 24 integer bits and 8 fractional bits
Q24n8 mod_freq; // unsigned long with 24 integer bits and 8 fractional bits

// for random notes
Q8n0 octave_start_note = 42;

void setup(){
  ratio = float_to_Q8n8(3.0f);   // define modulation ratio in float and convert to fixed-point
  kNoteChangeDelay.set(200); // note duration ms, within resolution of CONTROL_RATE
  aModDepth.setFreq(13.f);     // vary mod depth to highlight am effects
  randSeed(); // reseed the random generator for different results each time the sketch runs
  startMozzi(CONTROL_RATE);
}

void updateControl(){
  // Read all the analog inputs
  Pot1Val = mozziAnalogRead(Pot1); // value is 0-1023
  Pot2Val = mozziAnalogRead(Pot2); // value is 0-1023
  Pot3Val = mozziAnalogRead(Pot3); // value is 0-1023
  Pot4Val = mozziAnalogRead(Pot4); // value is 0-1023
  Pot5Val = mozziAnalogRead(Pot5); // value is 0-1023
  ExpVal  = mozziAnalogRead(Exp);  // value is 0-1023
  
  static Q16n16 last_note = octave_start_note;

  if(kNoteChangeDelay.ready()){

    // change octave now and then
    if(rand((byte)5)==0){
      last_note = 36+(rand((byte)6)*12);
    }

    // change step up or down a semitone occasionally
    if(rand((byte)13)==0){
      last_note += 1-rand((byte)3);
    }

    // change modulation ratio now and then
    if(rand((byte)5)==0){
      ratio = ((Q8n8) 1+ rand((byte)5)) <<8;
    }

    // sometimes add a fractionto the ratio
    if(rand((byte)5)==0){
      ratio += rand((byte)255);
    }

    // step up or down 3 semitones (or 0)
    last_note += 3 * (1-rand((byte)3));

    // convert midi to frequency
    Q16n16 midi_note = Q8n0_to_Q16n16(last_note); 
    carrier_freq = Q16n16_to_Q24n8(Q16n16_mtof(midi_note));

    // calculate modulation frequency to stay in ratio with carrier
    mod_freq = (carrier_freq * ratio)>>8; // (Q24n8   Q8n8) >> 8 = Q24n8

      // set frequencies of the oscillators
    aCarrier.setFreq_Q24n8(carrier_freq);
    aModulator.setFreq_Q24n8(mod_freq);

    // reset the note scheduler
    kNoteChangeDelay.start();
  }
}

int updateAudio(){
  long mod = (128u+ aModulator.next()) * ((byte)128+ aModDepth.next());
  int out = (mod * aCarrier.next())>>16;
  return out;
}

void loop(){
  audioHook();
}
