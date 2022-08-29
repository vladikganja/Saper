#include "Field.h"

sf::Color Tile::CLOSED = { 160, 160, 160 };
sf::Color Tile::VOID = { 120, 120, 120 };
sf::Color Tile::MINE = { 255, 60, 47 };
sf::Color Tile::BG = { 220, 220, 220 };
sf::Color Tile::MARK = { 255, 110, 0 };
sf::Color Tile::DEFUSE = { 0, 204, 51 };
sf::Color Tile::HINT = { 129, 237, 233 };

Tile::Tile(int i, int j) noexcept {
    i--;
    j--;
    number = 0;
    text.setFont(Utils::font);
    text.setCharacterSize(24);
    text.setString(sf::String());
    text.setFillColor({ 0, 0, 0 });

    bg_tile.setSize({ static_cast<float>(WINDOW_WIDTH - 4) / FIELD_WIDTH,
                        static_cast<float>(WINDOW_HEIGHT - 4) / FIELD_HEIGHT });
    bg_tile.setPosition({ j * bg_tile.getSize().x + 2.f, i * bg_tile.getSize().y + 2.f });
    bg_tile.setFillColor(BG);

    tile.setSize({ bg_tile.getSize().x - 4.f, bg_tile.getSize().y - 4.f });
    tile.setPosition({ j * bg_tile.getSize().x + 4.f, i * bg_tile.getSize().y + 4.f });
    tile.setFillColor(CLOSED);

    text.setPosition({ tile.getPosition().x + 16.f, tile.getPosition().y + 8.f });

    closed = true;
    mine = false;
    marked = false;
    visited = false;
}

void Tile::place_mine() {
    mine = true;
}

void Tile::open() {
    closed = false;
    if (mine) {
        tile.setFillColor(MINE);
    }
    else {
        tile.setFillColor(VOID);
    }
}

void Tile::defuse() {
    tile.setFillColor(DEFUSE);
}

void Tile::mark() {
    if (closed) {
        marked = !marked;
        if (marked) {
            tile.setFillColor(MARK);
        }
        else {
            tile.setFillColor(CLOSED);
        }
    }
}

void Tile::hint() {
    tile.setFillColor(HINT);
}

void Tile::reset() {
    closed = true;
    mine = false;
    tile.setFillColor(CLOSED);
    text.setString(sf::String());
}

sf::FloatRect Tile::rect() const {
    return bg_tile.getGlobalBounds();
}

bool Tile::is_mine() const {
    return mine;
}

bool Tile::is_open() const {
    return !closed;
}

bool Tile::is_closed() const {
    return closed;
}

bool Tile::is_marked() const {
    return marked;
}

void Tile::set_number(int num) {
    number = num;
    text.setString(std::to_string(num));
}

int Tile::get_number() const {
    return number;
}

void Tile::show(sf::RenderWindow& window) const {
    window.draw(bg_tile);
    window.draw(tile);
    if (!closed && number != 0) {
        window.draw(text);
    }
}

// FIELD //////////////////////////////////////////////////////////////////////////////////////////

Field::Field() {
    opening_mine = 0;
    open_mines_timer = 0;
    defuse_mines_timer = 0;
    count_open_tiles = 0;

    bg.setFillColor(Tile::BG);
    bg.setPosition({ 0.f, 0.f });
    bg.setSize({ static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT) });

    // field filling
    field.resize(FIELD_HEIGHT + 2);
    for (int i = 0; i < FIELD_HEIGHT + 2; i++) {
        field[i].resize(FIELD_WIDTH + 2);
        for (int j = 0; j < FIELD_WIDTH + 2; j++) {
            field[i][j] = std::shared_ptr<Tile>(new Tile(i, j));
        }
    }

    // graph construction
    for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
        for (int j = 1; j < FIELD_WIDTH + 1; j++) {
            graph.push_back({ {j, i}, {} });
            for (int y = std::max(1, i - 1); y <= std::min(FIELD_HEIGHT, i + 1); y++) {
                for (int x = std::max(1, j - 1); x <= std::min(FIELD_WIDTH, j + 1); x++) {
                    if (y != i || x != j) {
                        graph[graph.size() - 1].second.push_back({x, y});
                    }

                }
            }
        }
    }
}

void Field::spawn_mines(int _i, int _j) {
    std::mt19937 gen;
    auto now = std::chrono::high_resolution_clock::now();
    gen.seed(static_cast<unsigned>(now.time_since_epoch().count()));

    // forming the vector of all tiles
    std::deque<sf::Vector2i> tiles;
    for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
        for (int j = 1; j < FIELD_WIDTH + 1; j++) {
            if ((i >= _i - 1 && i <= _i + 1) && (j >= _j - 1 && j <= _j + 1)) {
                continue;
            }
            tiles.push_back({j, i});
        }
    }

    // spawning mines
    for (int m = 0; m < MINES_COUNT; m++) {
        int index = gen() % tiles.size();
        field[tiles[index].y][tiles[index].x]->place_mine();
        mines.push_back({ tiles[index].x, tiles[index].y });
        tiles.erase(tiles.begin() + index);
    }

    // filling the field with correct numbers
    for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
        for (int j = 1; j < FIELD_WIDTH + 1; j++) {
            if (!field[i][j]->is_mine()) {
                int num = 0;
                for (int y = i - 1; y <= i + 1; y++) {
                    for (int x = j - 1; x <= j + 1; x++) {
                        if (field[y][x]->is_mine()) {
                            num++;
                        }
                    }
                }
                field[i][j]->set_number(num);
            }
        }
    }
}

void Field::open_tiles(int _i, int _j) {

    int opened = 0;

    if (field[_i][_j]->get_number() == 0) {

        field[_i][_j]->open();
        field[_i][_j]->visited = true;
        opened++;

        // bfs search
        std::queue<sf::Vector2i> qu;
        qu.push({_j, _i});
        while (!qu.empty()) {
            int pos = (qu.front().y - 1) * FIELD_WIDTH + qu.front().x - 1;
            for (auto& el : graph[pos].second) {
                int neighbour = (el.y - 1) * FIELD_WIDTH + el.x - 1;
                auto& tile = field[graph[neighbour].first.y][graph[neighbour].first.x];
                if (!tile->visited && !tile->is_mine()) {
                    bool firstly_open = tile->is_closed();
                    bool inside_of_visible_field =
                        (graph[neighbour].first.y >= 1 && graph[neighbour].first.y <= FIELD_HEIGHT + 1) &&
                        (graph[neighbour].first.x >= 1 && graph[neighbour].first.x <= FIELD_WIDTH + 1);

                    tile->visited = true;
                    tile->open();

                    if (inside_of_visible_field && firstly_open)
                        opened++;
                    if (tile->get_number() == 0)
                        qu.push(el);
                }
            }
            qu.pop();
        }

        // reset visited tiles after bfs
        for (int i = 0; i < FIELD_HEIGHT + 2; i++) {
            for (int j = 0; j < FIELD_WIDTH + 2; j++) {
                field[i][j]->visited = false;
            }
        }
    }
    else {
        field[_i][_j]->open();
        opened++;
    }

    count_open_tiles += opened;
    std::cout << count_open_tiles << "\n";
}

bool Field::open_mines(int time) {
    open_mines_timer += time;
    if (open_mines_timer > 25) {
        open_mines_timer = 0;
        field[mines[opening_mine].y][mines[opening_mine].x]->open();
        if (opening_mine < mines.size() - 1) {
            opening_mine++;
        }
        else {
            return true;
        }
    }
    return false;
}

bool Field::defuse_mines(int time) {
    defuse_mines_timer += time;
    if (defuse_mines_timer > 100) {
        defuse_mines_timer = 0;
        field[mines[opening_mine].y][mines[opening_mine].x]->defuse();
        if (opening_mine < mines.size() - 1) {
            opening_mine++;
        }
        else {
            return true;
        }
    }
    return false;
}

void Field::reset() {
    for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
        for (int j = 1; j < FIELD_WIDTH + 1; j++) {
            field[i][j]->reset();
        }
    }

    opening_mine = 0;
    open_mines_timer = 0;
    defuse_mines_timer = 0;
    count_open_tiles = 0;

    mines.clear();
}

bool Field::check_win() {
    if (count_open_tiles == FIELD_WIDTH * FIELD_HEIGHT - MINES_COUNT) {
        return true;
    }
    return false;
}

void Field::show(sf::RenderWindow& window) {
    window.draw(bg);
    for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
        for (int j = 1; j < FIELD_WIDTH + 1; j++) {
            field[i][j]->show(window);
        }
    }
}

std::vector<std::shared_ptr<Tile>> Field::operator[](int index) {
    return field[index];
}

bool Field::check_for_hint(std::vector<sf::Vector2i>& hint_tiles) {
    for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
        for (int j = 1; j < FIELD_WIDTH + 1; j++) {
            if (field[i][j]->is_closed() || field[i][j]->get_number() == 0) {
                continue;
            }

            int closed = 0;
            int marked = 0;
            for (int y = i - 1; y <= i + 1; y++) {
                for (int x = j - 1; x <= j + 1; x++) {
                    if (field[y][x]->is_marked())
                        marked++;
                    else if (field[y][x]->is_closed() && y > 0 && y < FIELD_HEIGHT + 1 && x > 0 && x < FIELD_WIDTH + 1)
                        closed++;
                }
            }

            if (field[i][j]->get_number() == marked && closed > 0) {
                std::cout << "obviously can open around: " << i << " " << j << "\n";
                return false;
            }
            if (field[i][j]->get_number() == closed + marked && closed > 0) {
                std::cout << "obviously can mark around: " << i << " " << j << "\n";
                return false;
            }
            else {
                for (int y = i - 1; y <= i + 1; y++) {
                    for (int x = j - 1; x <= j + 1; x++) {
                        if (field[y][x]->is_mine() && !field[y][x]->is_marked()) {
                            hint_tiles.push_back({ x, y });
                            break;
                        }
                    }
                }
            }
        }
    }
    std::cout << "hint\n";
    return true;
}
