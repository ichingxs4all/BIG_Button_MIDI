#include <uClock.h>
#include <MIDI.h>

// Simple tutorial on how to receive and send MIDI messages.
// Here, when receiving any message on channel 4, the Arduino
// will blink a led and play back a note for 1 second.

MIDI_CREATE_DEFAULT_INSTANCE();


// Optimized Performance Sequencer by Sam Battle
// Cleaned and improved for timing, readability, and performance

#define debug false

//Pins used 
// Pin 0 = Midi Out
// Pin 1 = Midi In
// Pin 2 = Clock In 
// Pin 3 = Bank select button
// Pin 4 = Clear button
// Pin 5 = Fill button
// Pin 6 = Reset button
// Pin 7 = Delete button
// Pin 8 ,9 , 10, 11, 12 & 13 = Jack Output 1,2,3,4,5
// Pin 14 (A0) = Channel Select switch
// Pin 15 (A1) = Steps pot
// Pin 16 (A2) = Shift pot
// Pin 17 (A3) = Clock Led
// Pin 18 (A4) = Bank led
// Pin 19 (A5) = Big button

const int note[6] = { 36, 38, 42, 46, 45, 49}; //Use the GM Key Map information to select drum sound

#define clkIn 2
#define ButtonBankSelect 3
#define ButtonClear 4
#define FillButton 5
#define ResetButton 6
#define ButtonDelete 7

#define ChannelSelect A0
#define StepsPot A1
#define ShiftPot A2
#define ClockLed A3
#define BankLED A4
#define BigButton A5
#define clock_pulse_length 20

#define midichannel 10
#define midiClockDivider 24
#define debounce 150
#define velocity 127

// MIDI clock, start, stop, note on and note off byte definitions - based on MIDI 1.0 Standards.
#define MIDI_CLOCK 0xF8
#define MIDI_START 0xFA
#define MIDI_STOP 0xFC

/*
const int ChannelSelect = A0;
const int StepsPot = A1;
const int ShiftPot = A2;
const int ClockLed = A3;
const int BankLED = A4;
const int BigButton = A5;

const int clkIn = 2;
const int ButtonBankSelect = 3;
const int ButtonClear = 4;
const int FillButton = 5;
const int ResetButton = 6;
const int ButtonDelete = 7;
*/

const int OUTPUT_PINS[6] = {8, 9, 10, 11, 12, 13};


int Sequence[14][43] = {0};
int Fill[6] = {0};
int BankPush[6] = {0};
int NewKnobValue[6] = {0};
int BankArrayShift[6] = {0};
int channelMap[6] = {1, 2, 3, 7, 8, 9};

int buttonState = LOW;
int looper = 0;
int steps = 16;
int ClockKeep = 0;
int Channel = 1;
int BankRecord = 0;
int ClearState = 1;
int BankArrayNumber = 1;

long time = 0;

int BankState[7] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW};
int Bankprevious[7] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};

int LastRecordButtonState = 0;
int LastResetButtonState = 0;

int midiClock = 0;
long clock_timer = 0;
int knobVal;
bool recordState,deleteState, clearState,resetState,fillState,bankButtonState;

// external or internal sync?
bool _external_sync_on = false;

uint8_t bpm_blink_timer = 1;

void setup() {
  setup_uClock();
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  MIDI.setHandleClock(onExternalClock);
  MIDI.setHandleStart(onExternalStart);
  MIDI.setHandleStop(onExternalStop);

  setupPins();
  attachInterrupt(digitalPinToInterrupt(clkIn), isr, RISING);
  flashBankLED(); //Little LED animation signaling we are alive
}

void loop() {
  digitalWrite(BankLED,LOW);
  
  MIDI.read(); 
  checkClock();
  checkButtons();

  updateShiftKnob();
  updateChannelSelection();

  handleClearButton(clearState);
  handleFillButton(fillState);

  // do we need to external sync?
  if (_external_sync_on) {
    // watch for external sync signal income
    bool signal_income = true; // your external input signal check will be this condition result
    if (signal_income) {
      // at each clockMe call uClock will process and handle external/internal syncronization
      uClock.clockMe();
    }
  }
  

}


void isr() {
  buttonState = HIGH;
  clock_timer = millis();
  digitalWrite(ClockLed, HIGH);
}

