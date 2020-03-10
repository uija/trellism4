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
#ifndef _PROGRESSION_H
#define _PROGRESSION_H

#include <Arduino.h>
#include "chord.h"
#include "scales.h"

class Progression {
uint8_t _root;
uint8_t _length;
uint8_t _currentStep;
SizedIntArray * _intervals;
SizedIntArray * _chords;
SizedIntArray * _progression;
Chord * _currentChord;
uint8_t _currentRoot;
uint8_t _pause;
uint8_t _pausecounter;
uint8_t _idx;
int8_t _queuedProgressionIndex;

public:
    Progression( uint8_t _root, uint8_t _pause);
    ~Progression(); 
    void setMode( SizedIntArray* intervals, SizedIntArray* chords);
    void setProgression( SizedIntArray * progression);
    void tick();
    void start();
    void stop();
    void queueProgressonIndex( uint8_t progressionIndex);
    void activateProgressionStep( uint8_t progressionIndex);
    void handleQueue();
    Chord* getCurrentChord();
    uint8_t getScaleRoot();
    SizedIntArray* getIntervals();
    SizedIntArray* getProgression();
    void setRoot( uint8_t root);
    uint8_t getRoot();
    uint8_t getCurrentRoot();
    uint8_t getSize();
    uint8_t getCurrentStep();
    
};

#endif
