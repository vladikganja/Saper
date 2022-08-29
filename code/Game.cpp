#include "Game.h"

Game::Game() {
    first_click = true;
    lose = false;
    win = false;
    gameover = false;

    gameover_bg.setFillColor(Tile::BG);
    gameover_bg.setPosition({ 0.f, 0.f });
    gameover_bg.setSize({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});

    gameover_text.setFont(Utils::font);
    gameover_text.setCharacterSize(100);
    gameover_text.setPosition({ 580.f, 200.f });

    restart_text.setFont(Utils::font);
    restart_text.setCharacterSize(40);
    restart_text.setFillColor(Tile::VOID);
    restart_text.setPosition({ 560.f, 400.f });
    restart_text.setString("Press any key to restart");
}

void Game::Run(sf::RenderWindow& window) {
    sf::Event event;
    window.pollEvent(event);
    event.type = sf::Event::GainedFocus;
    
    sf::Clock clock;
    while (window.isOpen()) {
        window.pollEvent(event);

        auto time = clock.getElapsedTime().asMilliseconds();
        clock.restart();

        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (!lose && !win && event.type == sf::Event::MouseButtonPressed) {
            check_click(sf::Mouse::getPosition(window));
            if (field.check_win()) {
                win = true;
                gameover_text.setString("You win!");
                gameover_text.setFillColor(Tile::DEFUSE);
            }
            if (lose) {
                gameover_text.setString("You lose!");
                gameover_text.setFillColor(Tile::MINE);
            }
            
            event.type = sf::Event::GainedFocus;
        }

        if (gameover && (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)) {
            reset();
            event.type = sf::Event::GainedFocus;
            continue;
        }

        if (!gameover && lose) {
            gameover = field.open_mines(time);
        }

        if (!gameover && win) {
            gameover = field.defuse_mines(time);
        }

        render(window);
    }
}

void Game::reset() {
    field.reset();
    first_click = true; // to avoid click by chance within new game
    lose = false;
    win = false;
    gameover = false;
}

void Game::check_click(const sf::Vector2i& _mouse_pos) {
    sf::Vector2f mouse_pos = { static_cast<float>(_mouse_pos.x), static_cast<float>(_mouse_pos.y) };
    for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
        for (int j = 1; j < FIELD_WIDTH + 1; j++) {
            if (field[i][j]->rect().contains(mouse_pos)) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (field[i][j]->is_closed()) {
                        if (first_click) {
                            first_click = false;
                            field.spawn_mines(i, j);
                        }
                        if (!field[i][j]->is_mine()) {
                            field.open_tiles(i, j);
                        }
                        else {
                            field[i][j]->open();
                            lose = true;
                        }
                        goto ret;
                    }
                }
                else if (!first_click && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    field[i][j]->mark();
                }
            }
        }
    }
ret:
    std::vector<sf::Vector2i> hint_tiles;
    if (!first_click && field.check_for_hint(hint_tiles)) {
        if (hint_tiles.size() > 0) {
            sf::Vector2i hint_tile = hint_tiles[rand() % hint_tiles.size()];
            field[hint_tile.y][hint_tile.x]->hint();
        }
    }
    return;
}

void Game::render(sf::RenderWindow& window) {
    if (!gameover) {
        field.show(window);
    }
    else {
        window.draw(gameover_bg);
        window.draw(gameover_text);
        window.draw(restart_text);
    }
    window.display();
}
