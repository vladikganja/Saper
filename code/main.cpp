#pragma once
#include "SFML/Graphics.hpp"
#include "Game.h"

int main() {

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Saper", sf::Style::Close);

    Utils::init();

    Game game;

    game.Run(window);

    return 0;
}
