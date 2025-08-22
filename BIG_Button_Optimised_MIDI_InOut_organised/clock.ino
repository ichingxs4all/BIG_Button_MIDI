void checkClock(){
  if ((clock_timer > 0) && (millis() - clock_timer >= clock_pulse_length)) { 
  digitalWrite(ClockLed,LOW); // Set clock pulse low after 20 msec 
  if(debug)Serial.println(" CLOCK LOW");
  clock_timer = 0;  
  }
}