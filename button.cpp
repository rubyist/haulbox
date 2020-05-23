#include "button.h"

Button::Button(int id) {
    _id = id;
    _v = 0;
}

int Button::state() {
    return _v;
}

int Button::id() {
    return _id;
}

void Button::activate(int value, Outputter output) {
}

Toggle::Toggle(int id) : Button(id) {}

void Toggle::activate(int value, Outputter output) {
  output.pressButton(_id);
  output.releaseButton(_id);
}

PushButton::PushButton(int id) : Button(id) {}

void PushButton::activate(int value, Outputter output) {
  output.setButton(_id, value);
  _v = value;
}

Latch::Latch(int id) : Button(id) {}

void Latch::activate(int value, Outputter output) {
    if (value == 1)
    {
        output.setButton(_id, !_v);
        _v = !_v;
    }
}