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
