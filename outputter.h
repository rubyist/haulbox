#ifndef OUTPUTTER_H
#define OUTPUTTER_H

#include <Joystick.h>

#define OUTPUT_SERIAL 0
#define OUTPUT_JOYSTICK 1

class Outputter {
    public:
    Outputter(int outputType, int numButtons);
    void begin();
    void pressButton(int button);
    void releaseButton(int button);
    void setButton(int button, int state);
    void toggle();

    private:
    int _type;
    Joystick_ *_joystick;
    bool _enabled;
};

#endif