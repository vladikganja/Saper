#pragma once
#include "SFML/Graphics.hpp"
#include "Utils.h"
#include "Field.h"

class Game {
private:

    sf::RectangleShape gameover_bg;
    sf::Text gameover_text;
    sf::Text restart_text;

    Field field;
    bool first_click;
    bool lose;
    bool win;

    bool gameover;

    void render(sf::RenderWindow& window);
    void reset();

    void check_click(const sf::Vector2i& mouse_pos);

public:
    Game();

    void Run(sf::RenderWindow& window);
};
