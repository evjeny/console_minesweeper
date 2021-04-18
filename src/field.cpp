//
// Created by evjeny on 4/18/21.
//

#include "../include/field.h"
#include "../include/color_print.h"

#include <random>
#include <algorithm>
#include <numeric>

#define print_color(color, c) stream << color##_color << (c) << reset_color

size_t field::to_position(size_t x, size_t y) const {
    assert((x < width) && (y < height));
    return y * width + x;
}

std::pair<env::CellStatus, env::Cell> field::get(size_t x, size_t y) const {
    size_t position = to_position(x, y);

    if (_field_status[position] == env::CellStatus::CLOSED) {
        return std::make_pair(env::CellStatus::CLOSED, env::Cell::UNKNOWN);
    }

    return {_field_status[position], _field[position]};
}

env::MoveResult field::mark(size_t x, size_t y) {
    if (game_status != env::GameStatus::PLAYING) {
        return env::MoveResult::GAME_FINISHED;
    }

    auto [cell_status, cell] = get(x, y);

    if (cell_status == env::CellStatus::CLOSED && marks < bombs) {
        _field_status[to_position(x, y)] = env::CellStatus::MARKED;
        marks++;

        if (closed_cells - marks == 0) {
            return env::MoveResult::GAME_FINISHED;
        }

        return env::MoveResult::NEW;
    } else if (cell_status == env::CellStatus::MARKED) {
        _field_status[to_position(x, y)] = env::CellStatus::CLOSED;
        marks--;
        return env::MoveResult::NEW;
    }

    return env::MoveResult::ALREADY_OPENED;
}

std::vector<coordinate> field::make_update_vector(size_t x, size_t y) {
    std::vector<coordinate> to_visit{{x, y}};
    std::vector<coordinate> result;

    while (!to_visit.empty()) {
        coordinate cur = to_visit.back();
        to_visit.pop_back();

        size_t cur_position = to_position(cur.first, cur.second);
        if (_field_status[cur_position] != env::CellStatus::CLOSED) {
            continue;
        }

        result.push_back(cur);

        _field_status[cur_position] = env::CellStatus::OPENED;
        closed_cells--;

        if (_field[cur_position] == env::Cell::BOMBS_0) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (cur.first + dx >= width || cur.second + dy >= height) {
                        continue;
                    }

                    if (_field_status[to_position(cur.first + dx, cur.second + dy)] == env::CellStatus::CLOSED) {
                        to_visit.emplace_back(cur.first + dx, cur.second + dy);
                    }
                }
            }
        }
    }

    return result;
}

std::pair<env::MoveResult, std::vector<coordinate>> field::open(size_t x, size_t y) {
    if (game_status != env::GameStatus::PLAYING) {
        return std::make_pair(env::MoveResult::GAME_FINISHED, std::vector<coordinate>());
    }

    size_t position = to_position(x, y);
    env::CellStatus status = _field_status[position];
    env::Cell cell = _field[position];

    if (status != env::CellStatus::CLOSED) {
        return std::make_pair(env::MoveResult::ALREADY_OPENED, std::vector<coordinate>());
    }

    if (cell == env::Cell::BOMB) {
        _field_status[to_position(x, y)] = env::CellStatus::OPENED;
        return std::make_pair(env::MoveResult::GAME_FINISHED, std::vector<coordinate>{{x, y}});
    }

    std::vector<coordinate> update_coordinates = make_update_vector(x, y);
    if (closed_cells - marks == 0) {
        return std::make_pair(env::MoveResult::GAME_FINISHED, update_coordinates);
    }

    return std::make_pair(env::MoveResult::NEW, update_coordinates);
}

void field::generate() {
    std::vector<size_t> indices(width * height);
    std::iota(indices.begin(), indices.end(), 0);

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::shuffle(indices.begin(), indices.end(), generator);

    for (size_t i = 0; i < bombs; i++) {
        _field[indices[i]] = env::Cell::BOMB;
    }

    for (size_t i = bombs; i < indices.size(); i++) {
        size_t y = indices[i] / width;
        size_t x = indices[i] % width;

        size_t nearest_bombs = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (x + dx >= width || y + dy >= height) {
                    continue;
                }
                if (dx == 0 && dy == 0) {
                    continue;
                }

                if (_field[to_position(x + dx, y + dy)] == env::Cell::BOMB) {
                    nearest_bombs++;
                }
            }
        }

        env::Cell cur_cell;

#define BOMBS(k) case k: { cur_cell = env::Cell::BOMBS_##k ; break; }

        switch (nearest_bombs) {
            BOMBS(0)
            BOMBS(1)
            BOMBS(2)
            BOMBS(3)
            BOMBS(4)
            BOMBS(5)
            BOMBS(6)
            BOMBS(7)
            BOMBS(8)
            default: {
                cur_cell = env::Cell::BOMBS_0;
            }
        }
#undef BOMBS

        _field[indices[i]] = cur_cell;
    }
}

void field::begin() {
    generate();
    std::fill(_field_status.begin(), _field_status.end(), env::CellStatus::CLOSED);
    game_status = env::GameStatus::PLAYING;
    closed_cells = width * height;
    marks = 0;
}

env::GameStatus field::get_game_status() {
    size_t true_hits = 0;
    size_t bomb_hits = 0;

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t position = to_position(x, y);
            if (_field[position] == env::Cell::BOMB && _field_status[position] == env::CellStatus::MARKED) {
                true_hits++;
            } else if (_field[position] == env::Cell::BOMB && _field_status[position] == env::CellStatus::OPENED) {
                bomb_hits++;
            }
        }
    }

    if (closed_cells - marks == 0 && true_hits == bombs) {
        return env::GameStatus::WIN;
    } else if (bomb_hits > 0) {
        return env::GameStatus::LOSE;
    }

    return env::GameStatus::PLAYING;
}

void field::print_true_field(std::ostream &stream) {
    for (size_t cy = 0; cy < height; cy++) {
        for (size_t cx = 0; cx < width; ++cx) {
            size_t position = to_position(cx, cy);
            if (_field[position] == env::Cell::BOMB) {
                stream << ' ' << red_color << static_cast<char>(_field[position]) << reset_color << ' ';
            } else {
                stream << ' ' << static_cast<char>(_field[position]) << ' ';
            }
        }
        stream << '\n';
    }
}

void field::print_field_status(std::ostream &stream, size_t x, size_t y) {
    for (size_t cy = 0; cy < height; cy++) {
        for (size_t cx = 0; cx < width; ++cx) {
            char open_char, close_char;
            if (cx == x && cy == y) {
                open_char = '[';
                close_char = ']';
            } else {
                open_char = close_char = ' ';
            }

            stream << open_char;

            size_t cur_position = to_position(cx, cy);
            switch(_field_status[cur_position]) {
                case env::CellStatus::CLOSED: {
                    stream << '?';
                    break;
                }
                case env::CellStatus::MARKED: {
                    print_color(yellow, '!');
                    break;
                }
                case env::CellStatus::OPENED: {
                    env::Cell cell = _field[cur_position];
                    char print_char = static_cast<char>(cell);
                    if (cell == env::Cell::BOMB) {
                        print_color(red, print_char);
                    } else if (cell == env::Cell::BOMBS_0) {
                        print_color(green, print_char);
                    } else if (cell == env::Cell::BOMBS_1 || cell == env::Cell::BOMBS_2 || cell == env::Cell::BOMBS_3) {
                        print_color(blue, print_char);
                    } else if (cell == env::Cell::BOMBS_4 || cell == env::Cell::BOMBS_5 || cell == env::Cell::BOMBS_6) {
                        print_color(cyan, print_char);
                    } else if (cell == env::Cell::BOMBS_7 || cell == env::Cell::BOMBS_8) {
                        print_color(magenta, print_char);
                    }
                }
            }

            stream << close_char;
        }
        stream << '\n';
    }
}

void field::print_field_header(std::ostream &stream) {
    stream << "Left bombs: " << red_color << bombs - marks << reset_color << "\tLeft cells: "
        << blue_color << get_closed_cells_count() << reset_color << "\n===\n\n";
}
