
void setup_uClock(){
   // inits the clock library
  uClock.init();

  // avaliable output PPQN resolutions for this example
  // [ uClock.PPQN_48, uClock.PPQN_96, uClock.PPQN_384, uClock.PPQN_480, uClock.PPQN_960 ]
  // not mandatory to call, the default is 96PPQN if not set
  uClock.setOutputPPQN(uClock.PPQN_96);

  // you need to use at least one!
  uClock.setOnOutputPPQN(onOutputPPQNCallback);
  uClock.setOnStep(onStepCallback);
  // multi sync output signatures avaliable
  // normaly used by eurorack modular modules
  uClock.setOnSync1(onSync1Callback);
  uClock.setOnSync2(onSync2Callback);
  uClock.setOnSync4(onSync4Callback);
  // midi sync standard
  uClock.setOnSync24(onSync24Callback);
  // some korg machines do 48ppqn
  uClock.setOnSync48(onSync48Callback);

  uClock.setOnClockStart(onClockStartCallback);
  uClock.setOnClockStop(onClockStopCallback);

  // set external sync mode?
  if (_external_sync_on) {
    uClock.setClockMode(uClock.EXTERNAL_CLOCK);
  }

  // starts clock
  uClock.start();
}


void handle_bpm_led(uint32_t tick) {
  // BPM led indicator
  if (!(tick % (96)) || (tick == 1)) {  // first compass step will flash longer
    bpm_blink_timer = 8;
    digitalWrite(ClockLed, HIGH);
  } else if (!(tick % (24))) {  // each quarter led on
    digitalWrite(ClockLed, HIGH);
  } else if (!(tick % bpm_blink_timer)) {  // get led off
    digitalWrite(ClockLed, LOW);
    bpm_blink_timer = 1;
  }
}

// the main uClock PPQN resolution ticking
void onOutputPPQNCallback(uint32_t tick) {
  // tick your sequencers or tickable devices...
}

void onStepCallback(uint32_t step) {
  // triger step data for sequencer device...
}

// The callback function called by uClock each Pulse of 1PPQN clock resolution.
void onSync1Callback(uint32_t tick) {
  // send sync signal to...
}

// The callback function called by uClock each Pulse of 2PPQN clock resolution.
void onSync2Callback(uint32_t tick) {
  // send sync signal to...
}

// The callback function called by uClock each Pulse of 4PPQN clock resolution.
void onSync4Callback(uint32_t tick) {
  // send sync signal to...
  // Send MIDI_CLOCK to external gears
  using namespace midi;
  MIDI.sendRealTime(Clock);
  handle_bpm_led(tick);
}

// The callback function called by uClock each Pulse of 24PPQN clock resolution.
void onSync24Callback(uint32_t tick) {
  // send sync signal to...
}

// The callback function called by uClock each Pulse of 48PPQN clock resolution.
void onSync48Callback(uint32_t tick) {
  // send sync signal to...
}

// The callback function called when clock starts by using uClock.start() method.
void onClockStartCallback() {
  using namespace midi;
  // send start signal to...
  MIDI.sendRealTime(Start);
}

// The callback function called when clock stops by using uClock.stop() method.
void onClockStopCallback() {
   using namespace midi;
  // send stop signal to...
   MIDI.sendRealTime(Stop);
  digitalWrite(ClockLed, LOW);
}

// External clock handlers
void onExternalClock() {
  uClock.clockMe();
}

void onExternalStart() {
  uClock.start();
}

void onExternalStop() {
  uClock.stop();
  digitalWrite(ClockLed, LOW);
}