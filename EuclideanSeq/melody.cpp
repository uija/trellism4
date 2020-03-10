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
#include "melody.h"
#include "scales.h"

SingleNote::SingleNote() : Melody( 1) {
    _offset = 0;
}
SingleNote::SingleNote( int8_t offset) : Melody( 1) {
    _offset = offset;
}
uint8_t SingleNote::getNoteValue( uint8_t idx, Progression* progression) {
    if( progression == 0) {
        return 0;
    }
    return progression->getCurrentRoot() + _offset;
}

FixedNote::FixedNote() : Melody( 1) {
  _note = 64;
}
FixedNote::FixedNote( uint8_t note) : Melody( 1) {
  _note = note;
}
uint8_t FixedNote::getNoteValue( uint8_t idx, Progression* progression) {
  return _note;
}



Arp::Arp( uint8_t size, int8_t offset) : Melody( size){
    _size = size;
    _offset = offset;
}
uint8_t Arp::getNoteValue( uint8_t idx, Progression* progression) {
    if( progression == 0) {
        return 0;
    }
    // currently we ignore the settings and just walk the chord
    Chord* chord = progression->getCurrentChord();
    if( chord == 0) {
        return 0;
    }
    return chord->getNote( idx, _offset);
}




Pad::Pad( uint8_t spread, int8_t offset) : Melody( 1, true) {
    _spread = spread;
    _offset = offset;
}
Chord* Pad::getChord( uint8_t idx, Progression* progression) { 
    if( progression == 0) {
        return 0;
    }
    Chord * orig = progression->getCurrentChord();
    Chord * mine = new Chord( orig->getLength());
    for( uint8_t i = 0; i < orig->getLength(); ++i) {
        mine->setNote( i, orig->getNote( i) + _offset);
    }
    return mine;
}


Random::Random( uint8_t range, int8_t offset) : Melody( 1) {
    _range = range;
    _offset = offset;
    _chordnotes = false;
}
Random::Random( uint8_t range, int8_t offset, bool chordNotes) : Melody( 1) {
    _range = range;
    _offset = offset;
    _chordnotes = chordNotes;
}
uint8_t Random::getRandomScaleNote( Progression* progression) {
    SizedIntArray * intervals = progression->getIntervals();
    uint8_t max = intervals->size * _range;
    uint8_t idx = random( max);
    uint8_t offset = 0;
    while( idx >= intervals->size) {
        idx -= intervals->size;
        offset+=12;
    }
    return progression->getScaleRoot() + _offset + offset + intervals->data[idx];
}
uint8_t Random::getRandomChordNote( Progression* progression) {
    Chord* c = progression->getCurrentChord();
    uint8_t max = c->getLength() * _range;
    uint8_t idx = random( max);
    uint8_t offset = 0;
    while( idx >= c->getLength()) {
        idx -= c->getLength();
        offset+=12;
    }
    return c->getNote( idx) + _offset + offset;
}

uint8_t Random::getNoteValue( uint8_t idx, Progression* progression) {
    if( progression == 0) {
        return 0;
    }
    if( _chordnotes) {
        return this->getRandomChordNote( progression);
    } else {
        return this->getRandomScaleNote( progression);
    }
}
