/* ----------------------------------------------------------------<Header>-
 Name:        interface.cpp
 Title:       Terminal UI — implementation.
 Description: Implements all terminal output functions: title screen,
              side-by-side board display, coloured cell rendering,
              and solve result reporting.
 ------------------------------------------------------------------</Header>-*/
#include "interface.h"
#include <iostream>
#include <iomanip>

/* Layout constants */
namespace UI {
    static constexpr int CELL_W = 4;
    static constexpr int MARGIN = 2;
    static constexpr int GAP    = 4;
    static constexpr int BOX_W  = 50;
}

/* Internal helpers (static) */

static void draw_line(char c, int width) {
    std::cout << "+" << std::string(width - 2, c) << "+\n";
}

static void draw_row(const std::string &text, int width, const char *color = Colors::RESET) {
    int inner_width = width - 2;
    std::cout << "| " << color << std::left << std::setw(inner_width - 1) << text << Colors::RESET << "|\n";
}

static void print_cell_solid(const Matrix &m, int r, int c) {
    int val      = m.get_number(r, c);
    int block_id = m.get_block(r, c);

    std::string num = (val <= 0) ? "  " : std::to_string(val);
    if (num.length() == 1) {
        num = " " + num;
    }

    if (m.is_black(r, c)) {
        std::cout << Colors::CELL_BLACK << " " << num << " " << Colors::RESET;
    } else if (block_id >= 0) {
        std::cout << Colors::PALETTE[block_id % Colors::PALETTE.size()]
                  << " " << num << " " << Colors::RESET;
    } else {
        std::cout << Colors::CELL_WHITE << " " << num << " " << Colors::RESET;
    }
}

static void print_task_header(const std::string &title) {
    std::cout << "\n" << Colors::BOLD << Colors::CYAN << "TASK: " << title << Colors::RESET << "\n\n";
}

static void print_board_labels(int n_size) {
    int board_w    = n_size * UI::CELL_W;
    int before_pad = UI::MARGIN + (board_w - 6) / 2;
    int remaining  = board_w - ((board_w - 6) / 2 + 6);
    int after_pad  = remaining + UI::GAP + (board_w - 5) / 2;

    std::cout << std::string(before_pad, ' ') << "BEFORE" << std::string(after_pad,  ' ') << "AFTER\n";
}

static void print_combined_row(const Matrix &left, const Matrix &right, int row) {
    int size = left.get_size();

    std::cout << std::string(UI::MARGIN, ' ');
    for (int c = 0; c < size; c++) {
        print_cell_solid(left, row, c);
    }

    std::cout << std::string(UI::GAP, ' ');
    for (int c = 0; c < size; c++) {
        print_cell_solid(right, row, c);
    }
    std::cout << "\n";
}

/* Public functions */

/* ---------------------------------------------------------------------[<]-
 Function: print_title_screen
 Synopsis: Clears the terminal and prints the welcome banner with rules.
 ---------------------------------------------------------------------[>]-*/
void print_title_screen() {
    std::cout << "\033[2J\033[H";

    draw_line('=', UI::BOX_W);
    std::cout << "|      " << Colors::BOLD << Colors::CYAN
              << "H I T O R I   S O L V E R" << Colors::RESET << "                 |\n";
    draw_line('=', UI::BOX_W);

    draw_row("General Rules:", UI::BOX_W);
    draw_row("1. No duplicate numbers in rows/cols (White).", UI::BOX_W);
    draw_row("2. Black cells cannot touch (H or V).", UI::BOX_W);
    draw_row("3. All white cells must be connected.", UI::BOX_W);

    draw_line('-', UI::BOX_W);
    draw_row("Press Enter to solve...", UI::BOX_W, Colors::CYAN);
    draw_line('=', UI::BOX_W);

    std::cin.get();
}

/* ---------------------------------------------------------------------[<]-
 Function: print_boards_side_by_side
 Synopsis: Prints before/after grids next to each other with a title.
 ---------------------------------------------------------------------[>]-*/
void print_boards_side_by_side(const Matrix &before, const Matrix &after, const std::string &title) {
    print_task_header(title);
    print_board_labels(before.get_size());

    for (int r = 0; r < before.get_size(); r++) {
        print_combined_row(before, after, r);
    }
    std::cout << "\n";
}

/* ---------------------------------------------------------------------[<]-
 Function: print_solve_result
 Synopsis: Prints SUCCESS or NO SOLUTION with elapsed time in ms.
 ---------------------------------------------------------------------[>]-*/
void print_solve_result(const std::string &title, double duration_ms, bool found) {
    const char *color = found ? Colors::GREEN : Colors::RED;
    std::cout << color << Colors::BOLD
              << " >> Result for [" << title << "]: "
              << (found ? "SUCCESS" : "NO SOLUTION")
              << " (" << std::fixed << std::setprecision(2) << duration_ms << " ms)"
              << Colors::RESET << "\n";
}

/* ---------------------------------------------------------------------[<]-
 Function: wait_enter
 Synopsis: Pauses execution until the user presses Enter.
 ---------------------------------------------------------------------[>]-*/
void wait_enter() {
    std::cout << Colors::DIM << "Press Enter to continue..." << Colors::RESET;
    std::cin.get();
}
