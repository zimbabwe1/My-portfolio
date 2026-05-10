#include "./input.h"
#include <SDL3/SDL.h>

Input::Input() : keystate(nullptr) {}

void Input::update() {
    keystate = SDL_GetKeyboardState(NULL);
}
#keys to press: 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT, 4=A, 5=B, 6=START, 7=SELECT
bool Input::isPressed(uint8_t key) {
    if (!keystate) return false;
    switch (key) {
        case 0: return keystate[SDL_SCANCODE_UP];
        case 1: return keystate[SDL_SCANCODE_DOWN];
        case 2: return keystate[SDL_SCANCODE_LEFT];
        case 3: return keystate[SDL_SCANCODE_RIGHT];
        case 4: return keystate[SDL_SCANCODE_Z]; // A
        case 5: return keystate[SDL_SCANCODE_X]; // B
        case 6: return keystate[SDL_SCANCODE_RETURN]; // Start
        case 7: return keystate[SDL_SCANCODE_SPACE]; // Select
        default: return false;
    }
}