#include <iostream>
#include "screen.h"

void createScreen() {
    Screen screen(NULL);
    screen.start();
}

int main() {
    createScreen();
    Screen::terminate();
    return 0;
}