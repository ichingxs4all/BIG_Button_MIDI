void handleClearButton(int clearState) {
  if (clearState == HIGH) {
    for (int i = 1; i < 42; i++) {
      Sequence[ClearState + BankRecord][i] = 0;
    }
  }
}

void handleFillButton(int fillState) {
  for (int i = 0; i < 6; i++) Fill[i] = 0;
  for (int i = 0; i < 6; i++) {
    if (Channel == channelMap[i] && fillState == HIGH) {
      Fill[i] = 1;
      break;
    }
  }
}

void handleClock(){
  digitalWrite(ClockLed, LOW);
  if(midiClock == midiClockDivider) {
    isr();
    midiClock = 0;
  }
  midiClock++;
}

void flashBankLED() {
  // int delayTimes[] = {200, 180, 160, 140, 120, 100, 80, 60, 40, 20, 60};
  // for (int i = 0; i < 11; i++) {
  //   digitalWrite(BankLED, HIGH);
  //   delay(delayTimes[i]/2);
  //   digitalWrite(BankLED, LOW);
  //   delay(delayTimes[i]/2);
  // }
}