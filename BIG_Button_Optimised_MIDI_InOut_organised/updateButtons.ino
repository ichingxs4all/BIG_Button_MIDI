void checkButtons(){

  recordState = digitalRead(BigButton);
  deleteState = digitalRead(ButtonDelete);
  clearState = digitalRead(ButtonClear);
  resetState = digitalRead(ResetButton);
  fillState = digitalRead(FillButton);
  bankButtonState = digitalRead(ButtonBankSelect);


  if (buttonState == HIGH) {
    looper++;
    ClockKeep++;
    for (int i = 0; i < 6; i++) {
      BankPush[i]++;
      int idx = BankPush[i] + NewKnobValue[i];
      digitalWrite(OUTPUT_PINS[i], Sequence[channelMap[i] + BankArrayShift[i]][idx] || Fill[i]);
      if(Sequence[channelMap[i] + BankArrayShift[i]][idx] || Fill[i]) {
        noteOn(i); // Play corresponding MIDi note On
        if(Channel == i + 1 ) digitalWrite(BankLED,HIGH); //Flash the selcted channel on the BigButton LED
      }
    }
    delay(10);
    for (int i = 0; i < 6; i++) {
    digitalWrite(OUTPUT_PINS[i], LOW);
    noteOff(i); // Play corresponding MIDi note On
    }
    buttonState = LOW;
  }

  if (recordState != LastRecordButtonState && recordState == HIGH) {
    if(debug) Serial.println("BigButton pressed ");
    digitalWrite(BankLED, HIGH);
    for (int i = 0; i < 6; i++) {
      if (Channel == channelMap[i]) {
        Sequence[Channel + BankRecord][BankPush[i] + 1 + NewKnobValue[i]] = 1;
        break;
      }
    }
    delay(5);
    
  }
//
  if (bankButtonState == HIGH && Bankprevious[BankArrayNumber] == LOW && millis() - time > debounce) {
    BankState[BankArrayNumber] = !BankState[BankArrayNumber];
    if(debug) Serial.println("Bank button pressed");
    time = millis();
  }

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

//
}