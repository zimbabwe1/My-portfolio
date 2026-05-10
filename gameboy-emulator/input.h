#ifndef INPUT_H
#define INPUT_H

#include <cstdint>

class Input {
public:
    Input();
    void update();
    bool isPressed(uint8_t key);

private:
    const uint8_t* keystate;
};

#endif