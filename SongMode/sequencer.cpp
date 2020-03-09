/**
 * MIT License
 * 
 * Copyright (c) Jens Kirchheimer
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

Sequencer::Sequencer( uint8_t midiChannel, uint16_t bpm) {
  _midiChannel = midiChannel;
  _currentStep = 0;
  _sequenceIdx = 0;
  _lastSentPattern = 255;
  this->setBpm( bpm);
  for( uint8_t i = 0; i < SEQUENCELENGTH; ++i) {
    this->resetPatternAt( i);
  }
}

bool Sequencer::tick96() {
  bool patternStart = false;
  this->sendMidiClock();
  // we handle start and stop here, to be in time with the
  // midi clock and do everything sequencer work
  // inside the interrupt loop
  if( _start) {
    _start = false;
    _running = true;
    _lastSentPattern = 255;
    this->sendMidiStart();
    
    _currentStep = &_sequence[0];
  } else if( _stop) {
    _running = false;
    _stop = false;
    _lastSentPattern = 255;
    this->sendMidiStop();
    this->sendMidiProgramChange( _sequence[0].pattern);
  }

  this->flushMidi();

  // only to pattern stuff when running
  if( _running) {
    // everytime the pattern resets, we tell the caller so
    // so the ui can be updated
    if( _patterncount == 0) {
      patternStart = true;
    }
    _patterncount++;
    
    if( _patterncount == (_currentStep->length*_currentStep->factor - PATTERNPREDELAY)) {
      _sequenceIdx++;
      if( _sequence[_sequenceIdx].length == 0) {
        _sequenceIdx = 0;
      }
      if( _sequenceIdx >= SEQUENCELENGTH) {
        _sequenceIdx = 0;
      }
      if( _sequence[_sequenceIdx].pattern != _lastSentPattern) {
        this->sendMidiProgramChange( _sequence[_sequenceIdx].pattern);
        this->flushMidi();
        _lastSentPattern = _sequence[_sequenceIdx].pattern;
      }
    }
    if( _patterncount >= _currentStep->length*_currentStep->factor) {
      _patterncount = 0;
      _currentStep = &_sequence[_sequenceIdx];
    }
  }
  return patternStart;
}
bool Sequencer::tick() {
  bool ret = false;
  if( _intervalcount == 0) {
    ret = this->tick96();
  }
  _intervalcount++;
  if( _intervalcount >= _interval) {
    _intervalcount = 0;
  }
  return ret;
}
void Sequencer::setBpm( uint16_t bpm) {
  _bpm = bpm;
  _interval = BPMTIME / bpm / BPMTIMEFACTOR;
}
uint16_t Sequencer::getBpm() {
  return _bpm;
}
void Sequencer::start() {
  _start = true;
}
void Sequencer::stop() {
  _stop = true;
  _sequenceIdx = 0;
  _intervalcount = 0;
  _patterncount = 0;
}
bool Sequencer::isRunning() {
  return _running;
}
void Sequencer::sendMidiClock() {
  MidiUSB.sendMIDI( {0x0F, 0xF8, 0, 0});
}
void Sequencer::sendMidiStart() {
  MidiUSB.sendMIDI( {0x0F, 0xFA, 0, 0});
}
void Sequencer::sendMidiStop() {
  MidiUSB.sendMIDI( {0x0F, 0xFC, 0, 0});
}
void Sequencer::sendMidiProgramChange( uint8_t program) {
  MidiUSB.sendMIDI( {0x0C, 0xC0 | _midiChannel, program, 0}); 
}
void Sequencer::flushMidi() {
  MidiUSB.flush();
}
uint8_t Sequencer::patternAt( uint8_t idx) {
  if( idx >= SEQUENCELENGTH) {
    return 0;
  }
  return _sequence[idx].pattern;
}
void Sequencer::setPatternAt( uint8_t idx, uint8_t pattern) {
  if( idx >= SEQUENCELENGTH) {
    return;
  }
  _sequence[idx].pattern = pattern;
}
uint8_t Sequencer::lengthAt( uint8_t idx) {
  if( idx >= SEQUENCELENGTH) {
    return 0;
  }
  return _sequence[idx].length;
}
uint8_t Sequencer::factorAt( uint8_t idx) {
  if( idx >= SEQUENCELENGTH) {
    return 0;
  }
  return _sequence[idx].factor;
}
void Sequencer::setLengthAt( uint8_t idx, uint8_t length, uint8_t factor) {
  if( idx >= SEQUENCELENGTH) {
    return;
  }
  _sequence[idx].length = length;
  _sequence[idx].factor = factor;
}
bool Sequencer::hasPatternAt( uint8_t idx) {
  if( idx >= SEQUENCELENGTH) {
    return false;
  }
  return _sequence[idx].length > 0;
}
void Sequencer::resetPatternAt( uint8_t idx) {
  _sequence[idx].length = 0;
  _sequence[idx].factor = 0;
  _sequence[idx].pattern = 0;
}
uint8_t Sequencer::getCurrentIndex() {
  return _sequenceIdx;
}
void Sequencer::clear() {
  for( uint8_t i = 0; i < SEQUENCELENGTH; ++i) {
    this->resetPatternAt( i);
  }
}
