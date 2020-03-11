/**
 * MIT License
 * 
 * Copyright (c) 2020 Jens Kirchheimer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "sequencer.h"
#include "MIDIUSB.h"

Sequencer::Sequencer( Adafruit_NeoTrellisM4* t) {
  _trellis = t;
  this->setBpm( 120);
  _intervalCount = _interval;
  _running = false;

  for( uint8_t i = 0; i < TRACKCOUNT; ++i) {
    _tracks[i].init( i, _trellis);
  }
}

void Sequencer::setBpm( short bpm) {
    _bpm = bpm;
    _interval = BPMTIME / _bpm / BPMTIMEFACTOR;  
}
short Sequencer::getBpm() {
  return _bpm;
}
void Sequencer::tick() {
  if( !_running) {
    return;
  }
  _intervalCount ++;
  if( _intervalCount >= _interval) {
      this->tick96();
      _intervalCount -= _interval;
  }
}
void Sequencer::tick96() {
  // {0x0F, 0xFA, 0, 0} - Start
  // {0x0F, 0xF8, 0, 0} - Clock
  MidiUSB.sendMIDI( {0x0F, 0xF8, 0, 0});
  MidiUSB.flush();
  if( _progression != 0 && _tracks[0].aboutToRestart()) {
    _progression->handleQueue();
  }
  for( uint8_t i = 0; i < TRACKCOUNT; ++i) {
    _tracks[i].tick();
  } 
  MidiUSB.flush();
}

bool Sequencer::toggle() {
  _running = !_running;
  if( !_running) {
    _intervalCount = _interval;
    for( uint8_t i = 0; i < TRACKCOUNT; ++i) {
      _tracks[i].stop();
    }
  }
  return _running;
}
bool Sequencer::isRunning() {
  return _running;
}
Track* Sequencer::getTrack( uint8_t t) {
  if( t >= TRACKCOUNT) {
    return 0;
  }
  return &_tracks[t];
}
void Sequencer::setProgression( Progression* progression) {
  _progression = progression;
}
