void setupPins(){
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
}