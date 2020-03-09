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
#ifndef _SEQUENCER_H
#define _SEQUENCER_H

#include <Arduino.h>
#include "MIDIUSB.h"

#define SEQUENCELENGTH 128
#define BPMTIME 6000000
#define BPMTIMEFACTOR 24
#define PATTERNPREDELAY 16

struct Step {
  uint8_t pattern;
  uint8_t length;
  uint8_t factor;
};

class Sequencer {
uint8_t _midiChannel;
bool _start = false;
bool _stop = false;
bool _running = false;

uint16_t _patterncount;
uint16_t _intervalcount;
uint16_t _interval;
uint16_t _bpm;

uint8_t _lastSentPattern;

Step* _currentStep;
Step _sequence[SEQUENCELENGTH];

uint8_t _sequenceIdx;

void sendMidiClock();
void sendMidiStart();
void sendMidiStop();
void flushMidi();

bool tick96();
public:
  Sequencer( uint8_t midiChannel, uint16_t bpm);
  void start();
  void stop();
  bool isRunning();
  bool tick();
  void setBpm( uint16_t bpm);
  uint16_t getBpm();

  void sendMidiProgramChange( uint8_t program);

  void clear();
  uint8_t patternAt( uint8_t idx);
  void setPatternAt( uint8_t idx, uint8_t pattern);
  uint8_t lengthAt( uint8_t idx);
  uint8_t factorAt( uint8_t idx);
  void setLengthAt( uint8_t idx, uint8_t length, uint8_t factor);
  bool hasPatternAt( uint8_t idx);
  void resetPatternAt( uint8_t idx);

  uint8_t getCurrentIndex();
};

#endif
