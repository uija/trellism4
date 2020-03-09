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
 #include "storage.h"

Storage::Storage( Adafruit_SPIFlashBase* flash) {
  _flash = flash;
  _flash->begin();
  _ready = false;
}
void Storage::init() {
  if( !_ready) {
    this->readBuffer();
  }
  // check if we are initialized
  const char* test = "SONGMODE";
  bool correct = true;
  for( uint8_t i = 0; i < 8; i++) {
    if( test[i] != _buffer[i]) {
      correct = false;
    }
  }
  if( !correct) {
    // set header
    for( uint8_t i = 0; i < 8; i++) {
      _buffer[i] = test[i];
    }
    // build index
    for( uint8_t i = 0; i < 128; ++i) {
      _buffer[INDEX_OFFSET+i] = 0xFF; // Set empty slots to max, because we only have valid values from 0 to 127
    }
    for( uint32_t i = DATA_OFFSET; i < BUFFERSIZE; ++i) {
      _buffer[i] = 0; // Set rest to 0 
    }
    this->writeBuffer();
  }
}
void Storage::read( uint32_t pos, uint8_t* buffer, uint32_t length) {
  if( !_ready) {
    this->readBuffer();
  }
  for( uint32_t i = 0; i < length; ++i) {
    buffer[i] = _buffer[pos+i];
  }
}
void Storage::write( uint32_t pos, uint8_t* buffer, uint32_t length) {
  if( !_ready) {
    this->readBuffer();
  }
  for( uint32_t i = 0; i < length; ++i) {
    _buffer[pos+i] = buffer[i];
  }
}
void Storage::save() {
  if( _ready) {
    this->writeBuffer();    
  }
}
void Storage::readBuffer() {
  _ready = true;
  _flash->readBuffer( 0, _buffer, BUFFERSIZE);
}
void Storage::writeBuffer() {
  _flash->eraseBlock( 0);
  _flash->writeBuffer( 0, _buffer, BUFFERSIZE);
}
void Storage::loadProject( uint8_t slot, Sequencer* sequencer) {
  if( !_ready) {
    this->readBuffer();
  }
  
  uint32_t position = DATA_OFFSET + slot * PROJECT_SIZE;
  
  // TODO: bpm
  position++;
  position++;

  for( uint8_t i = 0; i < SEQUENCELENGTH; ++i) {
    sequencer->setPatternAt( i, _buffer[position]);
    sequencer->setLengthAt( i, _buffer[position+1], _buffer[position+2]);
    position+=3;
  }
}
void Storage::saveProject( uint8_t slot, Sequencer* sequencer) {
  if( !_ready) {
    this->readBuffer();
  }
  // mark project slot as taken
  _buffer[INDEX_OFFSET+slot] = 1;
  // find project address
  uint32_t position = DATA_OFFSET + slot * PROJECT_SIZE;
  
  // TODO: bpm
  position++;
  position++;
  
  for( uint8_t i = 0; i < SEQUENCELENGTH; ++i) {
    _buffer[position] = sequencer->patternAt( i);
    position++;
    _buffer[position] = sequencer->lengthAt( i);
    position++;
    _buffer[position] = sequencer->factorAt( i);
    position++;
  }
  this->writeBuffer();
}
