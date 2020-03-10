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
#ifndef _TRACK_H
#define _TRACK_H

#include <Arduino.h>
#include <Adafruit_NeoTrellisM4.h>
#include "note.h"
#include "euclidean.h"
#include "melody.h"
#include "progression.h"

#define SEQUENCER_MAXLENGTH 64
#define POLY_COUNT 8

class Track {
bool _active;
Euclidean* _rythm;
Melody* _melody;
Progression* _progression;
uint8_t _midiChannel;
uint8_t _stepLength;
uint8_t _stepDevider;
uint8_t _noteLength;
uint8_t _targetStepLength;
uint8_t _velocity;
uint8_t _sequenceLength;
uint8_t _sequenceIdx;
uint8_t _playedNoteIndex;
bool _legato;
Note _stack[POLY_COUNT];
Adafruit_NeoTrellisM4* _trellis;

void stopAllNotes();
void playNote( byte value, byte velocity);
void noteOn( byte value, byte velocity);
void noteOff( byte value, byte velocity);
public:
  Track();
  void init( uint8_t midiChannel, Adafruit_NeoTrellisM4* trellis);
  void setRythm( Euclidean* r);
  Euclidean* getRythm();
  uint8_t getChannel();
  void setNoteLength( uint8_t l);
  void setStepLength( uint8_t l);
  void setVelocity( uint8_t v);
  uint8_t getVelocity();
  void setMelody( Melody* melody);
  Melody* getMelody();
  uint8_t getNoteLength();
  uint8_t getStepLength();
  void tick();
  void stop();
  bool isActive();
  bool toggle();
  bool toggleLegato();
  bool hasLegato();
  void setProgression( Progression* progression);
  bool aboutToRestart();
};

#endif 
