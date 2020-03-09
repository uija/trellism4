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
#ifndef _STORAGE_H
#define _STORAGE_H

#include "Adafruit_SPIFlash.h"
#include "sequencer.h"

#define BUFFERSIZE 40000
#define INDEX_OFFSET 8
#define INDEX_SIZE 128
#define DATA_OFFSET INDEX_OFFSET+INDEX_SIZE
#define PROJECT_SIZE 400

class Storage {
Adafruit_SPIFlashBase* _flash;
uint8_t _buffer[BUFFERSIZE];
bool _ready;

void readBuffer();
void writeBuffer();
public:
  Storage( Adafruit_SPIFlashBase* flash);
  void init();
  void save();
  void read( uint32_t pos, uint8_t* buffer, uint32_t length);
  void write( uint32_t pos, uint8_t* buffer, uint32_t length);
  void loadProject( uint8_t slot, Sequencer* sequencer);
  void saveProject( uint8_t slot, Sequencer* sequencer);
};
#endif
