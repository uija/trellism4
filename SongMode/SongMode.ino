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
 
/**
 * This is the main arduino file for the song mode sequencer
 * As this handles all the ui and buttons, its a way to big mess
 * I would like it to have some kind of state-machine, to get rid of all the bool
 * But it grew from a small prototype with "Oh, lets try this feature too"...
 * Maybe I find time and motivation to rewrite the gui stuff, 
 * think about a better way of handling buttons etc. Have something in mind
 * 
 * Jens Kirchheimer
 * jkirchheimer@gmail.com
 * 
 * March 2020
 */

#include <Adafruit_NeoTrellisM4.h>
#include "Adafruit_SPIFlash.h"

#include "Interrupt.h"
#include "sequencer.h"
#include "storage.h"

#define DEFAULT_BPM 100
#define MIDI_CHANNEL 9

#define PATTERNBEATLENGTH 192
#define PATTERNLENGTHFACTOR 24
#define DEFAULTPATTERNLENGTH 8

#if defined(EXTERNAL_FLASH_USE_QSPI)
  Adafruit_FlashTransport_QSPI flashTransport;
#elif defined(EXTERNAL_FLASH_USE_SPI)
  Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);
#else
  #error No QSPI/SPI flash are defined on your board variant.h !
#endif
Adafruit_SPIFlash flash(&flashTransport);

uint32_t pageColor[8] = {
  0x0080FF,
  0x00FF00,
  0xFFFF00,
  0xFF8000,
  0x0000FF,
  0x7F00FF,
  0xFF00FF,
  0xFF007F
};

Storage storage( &flash);

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();
Sequencer seq( MIDI_CHANNEL, DEFAULT_BPM);

bool isInLength = false;
uint8_t lastStepLength = DEFAULTPATTERNLENGTH;
int8_t currentActiveSequenceIndex = -1;
bool needRenderUpdate = false;

int16_t projectId = -1;

bool isLoad = false;
bool isSave = false;
bool isConfirm = false;
bool isRemove = false;
bool isInsert = false;
bool isSettings = false;


uint8_t currentPage = 0;
uint8_t currentBank = 0;
uint8_t projectPage = 0;

uint8_t saveId = 0;

uint8_t projects[128];

void setup() {
  Serial.begin(115200);
  // init projects
  for( uint8_t i = 0; i < 128; ++i) {
    projects[i] = 0;
  }
  
  trellis.begin();
  trellis.setBrightness( 100);

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
              trellis.setPixelColor( r*8+x, 0x550055);
              break;
            case 1:
            case -1:
              trellis.setPixelColor( r*8+x, 0xCC00CC);
              break;
            default:
              trellis.setPixelColor( r*8+x, 0xFF00FF);
          }
        }
      }
    }
    delay( 100);
  }
  

  storage.init();
 
  trellis.setPixelColor( 0, 0x00FF00); // Playbutton

  renderSequencer();
  
  TC.startTimer(10, isr);
}

void renderProjects() {
  uint32_t color = isLoad ? 0x4444FF : 0x44FF44;
  
  trellis.setPixelColor( 7, 0xFF0000);
  
  storage.read( 8, projects, 128);

  // render pages
  for( uint8_t i = 0; i < 8; ++i) {
    if( projectPage == i) {
      trellis.setPixelColor( 8+i, color);
    } else {
      trellis.setPixelColor( 8+i, 0x555555);
    }
  }
  uint8_t offset = projectPage * 16;
  for( uint8_t i = 0; i < 16; ++i) {
    if( projects[offset+i] < 128) {
      if( offset+i == projectId) {
        trellis.setPixelColor( 16+i, color);
      } else {
        trellis.setPixelColor( 16+i, 0xFFFFFF);
      }
    } else {
      trellis.setPixelColor( 16+i, 0x000000);
    }
  }
  // render projects
}
void renderLoad() {
  isLoad = true;
  isSave = false;
  isConfirm = false;
  renderProjects();
}
void renderSave() {
  isSave = true;
  isLoad = false;
  isConfirm = false;
  renderProjects();
}
void handleLoad( uint8_t slot) {
  // check if slot is able to load
  if( projects[slot] > 1) {
    return;
  }
  projectId = slot;
  isLoad = false;
  storage.loadProject( slot, &seq);
  renderSequencer();
}
void handleSave( uint8_t slot) {
  saveId = slot;
  if( projectId >= 0 && projects[slot] == 1 && slot != projectId) { // there is a project on that slot that is not this one
    renderSaveConfirm();
    return;
  }
  projectId = slot;
  projects[slot] = 1;
  storage.saveProject( slot, &seq);
  isSave = false;
  renderSequencer();
}


void renderStepLength() {
  for( uint8_t i = 0; i < 24; ++i) {
    if( seq.lengthAt(currentActiveSequenceIndex) == i+1) {
      trellis.setPixelColor( 8+i, 0xFF8000);
    } else {
      trellis.setPixelColor( 8+i, 0x555555);
    }
  }
}
void renderSaveConfirm() {
  for( uint8_t i = 1; i < 32; ++i) {
    trellis.setPixelColor( i, 0x00000);
  }
  trellis.setPixelColor( 4, 0x44FF44);
  trellis.setPixelColor( 16, 0x00FF00);
  trellis.setPixelColor( 23, 0xFF0000);
  isConfirm = true;
}
void renderStep() {
  // active back button
  trellis.setPixelColor( 1, 0x000000);
  trellis.setPixelColor( 3, 0x000000);
  trellis.setPixelColor( 4, 0x000000);
  trellis.setPixelColor( 5, 0xFFFFFF);
  trellis.setPixelColor( 6, 0xFF8000);
  trellis.setPixelColor( 7, 0xFF0000);

  // bank buttons
  for( uint8_t i = 0; i < 8; ++i) {
    if( currentBank == i) {
      trellis.setPixelColor( 8+i, 0xFF3333);
    } else {
      trellis.setPixelColor( 8+i, 0x330000);
    }
  }
  
  for( uint8_t i = 0; i < 16; ++i) {
    if( seq.hasPatternAt( currentActiveSequenceIndex) && seq.patternAt( currentActiveSequenceIndex) == i+currentBank*16) {
      trellis.setPixelColor( 16+i, 0xFFFFFF);
    } else {
      trellis.setPixelColor( 16+i, 0x0000FF);
    }
  }
}
void renderSequencer() {
  // active back button
  trellis.setPixelColor( 1, 0xFFFF00);
  trellis.setPixelColor( 2, 0xFF00FF); // Settings
  trellis.setPixelColor( 3, 0x4444FF); // Load
  trellis.setPixelColor( 4, 0x44FF44); // Save
  trellis.setPixelColor( 5, 0xFF4444); // Clear
  trellis.setPixelColor( 6, 0x00FF00); // Insert
  trellis.setPixelColor( 7, 0xFF0000); // Delete
  // page buttons
  for( uint8_t i = 0; i < 8; ++i) {
    // only render page buttons
    // if the page before is filled
    if( i == 0 || seq.hasPatternAt( (i-1)*16+15)) {
      if( currentPage == i) {
        trellis.setPixelColor( 8+i, 0x3333FF);
      } else {
        trellis.setPixelColor( 8+i, 0x000033);
      }
    } else {
      trellis.setPixelColor( 8+i, 0x000000);
    }
  }
  
  for( uint8_t i = 0; i < 16; ++i) {
    if( seq.hasPatternAt( i + currentPage*16)) {
      trellis.setPixelColor( 16+i, 0x0000FF);
    } else {
      trellis.setPixelColor( 16+i, 0x555555);
    }
  }
}
void insertAt( uint8_t idx) {
  for( uint8_t i = SEQUENCELENGTH-2; i > idx; --i) {
    seq.setPatternAt( i+1, seq.patternAt( i));
    seq.setLengthAt( i+1, seq.lengthAt( i), seq.factorAt( i));
  }
  seq.resetPatternAt( idx+1);
}
void removeAt( uint8_t idx) {
  for( uint8_t i = idx; i < SEQUENCELENGTH-1; ++i) {
    seq.setPatternAt( i, seq.patternAt( i+1));
    seq.setLengthAt( i, seq.lengthAt( i+1), seq.factorAt( i+1));
  }
  seq.resetPatternAt( SEQUENCELENGTH-1);
}
void renderSettings() {
  isSettings = true;
  for( uint8_t i = 1; i < 32; ++i) {
    trellis.setPixelColor( i, 0x000000);
  }
  trellis.setPixelColor( 7, 0xFF0000); // Cancel

  trellis.setPixelColor( 16, 0x88FF88); // BPM Up1
  trellis.setPixelColor( 24, 0xFF8888); // BPM Down1
  trellis.setPixelColor( 17, 0x44FF44); // BPM Up5
  trellis.setPixelColor( 25, 0xFF4444); // BPM Down5
  trellis.setPixelColor( 18, 0x00FF00); // BPM Up10
  trellis.setPixelColor( 26, 0xFF0000); // BPM Down10
  
}
void renderPlayScreen() {
  if( !seq.isRunning() || !needRenderUpdate) {
    return;
  }
  needRenderUpdate = false;
  // remove top buttons
  for( uint8_t i = 1; i < 8; ++i) {
    trellis.setPixelColor( i, 0x000000);
  }
  uint8_t currentIndex = seq.getCurrentIndex();
  uint8_t currentPageIndex = currentIndex;
  uint8_t currentPage = 0;
  while( currentPageIndex >= 16) {
    currentPage++;
    currentPageIndex -= 16;
  }
  // check pages
  for( uint8_t p = 0; p < 8; ++p) {
    // does page has sequence?
    if( seq.hasPatternAt( p*16)) {
      if( p == currentPage) {
        trellis.setPixelColor( 8+p, 0xFF00FF);
      } else {
        trellis.setPixelColor( 8+p, 0xFFCCFF); 
      }
    } else {
      trellis.setPixelColor( 8+p, 0x000000);
    }
  }
  for( uint8_t i = 0; i < 16; ++i) {
    if( seq.hasPatternAt( currentPage*16+i)) {
      if( i == currentPageIndex) {
        trellis.setPixelColor( 16+i, 0x00FF00);
      } else {
        trellis.setPixelColor( 16+i, 0xFFFFFF);
      }
    } else {
      trellis.setPixelColor( 16+i, 0x000000);
    }
  }
}

void handleStepPressed( uint8_t step) {
  if( currentActiveSequenceIndex < 0) { // we select a step in the sequencer
    if( isSave || isLoad) {
      if( step < 8) {
        projectPage = step;
        renderProjects();
      } else {
        if( isSave) {
          handleSave( projectPage*16+step-8);
        } else {
          handleLoad( projectPage*16+step-8);
        }
      }
    } else {
      if( step < 8) {
        if( step == 0 || seq.hasPatternAt( (step-1)*16+15)) {
          currentPage = step;
          renderSequencer();
        }
      } else {
        if( isInsert) {
          insertAt( currentPage*16+step-8);
          renderSequencer();
        } else if( isRemove) {
          removeAt( currentPage*16+step-8);
          renderSequencer();
        } else {
          currentActiveSequenceIndex = currentPage*16+step-8;
          renderStep();
        }
        
      }
    }
  } else {
    if( isInLength) {
      lastStepLength = (step+1);
      seq.setLengthAt( currentActiveSequenceIndex, lastStepLength, PATTERNLENGTHFACTOR);
      renderStepLength();
    } else {
      if( step < 8) {
        currentBank = step;
        renderStep();
      } else {
        seq.setPatternAt( currentActiveSequenceIndex, currentBank*16+step-8);
        if( seq.lengthAt( currentActiveSequenceIndex) == 0) {
          seq.setLengthAt( currentActiveSequenceIndex, lastStepLength, PATTERNLENGTHFACTOR);
        }
        currentActiveSequenceIndex = -1;
        renderSequencer();
      }
    }
  }
}

void loop() {
  trellis.tick();
  while( trellis.available()) {
    keypadEvent e = trellis.read();
    if( e.bit.EVENT == KEY_JUST_PRESSED) {
      if( e.bit.KEY == 0) {
        if( seq.isRunning()) stop();
        else {
          // check if there is at least one pattern
          if( seq.hasPatternAt( 0)) {
            start();
          }
        }
      } else if( !seq.isRunning() && isSave && isConfirm) {
        if( e.bit.KEY == 16) { // OK
          // confirming the save by setting projectId to match saveId
          projectId = saveId;
          handleSave( saveId);
        } else {
          isSave = false;
          isConfirm = false;
          renderSave();
        }
        return;
      } else if( !seq.isRunning() && isSettings) {
        uint16_t bpm = seq.getBpm();
        if( e.bit.KEY == 7) { // Cancel
          isSettings = false;
          renderSequencer();
        } else if( e.bit.KEY == 16 && bpm < 512) {
          seq.setBpm( bpm+1);
        } else if( e.bit.KEY == 24 && bpm > 1) {
          seq.setBpm( bpm-1);
        } else if( e.bit.KEY == 17 && bpm < 507) {
          seq.setBpm( bpm+5);
        } else if( e.bit.KEY == 25 && bpm > 5) {
          seq.setBpm( bpm-5);
        } else if( e.bit.KEY == 18 && bpm < 502) {
          seq.setBpm( bpm+10);
        } else if( e.bit.KEY == 26 && bpm > 10) {
          seq.setBpm( bpm-10);
        }
      } else if( !seq.isRunning() && e.bit.KEY == 1 && currentActiveSequenceIndex < 0) { // Play / Stop
        // Send Programm Change to first pattern in sequence
        if( seq.hasPatternAt( 0)) {
          seq.sendMidiProgramChange( seq.patternAt( 0));  
        }
      } else if( !seq.isRunning() && e.bit.KEY == 2 && currentActiveSequenceIndex < 0) { // Settings
        renderSettings();
      } else if( !seq.isRunning() && e.bit.KEY == 3 && currentActiveSequenceIndex < 0) { // Load
        renderLoad();
      } else if( !seq.isRunning() && e.bit.KEY == 4 && currentActiveSequenceIndex < 0) { // Save
        renderSave();
      } else if( !seq.isRunning() && e.bit.KEY == 5 && currentActiveSequenceIndex < 0) { // Clear Sequence
        // clear sequence
        seq.clear();
        projectId = -1;
        saveId = 0;
        isSave = false;
        isLoad = false;
        isConfirm = false;
        renderSequencer();
        currentActiveSequenceIndex = -1;
      } else if( !seq.isRunning() && e.bit.KEY == 6 && currentActiveSequenceIndex < 0) { // Insert
        isInsert = true;
        isRemove = false;
      } else if( !seq.isRunning() && e.bit.KEY == 7 && currentActiveSequenceIndex < 0) { // Remove
        isInsert = false;
        isRemove = true;
      } else if( !seq.isRunning() && e.bit.KEY == 7 && ( currentActiveSequenceIndex >= 0 || isLoad || isSave)) { // Cancel
        isSave = false;
        isLoad = false;
        isConfirm = false;
        renderSequencer();
        currentActiveSequenceIndex = -1;
      } else if( !seq.isRunning() && e.bit.KEY == 6 && currentActiveSequenceIndex >= 0) {
        isInLength = true;
        renderStepLength();
      } else if( !seq.isRunning() && e.bit.KEY == 5 && currentActiveSequenceIndex >= 0) {
        seq.resetPatternAt( currentActiveSequenceIndex);
        renderSequencer();
        currentActiveSequenceIndex = -1;
      } else if( !seq.isRunning() && e.bit.KEY >= 8) {
        handleStepPressed( e.bit.KEY-8);
      }
    } else if( e.bit.EVENT == KEY_JUST_RELEASED) {
      if( !seq.isRunning() && e.bit.KEY == 6 && currentActiveSequenceIndex >= 0) {
        isInLength = false;
        renderStep();
      } else if( !seq.isRunning() && e.bit.KEY == 6 && currentActiveSequenceIndex < 0) {
        isInsert = false;
        isRemove = false;
      } else if( !seq.isRunning() && e.bit.KEY == 7 && currentActiveSequenceIndex < 0) {
        isInsert = false;
        isRemove = false;
      }
    }
  }
  if( seq.isRunning() && needRenderUpdate) {
    renderPlayScreen();
  }
  delay( 10);
}
void isr() {
  if( seq.tick()) { // don't override true with false before rendering was done
    needRenderUpdate = true;
  }
}
void start() {
  isLoad = false;
  isSave = false;
  isConfirm = false;
  isRemove = false;
  isInsert = false;
  isSettings = false;
  currentActiveSequenceIndex = -1;
  seq.start();
  trellis.setPixelColor( 0, 0xFF0000);
}
void stop() {
  currentActiveSequenceIndex = -1;
  seq.stop();
  trellis.setPixelColor( 0, 0x00FF00);
  renderSequencer();
}
