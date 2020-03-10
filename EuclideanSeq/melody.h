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
#ifndef _MELODY_H
#define _MELODY_H

#include <Arduino.h>
#include "progression.h"
#include "chord.h"

class Melody {
protected:
    uint8_t _length;
    bool _playsChords;
    int8_t _offset;
public:
    Melody( uint8_t length){ _length = length; _playsChords = false; _offset = 0; }
    Melody( uint8_t length, bool playsChords){ _length = length; _playsChords = playsChords; }
    virtual ~Melody() {}
    bool playsChords() { return _playsChords; }
    virtual uint8_t getNoteValue( uint8_t idx, Progression* progression) { return 0; } 
    virtual Chord* getChord( uint8_t idx, Progression* progression)  { return 0; } 
    uint8_t getLength() { return _length; }
    int8_t getOffset() { return _offset; }
    void setOffset( int8_t offset) { _offset = offset; }
};
class FixedNote : public Melody {
  uint8_t _note;
public: 
  FixedNote();
  FixedNote( uint8_t note);
  ~FixedNote() {}
  uint8_t getNoteValue( uint8_t idx, Progression* progression);
  Chord* getChord( uint8_t idx, Progression* progression) { return 0; }
};

class SingleNote : public Melody {
public:
    SingleNote( int8_t offset);
    SingleNote();
    ~SingleNote(){};
    uint8_t getNoteValue( uint8_t idx, Progression* progression);
    Chord* getChord( uint8_t idx, Progression* progression) { return 0; }
};

class Arp : public Melody {
uint8_t _size;
public:
    Arp( uint8_t size, int8_t offset);
    ~Arp(){};
    uint8_t getNoteValue( uint8_t idx, Progression* progression);
    Chord* getChord( uint8_t idx, Progression* progression) { return 0; }
};

class Pad : public Melody {
uint8_t _spread;
public:
    Pad( uint8_t spread, int8_t offset);
    ~Pad(){};
    uint8_t getNoteValue( uint8_t idx, Progression* progression) { return 0; }
    Chord* getChord( uint8_t idx, Progression* progression);
};

class Random : public Melody {
uint8_t _range;
uint8_t _randMax;
bool _chordnotes;

uint8_t getRandomScaleNote( Progression* progression);
uint8_t getRandomChordNote( Progression* progression);

public:
    Random( uint8_t rangeInOctaveCounts, int8_t offset);
    Random( uint8_t rangeInOctaveCounts, int8_t offset, bool chordNotes);
    ~Random(){};
    uint8_t getNoteValue( uint8_t idx, Progression* progression);
    Chord* getChord( uint8_t idx, Progression* progression) { return 0; }
};

#endif
