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
#ifndef _EUCLIDEAN_H
#define _EUCLIDEAN_H

#include <Arduino.h>

#define SEQUENCE_MAX_LENGTH 16

class Euclidean {
    uint8_t sequence[SEQUENCE_MAX_LENGTH];
    uint8_t _pulses;
    uint8_t _offset;
    uint8_t _length;
public:
    Euclidean( uint8_t steps, uint8_t pulses, uint8_t offset);
    ~Euclidean();
    uint8_t getLength();
    void initialize( uint8_t steps, uint8_t pulses, uint8_t offset);
    bool hasPulse( uint8_t idx);
    uint8_t getPulses();
    uint8_t getOffset();
};

#endif
