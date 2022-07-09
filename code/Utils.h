#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>

static constexpr int WINDOW_WIDTH = 1504;
static constexpr int WINDOW_HEIGHT = 804;

static constexpr int FIELD_WIDTH = 30;
static constexpr int FIELD_HEIGHT = 16;

static constexpr int MINES_COUNT = 100;

class Utils {
public:
    static inline sf::Font font;
    static void init();
};
