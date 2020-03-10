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
#include "euclidean.h"

Euclidean::Euclidean( uint8_t steps, uint8_t pulses, uint8_t offset) {
    this->initialize( steps, pulses, offset);
}
Euclidean::~Euclidean() {

}
void Euclidean::initialize( uint8_t steps, uint8_t pulses, uint8_t offset) {
  _pulses = pulses;
  _offset = offset;
  if( steps > SEQUENCE_MAX_LENGTH) {
      steps = SEQUENCE_MAX_LENGTH;
  }
  _length = steps;

  for( uint8_t i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
      sequence[i] = 0;
  }

  uint8_t bucket = 0; //out variable to add pulses together for each step
  int a = steps - 1 + offset;
  if( a >= steps) {
      a -= steps;
  }

  //fill array with rhythm
  for( uint8_t i = 0 ; i < steps ; ++i){
      bucket += pulses;
      if(bucket >= steps) {
          bucket -= steps;
          sequence[a--] = 1;
      } else {
          sequence[a--] = 0;
      }
      if( a < 0) {
          a = steps - 1;
      }
  }
}
uint8_t Euclidean::getLength() {
    return _length;
}
bool Euclidean::hasPulse( uint8_t idx) {
    while( idx > _length) {
        idx -= _length;
    }
    return sequence[idx] == 1;
}
uint8_t Euclidean::getPulses() {
  return _pulses;
}
uint8_t Euclidean::getOffset() {
  return _offset;
}
