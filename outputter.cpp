#include "Arduino.h"

#include "outputter.h"


Outputter::Outputter(int outputType, int numButtons) {
    _type = outputType;
    _enabled = true;
    if (outputType == OUTPUT_JOYSTICK) {
        _joystick = new Joystick_(0x09, JOYSTICK_TYPE_JOYSTICK, numButtons, 0, false, false, false, false, false, false, false, false, false, false, false);
    }
}

void Outputter::begin() {
    if (_type == OUTPUT_JOYSTICK) {
        _joystick->begin();
    } else {
        Serial.begin (115200);
    }
}

void Outputter::pressButton(int button) {
    if (!_enabled)
        return;

    if (_type == OUTPUT_JOYSTICK) {
        _joystick->pressButton(button);
    } else {
        Serial.print("Button ");
        Serial.print(button);
        Serial.println(" PRESS");
    }
}

void Outputter::releaseButton(int button) {
    if (!_enabled)
        return;

    if (_type == OUTPUT_JOYSTICK) {
        _joystick->releaseButton(button);
    } else {
        Serial.print("Button ");
        Serial.print(button);
        Serial.println(" RELEASE");
    }
}

void Outputter::setButton(int button, int state) {
    if (!_enabled)
        return;

     if (_type == OUTPUT_JOYSTICK) {
        _joystick->setButton(button, state);
    } else {
        Serial.print("Button ");
        Serial.print(button);
        Serial.print(" STATE ");
        Serial.println(state);
    }
}

void Outputter::toggle() {
    _enabled = !_enabled;
}