#include <MIDI.h>

// Simple tutorial on how to receive and send MIDI messages.
// Here, when receiving any message on channel 4, the Arduino
// will blink a led and play back a note for 1 second.

MIDI_CREATE_DEFAULT_INSTANCE();

// Optimized Performance Sequencer by Sam Battle
// Cleaned and improved for timing, readability, and performance

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

const int clkIn = 2;
const int ButtonBankSelect = 3;
const int ButtonClear = 4;
const int FillButton = 5;
const int ResetButton = 6;
const int ButtonDelete = 7;

const int OUTPUT_PINS[6] = {8, 9, 10, 11, 12, 13};

const int ChannelSelect = A0;
const int StepsPot = A1;
const int ShiftPot = A2;
const int ClockLed = A3;
const int BankLED = A4;
const int BigButton = A5;

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

long debounce = 150;
long time = 0;
int BankState[7] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW};
int Bankprevious[7] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};

int LastRecordButtonState = 0;
int LastResetButtonState = 0;

int clock_timer = 0;

const int clock_pulse_length = 40;

const bool debug = false;

const int midiClockDivider = 8;
int midiClock = 0;

int knobVal;

void setup() {

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleClock(handleClock);
  
  if(debug)Serial.begin(115200);

  for (int i = 0; i < 6; i++) pinMode(OUTPUT_PINS[i], OUTPUT);
  pinMode(BankLED, OUTPUT);
  pinMode(ClockLed, OUTPUT);
  pinMode(clkIn, INPUT);
  pinMode(BigButton, INPUT);
  pinMode(ButtonClear, INPUT);
  pinMode(ButtonDelete, INPUT);
  pinMode(ButtonBankSelect, INPUT);
  pinMode(ResetButton, INPUT);
  pinMode(FillButton, INPUT);

  flashBankLED();

  attachInterrupt(digitalPinToInterrupt(clkIn), isr, RISING);
}

void loop() {
  MIDI.read(); 
  // if(debug) {
  //   Serial.print("Steps: ");
  //   Serial.print(steps);
  //   Serial.print(" ,  Channel: ");
  //   Serial.print(Channel);   
  //   Serial.print(" , Shift: ");
  //   Serial.println(knobVal);
  // }

  // if ((clock_timer > 0) && (millis() - clock_timer > clock_pulse_length)) { 
  digitalWrite(ClockLed,LOW); // Set clock pulse low after 20 msec 
  // clock_timer = 0;  
  // }

  int recordState = digitalRead(BigButton);
  int deleteState = digitalRead(ButtonDelete);
  int clearState = digitalRead(ButtonClear);
  int resetState = digitalRead(ResetButton);
  int fillState = digitalRead(FillButton);
  int bankButtonState = digitalRead(ButtonBankSelect);

  if (buttonState == HIGH) {
    looper++;
    ClockKeep++;
    for (int i = 0; i < 6; i++) {
      BankPush[i]++;
      int idx = BankPush[i] + NewKnobValue[i];
      digitalWrite(OUTPUT_PINS[i], Sequence[channelMap[i] + BankArrayShift[i]][idx] || Fill[i]);
    }
    delay(10);
    for (int i = 0; i < 6; i++) digitalWrite(OUTPUT_PINS[i], LOW);
    buttonState = LOW;
  }

  if (recordState != LastRecordButtonState && recordState == HIGH) {
    for (int i = 0; i < 6; i++) {
      if (Channel == channelMap[i]) {
        Sequence[Channel + BankRecord][BankPush[i] + 1 + NewKnobValue[i]] = 1;
        break;
      }
    }
    delay(5);
  }

  if (bankButtonState == HIGH && Bankprevious[BankArrayNumber] == LOW && millis() - time > debounce) {
    BankState[BankArrayNumber] = !BankState[BankArrayNumber];
    if(debug) Serial.println("Bank button pressed");
    time = millis();
  }

  updateShiftKnob();
  updateChannelSelection();
  handleClearButton(clearState);
  handleFillButton(fillState);

  if (deleteState == HIGH) 
  {
    Sequence[Channel + BankRecord][looper + 1] = 0;
    if(debug) Serial.println("Delete button pressed");
  }
  if (resetState != LastResetButtonState && resetState == HIGH) {
    looper = ClockKeep = 0;
    for (int i = 0; i < 6; i++) BankPush[i] = 0;
    if(debug) Serial.println("Reset button pressed");
  }

  steps = getStepLength(analogRead(StepsPot));
  if (looper >= steps) looper = 0;
  if (ClockKeep >= 32) ClockKeep = looper = 0;
  for (int i = 0; i < 6; i++) {
    if ((BankPush[i] + NewKnobValue[i]) >= steps) BankPush[i] = 0;
  }

  LastRecordButtonState = recordState;
  LastResetButtonState = resetState;
  Bankprevious[BankArrayNumber] = bankButtonState;
}

int getStepLength(int analogVal) {
  if (analogVal < 150) return 1;
  if (analogVal < 300) return 2;
  if (analogVal < 500) return 4;
  if (analogVal < 750) return 8;
  if (analogVal < 1000) return 16;
  return 32;
}

void updateShiftKnob() {
  knobVal = analogRead(ShiftPot) / 127;
  knobVal = constrain(knobVal, 0, 8);
  for (int i = 0; i < 6; i++) {
    NewKnobValue[i] = knobVal;
  }
}

void updateChannelSelection() {
  int chVal = analogRead(ChannelSelect);
  int chIndex = 0;
  if (chVal > 1000) chIndex = 5;
  else if (chVal > 750) chIndex = 4;
  else if (chVal > 420) chIndex = 3;
  else if (chVal > 240) chIndex = 2;
  else if (chVal > 170) chIndex = 1;

  Channel = channelMap[chIndex];
  BankArrayNumber = chIndex + 1;
  BankRecord = BankState[BankArrayNumber] ? 3 : 0;
  BankArrayShift[chIndex] = BankRecord;
  ClearState = Channel;
}

void handleClearButton(int clearState) {
  if (clearState == HIGH) {
     if(debug) Serial.println("Clear button pressed");
    for (int i = 1; i < 42; i++) {
      Sequence[ClearState + BankRecord][i] = 0;
    }
  }

}

void handleFillButton(int fillState) {
  for (int i = 0; i < 6; i++) Fill[i] = 0;
  for (int i = 0; i < 6; i++) {
    if (Channel == channelMap[i] && fillState == HIGH) {
      if(debug) Serial.println("Fill button pressed");
      Fill[i] = 1;
      break;
    }
  }
}

void flashBankLED() {
  int delayTimes[] = {200, 180, 160, 140, 120, 100, 80, 60, 40, 20, 60};
  for (int i = 0; i < 11; i++) {
    digitalWrite(BankLED, HIGH);
    delay(delayTimes[i]/2);
    digitalWrite(BankLED, LOW);
    delay(delayTimes[i]/2);
  }
}

void isr() {
  digitalWrite(ClockLed, LOW);
  buttonState = HIGH;
  clock_timer = millis();
  digitalWrite(ClockLed, HIGH);
}

void handleClock(){
  midiClock++;
  if(midiClock==midiClockDivider) {
    isr();
    midiClock = 0;
  }
  
}