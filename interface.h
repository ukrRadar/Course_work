/* ----------------------------------------------------------------<Header>-
 Name:        interface.h
 Title:       Terminal UI — colors and display function declarations.
 Description: Declares the Colors struct with ANSI escape constants
              and the public interface for printing the Hitori board,
              solve results, and the title screen.
 ------------------------------------------------------------------</Header>-*/
#pragma once
#include <string>
#include <array>
#include "Matrix.h"

struct Colors {
    static constexpr const char *RESET   = "\033[0m";
    static constexpr const char *BOLD    = "\033[1m";
    static constexpr const char *CYAN    = "\033[36m";
    static constexpr const char *GREEN   = "\033[32m";
    static constexpr const char *RED     = "\033[31m";
    static constexpr const char *YELLOW  = "\033[33m";
    static constexpr const char *MAGENTA = "\033[35m";
    static constexpr const char *DIM     = "\033[2m";

    static constexpr const char *BG_EMPTY   = "\033[48;5;234m";
    static constexpr const char *CELL_BLACK = "\033[40;97;1m";
    static constexpr const char *CELL_WHITE = "\033[107;30;1m";

    static constexpr std::array<const char *, 9> PALETTE = {{
        "\033[43;30;1m",
        "\033[42;30;1m",
        "\033[46;30;1m",
        "\033[41;30;1m",
        "\033[45;30;1m",
        "\033[48;5;202;30;1m",
        "\033[48;5;27;30;1m",
        "\033[48;5;160;30;1m",
        "\033[48;5;93;30;1m"
    }};
};

void print_title_screen();
void print_boards_side_by_side(const Matrix &before, const Matrix &after, const std::string &title);
void print_solve_result(const std::string &title, double duration_ms, bool found);
void wait_enter();
