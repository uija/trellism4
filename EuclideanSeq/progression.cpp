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
#include "progression.h"
#include "MIDIUSB.h"

/*
uint8_t _root;
uint8_t _length;
uint8_t * _intervals;
uint8_t * _progression;
Chord * _currentChord;
uint8_t _pause;
uint8_t _pausecounter;
uint8_t _idx;
*/

Progression::Progression( uint8_t root, uint8_t pause) {
    _root = root;
    _currentRoot = root;
    _pause = pause;
    _pausecounter = 0;
    _length = 0;
    _intervals = 0;
    _chords = 0;
    _progression = 0;
    _currentChord = 0;
    _idx = 0;
    _currentStep = 0;
    _queuedProgressionIndex = -1;
}
Progression::~Progression() {
    if( _currentChord != 0) {
        delete _currentChord;
    }
}
void Progression::queueProgressonIndex( uint8_t progressionIndex) {
  _queuedProgressionIndex = progressionIndex;
}
void Progression::handleQueue() {
  if( _queuedProgressionIndex < 0) return;
  this->activateProgressionStep( _queuedProgressionIndex);
  _queuedProgressionIndex = -1;
}
void Progression::setMode( SizedIntArray* intervals, SizedIntArray* chords) {
    _intervals = intervals;
    _chords = chords;
}
void Progression::setProgression( SizedIntArray * progression) {
    _progression = progression;
}
void Progression::activateProgressionStep( uint8_t progressionStep) {
  _currentStep = progressionStep;
  _currentRoot = _root + _intervals->data[progressionStep];

  MidiUSB.sendMIDI({0x08, 0x80 | 15, _root, _currentRoot});
  
  if( _currentChord != 0) {
      delete _currentChord;
  }

  _currentChord = new Chord( 3);
  SizedIntArray * chordType = &CHORD_INTERVALS_MAJOR;
  if( _chords->data[progressionStep] == 1) {
      chordType = &CHORD_INTERVALS_MINOR;
  }
  for( uint8_t i = 0; i < 3; ++i) {
      uint8_t note = _currentRoot + chordType->data[i];
      _currentChord->setNote( i, note);
  }
}
void Progression::tick() {
    if( _progression == 0 || _intervals == 0 || _chords == 0) {
        return;
    }
    if( _pausecounter == 0) { // initialize next step
      uint8_t progressionStep = _progression->data[_idx];
      this->activateProgressionStep( progressionStep);
    }

    _pausecounter++;
    if( _pausecounter >= _pause) {
        _pausecounter = 0;
        _idx++;
        if( _idx >= _progression->size) {
            _idx = 0;
        }
    }
}
uint8_t Progression::getScaleRoot() {
    return _root;
}
Chord* Progression::getCurrentChord() {
    return _currentChord;
}
SizedIntArray* Progression::getIntervals() {
    return _intervals;
}
uint8_t Progression::getCurrentRoot() {
    return _currentRoot;
}
SizedIntArray* Progression::getProgression() {
    return _progression;
}
void Progression::start() {
    _pausecounter = 0;
    _idx = 0;
}
void Progression::stop() {
}
uint8_t Progression::getSize() {
  if( _intervals == 0) return 0;
  return _intervals->size;
}
uint8_t Progression::getCurrentStep() {
  return _currentStep;
}
uint8_t Progression::getRoot() {
  return _root;
}
void Progression::setRoot( uint8_t root) {
  _root = root;
}
