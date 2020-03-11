#ifndef _UI_H
#define _UI_H

#include <Adafruit_NeoTrellisM4.h>

#define BRIGHTNESS 50

class UIScreen;

class UIEvent {
  UIScreen* _nextScreen;
public:
  UIEvent();
  UIEvent( UIScreen* nextScreen);
  UIScreen* getNextScreen();
};

class UIScreen {
  Adafruit_NeoTrellisM4* _trellis;
public:
  UIScreen( Adafruit_NeoTrellisM4* trellis) {
    _trellis = trellis;
  }
  virtual void render() = 0;
  virtual UIEvent* buttonPressed( uint8_t idx) = 0;
  virtual UIEvent* buttonReleased( uint8_t idx) = 0;
};

class UI {
  Adafruit_NeoTrellisM4* _trellis;
  UIScreen* _currentScreen;
public:
  UI();
  ~UI();
  void begin();
  void tick();
};

#endif
