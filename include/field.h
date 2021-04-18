//
// Created by evjeny on 4/18/21.
//

#ifndef CONSOLE_MINESWEEPER_FIELD_H
#define CONSOLE_MINESWEEPER_FIELD_H

#include <vector>
#include <assert.h>
#include <ostream>

namespace env {
    enum class Cell : char {
        BOMBS_0 = '0',
        BOMBS_1 = '1',
        BOMBS_2 = '2',
        BOMBS_3 = '3',
        BOMBS_4 = '4',
        BOMBS_5 = '5',
        BOMBS_6 = '6',
        BOMBS_7 = '7',
        BOMBS_8 = '8',
        BOMB = 'x',
        UNKNOWN = '?'
    };
    enum class CellStatus : char {
        CLOSED = '?',
        MARKED = '!',
        OPENED = '.'
    };
    enum class GameStatus {
        NOT_STARTED,
        PLAYING,
        WIN,
        LOSE
    };
    enum class MoveResult {
        NEW,
        ALREADY_OPENED,
        GAME_FINISHED
    };
}

using coordinate = std::pair<size_t, size_t>;

class field {
private:
    size_t width, height;
    size_t bombs;
    size_t closed_cells;
    size_t marks;

    std::vector<env::Cell> _field;
    std::vector<env::CellStatus> _field_status;

    env::GameStatus game_status;

    size_t to_position(size_t x, size_t y) const;

    std::vector<coordinate> make_update_vector(size_t x, size_t y);

    void generate();

public:
    field(size_t width, size_t height, size_t n_bombs) :
            width(width), height(height), bombs(n_bombs),
            _field(width * height),
            _field_status(width * height),
            game_status(env::GameStatus::NOT_STARTED) {
        assert(n_bombs <= width * height);
    }

    void begin();

    std::pair<env::MoveResult, std::vector<coordinate>> open(size_t x, size_t y);

    env::MoveResult mark(size_t x, size_t y);

    std::pair<env::CellStatus, env::Cell> get(size_t x, size_t y) const;

    env::GameStatus get_game_status();

    void print_true_field(std::ostream &stream);

    void print_field_status(std::ostream &stream, size_t x, size_t y);

    size_t get_closed_cells_count() {
        return closed_cells - marks;
    }

    size_t get_marks_count() {
        return marks;
    }
};


#endif //CONSOLE_MINESWEEPER_FIELD_H
