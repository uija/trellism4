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
#include <Adafruit_NeoTrellisM4.h>
#include "Interrupt.h"
#include "sequencer.h"
#include "track.h"
#include "euclidean.h"
#include "melody.h"
#include "progression.h"

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

#define BTNCOUNT 8

#define BTN_STARTSTOP 0
#define BTN_BPMLENGTH 1
#define BTN_ROOTNOTE 2
#define BTN_UNKNOWN 3
#define BTN_EUCSTEPS 4
#define BTN_EUCPULSES 5
#define BTN_EUCOFFSET 6
#define BTN_SHIFT 7

Sequencer seq( &trellis);
Progression progression( 60, 16);


uint8_t activeTrack = 0;

uint32_t passiveColor[BTNCOUNT];
uint32_t activeColor[BTNCOUNT];
bool btnDown[BTNCOUNT];

void setup() {
  progression.setMode( &INTERVALS_MINOR, &CHORDS_MINOR);
  progression.setProgression( &PROGRESSION_1564);
  progression.activateProgressionStep( 0);

  seq.setProgression( &progression);

  Serial.begin( 115200);
  trellis.begin();
  trellis.setBrightness( 50);

  trellis.enableUSBMIDI( true);
  trellis.setUSBMIDIchannel( 1);

  // for every column
  for( int8_t c = -8; c < 16; ++c) {
    // shadows
    for( int8_t s = -3; s < 4; ++s) {
      // row
      for( uint8_t r = 0; r < 4; ++r) {
        int8_t x = c + s - r;
        if( x >= 0 && x < 8) {
          switch( s) {
            case 3:
            case -3:
              trellis.setPixelColor( r*8+x, 0x000000);
              break;
            case 2:
            case -2:
              trellis.setPixelColor( r*8+x, 0x005555);
              break;
            case 1:
            case -1:
              trellis.setPixelColor( r*8+x, 0x00CCCC);
              break;
            default:
              trellis.setPixelColor( r*8+x, 0x00FFFF);
          }
        }
      }
    }
    delay( 100);
  }

  for( uint8_t i = 1; i < BTNCOUNT; ++i) {
    btnDown[i] = false;
  }
  activeColor[BTN_STARTSTOP] = 0xFF0000;
  activeColor[BTN_BPMLENGTH] = 0xFFE5CC;
  activeColor[BTN_ROOTNOTE] = 0xFFFFCC;
  activeColor[BTN_UNKNOWN] = 0x000000; //0xE5FFCC;
  activeColor[BTN_EUCSTEPS] = 0xCCFFFF;
  activeColor[BTN_EUCPULSES] = 0xE5CCFF;
  activeColor[BTN_EUCOFFSET] = 0xFFCCE5;
  activeColor[BTN_SHIFT] = 0xFFFFFF;

  passiveColor[BTN_STARTSTOP] = 0x00FF00;
  passiveColor[BTN_BPMLENGTH] = 0xFF8000;
  passiveColor[BTN_ROOTNOTE] = 0xFFFF00;
  passiveColor[BTN_UNKNOWN] = 0x000000; // 0x4C9900;
  passiveColor[BTN_EUCSTEPS] = 0x009999;
  passiveColor[BTN_EUCPULSES] = 0x6600CC;
  passiveColor[BTN_EUCOFFSET] = 0xCC0066;
  passiveColor[BTN_SHIFT] = 0x111111;

  for( uint8_t i = 0; i < BTNCOUNT; ++i) {
    trellis.setPixelColor( i, passiveColor[i]);
  }

  // init tracks
  for( uint8_t i = 0; i < TRACKCOUNT; ++i) {
    trellis.setPixelColor( 24+i, seq.getTrack( i)->isActive() ? 0x001100 : 0x110000);
  }
  trellis.setPixelColor( 24+activeTrack, seq.getTrack( activeTrack)->isActive() ? 0x00FF00 : 0xFF0000);

  TC.startTimer(10, isr);

  seq.getTrack( 0)->setRythm( new Euclidean( 16, 1, 0)); // kick
  seq.getTrack( 0)->setMelody( new FixedNote( 64));
  seq.getTrack( 0)->setProgression( &progression);
  
  seq.getTrack( 1)->setRythm( new Euclidean( 16, 1, 0)); // snare
  seq.getTrack( 1)->setMelody( new FixedNote( 64));
  seq.getTrack( 1)->setProgression( &progression);
  
  seq.getTrack( 2)->setRythm( new Euclidean( 16, 1, 0)); // hihat
  seq.getTrack( 2)->setMelody( new FixedNote( 64));
  seq.getTrack( 2)->setProgression( &progression);
  

  seq.getTrack( 3)->setRythm( new Euclidean( 16, 1, 0)); // bass
  seq.getTrack( 3)->setMelody( new SingleNote( -12));
  seq.getTrack( 3)->setProgression( &progression);

  seq.getTrack( 4)->setRythm( new Euclidean( 16, 1, 0)); // arp
  seq.getTrack( 4)->setMelody( new Arp( 8, 0));
  seq.getTrack( 4)->setProgression( &progression);

  seq.getTrack( 5)->setRythm( new Euclidean( 16, 1, 0)); // pad
  seq.getTrack( 5)->setMelody( new Pad( 8, 0));
  seq.getTrack( 5)->setProgression( &progression);

  seq.getTrack( 6)->toggle();
  seq.getTrack( 7)->toggle();

  drawNoButton(); 
}
void showKeyboard() {
  clearRows();
  // white keys
  for( uint8_t i = 0; i < 8; ++i) {
    trellis.setPixelColor( 16+i, 0xFFFFFF);  
  }
  // black keys
  trellis.setPixelColor( 9, 0x888888);
  trellis.setPixelColor( 10, 0x888888); 
  trellis.setPixelColor( 12, 0x888888);
  trellis.setPixelColor( 13, 0x888888);
  trellis.setPixelColor( 14, 0x888888);

  uint8_t root = progression.getRoot();
  switch( root) {
    case 60: trellis.setPixelColor( 16, 0x00FF00); break;
    case 61: trellis.setPixelColor( 9, 0x00FF00); break;
    case 62: trellis.setPixelColor( 17, 0x00FF00); break;
    case 63: trellis.setPixelColor( 10, 0x00FF00); break;
    case 64: trellis.setPixelColor( 18, 0x00FF00); break;
    case 65: trellis.setPixelColor( 19, 0x00FF00); break;
    case 66: trellis.setPixelColor( 12, 0x00FF00); break;
    case 67: trellis.setPixelColor( 20, 0x00FF00); break;
    case 68: trellis.setPixelColor( 13, 0x00FF00); break;
    case 69: trellis.setPixelColor( 21, 0x00FF00); break;
    case 70: trellis.setPixelColor( 14, 0x00FF00); break;
    case 71: trellis.setPixelColor( 22, 0x00FF00); break;
  }
}
void showLength() {
  clearRows();
  // notelength
  for( uint8_t i = 0; i < 6; ++i) {
    trellis.setPixelColor( 8+i, 0x111111);
  }
  uint8_t idx = 0;
  uint8_t noteLength = seq.getTrack( activeTrack)->getNoteLength();
  switch( noteLength) {
    case 1: idx = 0; break;
    case 2: idx = 1; break;
    case 4: idx = 2; break;
    case 8: idx = 3; break;
    case 16: idx = 4; break;
    case 32: idx = 5; break;
  }
  trellis.setPixelColor( 8+idx, 0xFFFFFF);
  
  for( uint8_t i = 0; i < 6; ++i) {
    trellis.setPixelColor( 16+i, 0x111111);
  }
  // Step length
  idx = 0;
  uint8_t stepLength = seq.getTrack( activeTrack)->getStepLength();
  switch( stepLength) {
    case 1: idx = 0; break;
    case 2: idx = 1; break;
    case 4: idx = 2; break;
    case 8: idx = 3; break;
    case 16: idx = 4; break;
    case 32: idx = 5; break;
  }
  trellis.setPixelColor( 16+idx, 0xFFFFFF);

  if( seq.getTrack(activeTrack)->hasLegato()) {
    trellis.setPixelColor( 14, 0xFFFF00);
  } else {
    trellis.setPixelColor( 14, 0xFFFFFF);
  }
  

  //  BPM up/down
  trellis.setPixelColor( 15, 0x00FF00);
  trellis.setPixelColor( 23, 0xFF0000);
}
void showShift() {
  clearRows();
  trellis.setPixelColor( 8, 0x00FF00);
  trellis.setPixelColor( 9, 0xFF0000);

  for( uint8_t i = 0; i < 8; ++i) {
    uint8_t v = i * 16 + 16;
    if( seq.getTrack( activeTrack)->getVelocity() == v) {
      trellis.setPixelColor( 16+i, 0xFFFFFF);
    } else {
      trellis.setPixelColor( 16+i, 0x444444);
    }
  }
}
void showEuclide() {
  Euclidean* rythm = seq.getTrack( activeTrack)->getRythm();
  if( rythm == 0) return;

  clearRows();
  uint8_t offset = rythm->getOffset();
  for( uint8_t i = 0; i < rythm->getLength(); ++i) {
    if( rythm->hasPulse( i)) {
      if( i < offset) {
        trellis.setPixelColor( 8+i, 0x4C0099);
      } else {
        trellis.setPixelColor( 8+i, 0xFF9933);  
      }
    } else {
      if( i < offset) {
        trellis.setPixelColor( 8+i, 0x003300);
      } else {
        trellis.setPixelColor( 8+i, 0xFFFFFF);
      }
    }
  }
}

void buttonPressed( uint8_t i) {
  trellis.setPixelColor( i, activeColor[i]);
  btnDown[i] = true;
  // setup next rows
  if( i == BTN_BPMLENGTH) {
    showLength();
  } else if( i == BTN_ROOTNOTE) {
    showKeyboard();
  } else if( i == BTN_EUCSTEPS) {
    showEuclide();
  } else if( i == BTN_EUCPULSES) {
    showEuclide();
  } else if( i == BTN_EUCOFFSET) {
    showEuclide();
  } else if( i == BTN_SHIFT) {
    showShift();
  }
}
void valuePressed( uint8_t i) {
  if( btnDown[BTN_BPMLENGTH]) {
    if( i < 6) {
      uint8_t v = pow( 2, i);
      seq.getTrack( activeTrack)->setNoteLength( v);
      showLength();
    } else if( i >= 8 && i < 14) {
      uint8_t v = pow( 2, i-8);
      seq.getTrack( activeTrack)->setStepLength( v);
      showLength();
    }
    if( i == 7 && seq.getBpm() < 500) {
      seq.setBpm( seq.getBpm()+20);
    }
    if( i == 15 && seq.getBpm() > 20) {
      seq.setBpm( seq.getBpm()-20);
    }
    if( i == 6) {
      seq.getTrack( activeTrack)->toggleLegato();
      showLength();
    }
  } else if( btnDown[BTN_EUCSTEPS] || btnDown[BTN_EUCPULSES] || btnDown[BTN_EUCOFFSET]) {
    Euclidean* rythm = seq.getTrack( activeTrack)->getRythm();
    if( rythm == 0) return;
    uint8_t steps = rythm->getLength();
    uint8_t pulses = rythm->getPulses();
    uint8_t offset = rythm->getOffset();
    if( btnDown[BTN_EUCSTEPS]) {
      steps = i+1;
    }
    else if( btnDown[BTN_EUCPULSES]) {
      if( i >= steps) {
        return;
      }
      pulses = i+1;
    }
    else if( btnDown[BTN_EUCOFFSET]) {
      if( i >= steps) {
        return;
      }
      offset = i+1;
      if( offset == steps) {
        offset = 0;
      }
    }
    rythm->initialize( steps, pulses, offset);
    showEuclide();
  } else if( btnDown[BTN_ROOTNOTE]) {
    // 60 is C
    uint8_t note = 0;
    switch( i+8) {
      case 16: note = 60; break; // C
      case 9: note = 61; break;
      case 17: note = 62; break; // D
      case 10: note = 63; break;
      case 18: note = 64; break; // E
      case 19: note = 65; break; // F
      case 12: note = 66; break;
      case 20: note = 67; break; // G
      case 13: note = 68; break;
      case 21: note = 69; break; // A
      case 14: note = 70; break;
      case 22: note = 71; break; // H
      case 23: note = 60; break; // C
    }
    if( note > 0) {
      progression.setRoot( note);
      progression.activateProgressionStep( progression.getCurrentStep());
      showKeyboard();
    }
  } else if( btnDown[BTN_SHIFT]) {
    if( i == 0) {
      // increase offset of Tracks Melody by 12
      Melody* melody = seq.getTrack( activeTrack)->getMelody();
      if( melody != 0) {
        melody->setOffset( melody->getOffset() + 12);
      }
    } else if( i == 1) {
      Melody* melody = seq.getTrack( activeTrack)->getMelody();
      if( melody != 0) {
        melody->setOffset( melody->getOffset() - 12);
      }
    }
    if( i >= 8 && i < 16) {
      uint8_t v = (i-7)*6;
      seq.getTrack( activeTrack)->setVelocity( v);
    }
    showShift();
  } else { // no button is shown
    if( i >= 8 && i < 16) {
      //progression.activateProgressionStep( i-8);
      progression.queueProgressonIndex( i-8);
      clearRows();
      drawNoButton();
    }
  }
}
void clearRows() {
  for( uint8_t i = 0; i < 16; ++i) {
    trellis.setPixelColor( 8+i, 0x000000);
  }
}
void drawNoButton() {
  // render progression
  for( uint8_t i = 0; i < 8; ++i) {
    if( progression.getCurrentStep() == i) {
      trellis.setPixelColor( 16+i, 0x00FFFF);
    } else {
      trellis.setPixelColor( 16+i, 0x444444);
    }
  }
}
void buttonReleased( uint8_t idx) {
  trellis.setPixelColor( idx, passiveColor[idx]);
  btnDown[idx] = false;
  // reset next rows
  clearRows();
  drawNoButton();
}
void loop() {
  trellis.tick();
  while( trellis.available()) {
    keypadEvent e = trellis.read();
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      if( e.bit.KEY == BTN_STARTSTOP) { // StartStop
        if( seq.toggle()) {
          trellis.setPixelColor( BTN_STARTSTOP, activeColor[BTN_STARTSTOP]);
        } else {
          trellis.setPixelColor( BTN_STARTSTOP, passiveColor[BTN_STARTSTOP]);
        }
      }
      // Handle Hold-Down-Buttons
      for( uint8_t i = 1; i < BTNCOUNT; ++i) {
        if( e.bit.KEY == i) {
          buttonPressed( i);
        }
      }
      for( uint8_t i = 0; i < 16; ++i) {
        if( e.bit.KEY == 8+i) {
          valuePressed( i);
        }
      }
      
      for( uint8_t i = 0; i < TRACKCOUNT; ++i) {
        if( e.bit.KEY == 24+i) {
          if( btnDown[BTN_SHIFT]) {
            if(  seq.getTrack( i)->toggle()) {
              trellis.setPixelColor( 24+i, i == activeTrack ? 0x00FF00 : 0x001100);
            } else {
              trellis.setPixelColor( 24+i, i == activeTrack ? 0xFF0000 : 0x110000);
            }
          } else {
            // dim down last activeTrack
            if( i != activeTrack) {
              trellis.setPixelColor( 24+activeTrack, seq.getTrack( activeTrack)->isActive() ? 0x001100 : 0x110000);
              trellis.setPixelColor( 24+i, seq.getTrack( i)->isActive() ? 0x00FF00 : 0xFF0000);
              activeTrack = i;
            }
          }
        }
      }
    } 
    else if( e.bit.EVENT == KEY_JUST_RELEASED) {
      for( uint8_t i = 1; i < BTNCOUNT; ++i) {
        if( e.bit.KEY == i) {
          buttonReleased( i);
        }
      }
    }
  }
  bool doit = true;
  for( uint8_t i = 0; i < BTNCOUNT; ++i) {
    if( btnDown[i]) {
      doit = false;
    }
  }
  if( doit) {
    drawNoButton();
  }
  delay( 10);
}

bool onoff = false;

void isr() {
  seq.tick();
}
