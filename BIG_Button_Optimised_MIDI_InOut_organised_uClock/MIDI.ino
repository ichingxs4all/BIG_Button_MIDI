void noteOn(int channel){
 MIDI.sendNoteOn(note[channel], velocity,midichannel);
}
  
void noteOff(int channel){
 MIDI.sendNoteOff(note[channel], 0, midichannel);
}
