#include "ui.h"

UI::UI() {
  _trellis = new Adafruit_NeoTrellisM4();
  _currentScreen = 0;
}
UI::~UI() {
  delete _trellis;
  if( _currentScreen != 0) {
    delete _currentScreen;
  }
}
void UI::begin() {
  _trellis->begin();
  _trellis->setBrightness( BRIGHTNESS);
}
void UI::tick() {
  _trellis->tick();
  while( _trellis->available()) {
    keypadEvent e = _trellis->read();
    UIEvent* event = 0;
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      if( _currentScreen != 0) {
        UIEvent* event = _currentScreen->buttonPressed( e.bit.KEY);
      }
    } else if( e.bit.EVENT == KEY_JUST_RELEASED) {
      if( _currentScreen != 0) {
        UIEvent* event = _currentScreen->buttonReleased( e.bit.KEY);
      }
    }
    if( event != 0) {
      UIScreen* nextScreen = event->getNextScreen();
      if( nextScreen != 0) {
        delete _currentScreen;
        _currentScreen = nextScreen;
        _currentScreen->render();
      }
      delete event;
    }
  }
}


UIEvent::UIEvent() {
  _nextScreen = 0;
}
UIEvent::UIEvent( UIScreen* nextScreen) {
  _nextScreen = nextScreen;
}
UIScreen* UIEvent::getNextScreen() {
  return _nextScreen;
}
