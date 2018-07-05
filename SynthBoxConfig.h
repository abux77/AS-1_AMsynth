#ifndef SynthBoxConfig_h
#define SynthBoxConfig_h

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
int Pot1;
int Pot2;
int Pot3;
int Pot4;
int Pot5;
#define Pot1Default (3)
#define Pot2Default (4)
#define Pot3Default (2)
#define Pot4Default (0)
#define Pot5Default (1)
#define Exp  (5)

#endif        //  #ifndef SynthBoxConfig_h

