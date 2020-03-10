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
#include "track.h"

Track::Track() {
  _active = true;
  _rythm = 0;
  _stepDevider = 0;
  _stepLength = 8;
  _noteLength = 8;
  _velocity = 96;
  _targetStepLength = 0;
  _sequenceLength = SEQUENCER_MAXLENGTH;
  _sequenceIdx = 0;
  _playedNoteIndex = 0;
}
void Track::noteOn( byte value, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | _midiChannel, value, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void Track::noteOff( byte value, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | _midiChannel, value, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}
void Track::setRythm( Euclidean* r) {
  _rythm = r;
  _sequenceLength = _rythm->getLength();
}
Euclidean* Track::getRythm() {
  return _rythm;
}
void Track::playNote( byte value, byte velocity) {
  // evaluate what slot in the polyphone stack we use
  // first find one that plays the same note
  uint8_t idx = POLY_COUNT;
  for( uint8_t i = 0; i < POLY_COUNT; ++i) {
    if( _stack[i].value == value) {
      idx = i;
    }
  }
  if( idx < POLY_COUNT) { // We have found a note
    if( _legato) { // we need to stop the note before playing it again without legato
      _stack[idx].length = _noteLength;
      return;
    }
    this->noteOff( value, velocity);
    _stack[idx].length = -1;
    _stack[idx].value = -1;
  }
  for( uint8_t i = 0; i < POLY_COUNT; ++i) {
    if( _stack[i].value == -1) {
      idx = i;
      break;
    } else {
      if( _stack[i].length < _stack[idx].length) {
        idx = i;
      }
    }
  }
  _stack[idx].value = value;
  _stack[idx].length = _noteLength;
  this->noteOn( _stack[idx].value, velocity);
}
bool Track::isActive() {
  return _active;
}
bool Track::toggle() {
  _active = !_active;
  if( !_active) {
    this->stopAllNotes();
  }
  return _active;
}
void Track::init( uint8_t midiChannel, Adafruit_NeoTrellisM4* trellis) {
  _midiChannel = midiChannel;
  _trellis = trellis;
  for( uint8_t i = 0; i < POLY_COUNT; ++i) {
    _stack[i].value = -1;
    _stack[i].length = -1;
  }
}
void Track::tick() {
  if( _active) {
    for( uint8_t i = 0; i < POLY_COUNT; ++i) {
      if( _stack[i].value >= 0) {
        _stack[i].length--;
        if( _stack[i].length < 0) {
          this->noteOff( _stack[i].value, _velocity);
          _stack[i].value = -1;
          _stack[i].length = -1;
        }
      }
    }
  }
  if( _stepDevider == 0) {
    if( _rythm != 0 && _rythm->hasPulse( _sequenceIdx)) {
      if( _active) {

        if( _melody != 0) {
          /*
           * There are 2 ways to feed the arp
           * 1. Let the arp take the next note in its arp
           *    whenever a new note is triggered
           *    For this we need to have a second index, that counts up
           *    whenever a note was played
           *    _note = this->melody->getNoteValue( idx);
           * 2. Take a note out of its arp based on the position 
           *    in the step-sequence of the track
           *    That will lead to a lot of skipped notes with less pulses
           *    For this we use the tracks idx, ignoring if a note was played
           *    _note = this->melody->getNoteValue( _playedNoteIndex);
           */
          uint8_t playNote;
          if( _melody->playsChords()) {
              Chord* chord = _melody->getChord( _playedNoteIndex, _progression);
              if( chord != 0) {
                  for( uint8_t i = 0; i < chord->getLength(); ++i) {
                      this->playNote( chord->getNote( i), _noteLength);
                  }
              }
              delete chord;
          } else {
              this->playNote( _melody->getNoteValue( _playedNoteIndex, _progression), _noteLength);
          }
        }    
      }    
      if( _melody != 0) {
        _playedNoteIndex++;
        if( _playedNoteIndex >= _melody->getLength()) {
          _playedNoteIndex = 0;
        }
      }  
    }
    if( _rythm != 0) {
      _sequenceLength = _rythm->getLength();
    }
    _sequenceIdx++;
    if( _sequenceIdx >= _sequenceLength) {
      _sequenceIdx = 0; 
    }
  }

  _stepDevider++;
  if( _stepDevider >= _stepLength) {
    _stepDevider = 0;
    if( _targetStepLength != 0) {
      _stepLength = _targetStepLength;
      _targetStepLength = 0;
    }
  }
}
bool Track::aboutToRestart() {
  return _stepDevider == 0 && _sequenceIdx == 0;
}
void Track::stopAllNotes() {
  for( uint8_t i = 0; i < POLY_COUNT; ++i) {
      if( _stack[i].value >= 0) {
        this->noteOff( _stack[i].value, _velocity);
        _stack[i].value = -1;
        _stack[i].length = -1;
      }
  }
}
void Track::stop() {
  _stepDevider = 0;
  _sequenceIdx = 0;
  _playedNoteIndex = 0;
  this->stopAllNotes();
}
void Track::setNoteLength( uint8_t l) {
  _noteLength = l;
}
void Track::setStepLength( uint8_t l) {
  _targetStepLength = l;
}
uint8_t Track::getChannel() {
  return _midiChannel;
}
uint8_t Track::getNoteLength() {
  return _noteLength;
}
uint8_t Track::getStepLength() {
  if( _targetStepLength != 0) {
    return _targetStepLength;
  }
  return _stepLength;
}
bool Track::toggleLegato() {
  _legato = !_legato;
  return _legato;
}
bool Track::hasLegato() {
  return _legato;
}
void Track::setMelody( Melody* melody) {
  _melody = melody;
}
Melody* Track::getMelody() {
  return _melody;
}
void Track::setProgression( Progression* progression) {
  _progression = progression;
}
void Track::setVelocity( uint8_t v) {
  _velocity = v;
}
uint8_t Track::getVelocity() {
  return _velocity;
}
