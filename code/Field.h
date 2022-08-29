#pragma once
#include "Utils.h"
#include <vector>
#include <random>
#include <chrono>
#include <deque>
#include <queue>

class Tile {
private:
    sf::RectangleShape bg_tile;
    sf::RectangleShape tile;
    int number;

    sf::Text text;

    bool closed;
    bool mine;
    bool marked;

public:
    static sf::Color CLOSED;
    static sf::Color VOID;
    static sf::Color MINE;
    static sf::Color BG;
    static sf::Color MARK;
    static sf::Color DEFUSE;
    static sf::Color HINT;

    explicit Tile(int i, int j) noexcept;
    void place_mine();
    void open();
    void defuse();
    void mark();
    void hint();
    void reset();
    sf::FloatRect rect() const;
    bool is_mine() const;
    bool is_open() const;
    bool is_closed() const;
    bool is_marked() const;
    void set_number(int num);
    int get_number() const;

    void show(sf::RenderWindow& window) const;

    bool visited;
};

class Field {
private:
    std::vector<std::pair<sf::Vector2i, std::vector<sf::Vector2i>>> graph;
    std::vector<std::vector<std::shared_ptr<Tile>>> field;
    std::vector<sf::Vector2i> mines;
    sf::RectangleShape bg;

    int opening_mine;
    int open_mines_timer;
    int defuse_mines_timer;

    int count_open_tiles;

public:
    Field();

    void spawn_mines(int i, int j);
    void open_tiles(int i, int j);
    bool open_mines(int time);
    bool defuse_mines(int time);
    void reset();

    bool check_win();
    void show(sf::RenderWindow& window);

    std::vector<std::shared_ptr<Tile>> operator[] (int index);

    bool check_for_hint(std::vector<sf::Vector2i>& hint_tiles); // checks if there is need for a hint
};
