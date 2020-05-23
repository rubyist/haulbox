#ifndef BUTTON_H
#define BUTTON_H

#include "outputter.h"

class Button {
    public:
    Button(int id);
    int state();
    int id();
    virtual void activate(int value, Outputter output);

    protected:
    int _id;
    int _v;
};

class Toggle: public Button {
    public:
    Toggle(int id);
    void activate(int value, Outputter output);
};

class PushButton: public Button {
    public:
    PushButton(int id);
    void activate(int value, Outputter output);
};

class Latch: public Button {
    public:
    Latch(int id);
    void activate(int value, Outputter output);
};

#endif