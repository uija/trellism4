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
#ifndef _SEQUENCER_H
#define _SEQUENCER_H

#include <Adafruit_NeoTrellisM4.h>
#include "track.h"
#include "progression.h"

#define BPMTIME 6000000
#define BPMTIMEFACTOR 24
#define TRACKCOUNT 8

class Sequencer {
Adafruit_NeoTrellisM4* _trellis;
Progression* _progression;
short _bpm;
int _interval;
int _intervalCount;
bool _running;
Track _tracks[TRACKCOUNT];

void tick96();

public:
  Sequencer( Adafruit_NeoTrellisM4* t);
  void setBpm( short bpm);
  short getBpm();
  void tick();
  bool toggle();
  Track* getTrack( uint8_t t);
  bool isRunning();
  void setProgression( Progression* progression);
};

#endif
