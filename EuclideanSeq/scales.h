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
#ifndef _SCALES_H
#define _SCALES_H

#include <Arduino.h>
/*
Ionian: CDEFGAB (Major)
Aeolian: CDEbFGAbBb (Minor)
Dorian: CDEbFGABb
*/

struct SizedIntArray {
    uint8_t size;
    uint8_t * data;
    const char* name;
    SizedIntArray( uint8_t s, uint8_t* d, const char* n) {
        size = s; data = d; name = n;
    }
};

static SizedIntArray INTERVALS_MAJOR( 8, (uint8_t[]){0,2,4,5,7,9,11,12}, "Maj");
static SizedIntArray INTERVALS_MINOR( 8, (uint8_t[]){0,2,3,5,7,8,10,12}, "Min");

static SizedIntArray CHORDS_MINOR( 7, (uint8_t[]){1, 0, 0, 1, 1, 0, 0}, "Min");
static SizedIntArray CHORDS_MAJOR( 7, (uint8_t[]){0, 1, 1, 0, 0, 1, 0}, "Maj");

static SizedIntArray CHORD_INTERVALS_MINOR( 3, (uint8_t[]){0, 3, 7}, "Min");
static SizedIntArray CHORD_INTERVALS_MAJOR( 3, (uint8_t[]){0, 4, 7}, "Maj");

static SizedIntArray PROGRESSION_1564( 4, (uint8_t[]){0,4,5,3}, "I V VI IV");
static SizedIntArray PROGRESSION_1( 1, (uint8_t[]){0}, "I");


#endif
