#include <iostream>
#include <cstdio>

#include "../include/field.h"

int main() {
    std::system("clear");

    const size_t width = 10;
    const size_t height = 10;
    const size_t n_bombs = 25;

    field game(width, height, n_bombs);
    game.begin();

    int w = 119, a = 97, s = 115, d = 100;
    int enter = 10;
    int f = 102;
    int e = 101;

    size_t cur_x = 0;
    size_t cur_y = 0;

    while (true) {
        std::system("clear");
        game.print_field_status(std::cout, cur_x, cur_y);

        int cur_move = getchar();
        if (cur_move == e) {
            break;
        } else if (cur_move == enter) {
            auto [result, updates] = game.open(cur_x, cur_y);
            if (result == env::MoveResult::GAME_FINISHED) {
                break;
            }
            continue;
        } else if (cur_move == f) {
            game.mark(cur_x, cur_y);
        } else if (cur_move == w && cur_y - 1 < height) {
            cur_y--;
        } else if (cur_move == s && cur_y + 1 < height) {
            cur_y++;
        } else if (cur_move == a && cur_x - 1 < width) {
            cur_x--;
        } else if (cur_move == d && cur_x + 1 < width) {
            cur_x++;
        }
        getchar();
    }

    std::system("clear");
    game.print_true_field(std::cout);
    std::cout << "\n\n===\n\n";

    env::GameStatus result_status = game.get_game_status();

    if (result_status == env::GameStatus::WIN) {
        std::cout << "Grats!\n";
    } else if (result_status == env::GameStatus::LOSE) {
        std::cout << "Maybe next time...\n";
    } else {
        std::cout << "Someone interrupted the game 0o\n";
    }

    return 0;
}
