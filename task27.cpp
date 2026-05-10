/* ----------------------------------------------------------------<Header>-
 Name:        task27.cpp
 Title:       Hitori solver.
 Group:       TV-51
 Student:     Pavelchuk Y.V.
 Written:     2026-05-09
 Revised:     2026-05-09
 Description: Initialises several Hitori boards, solves each one via
              the Solver class, and prints the before/after grids
              together with the elapsed solve time.
 ------------------------------------------------------------------</Header>-*/
#include <iostream>
#include <chrono>
#include "Matrix.h"
#include "Solver.h"
#include "interface.h"

/* ---------------------------------------------------------------------[<]-
 Function: run_test
 Synopsis: Copies the board, solves it, measures elapsed time, and
           prints the result side-by-side with the original.
 ---------------------------------------------------------------------[>]-*/
static void run_test(const char *title, const Matrix &board) {
    Matrix copy = board;

    auto t_start = std::chrono::high_resolution_clock::now();
    bool found   = Solver(copy).solve();
    auto t_end   = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = t_end - t_start;

    print_boards_side_by_side(board, copy, title);
    print_solve_result(title, elapsed.count(), found);
    wait_enter();
}

int main() {
    print_title_screen();

    /* Test 1 */
    Matrix hitori(6, {
        6, 6, 3, 2, 1, 5,
        1, 2, 6, 3, 5, 4,
        5, 1, 4, 2, 3, 4,
        3, 3, 3, 5, 2, 1,
        4, 2, 1, 6, 5, 3,
        6, 1, 5, 4, 4, 4
    });
    run_test("Hitori 6x6", hitori);

    /* Test 2 */
    Matrix hitori6(6,
        std::vector<int>{
            2, 2, 3, 6, 4, 7,
            1, 5, 2, 5, 3, 6,
            3, 1, 4, 5, 7, 2,
            2, 3, 6, 7, 1, 5,
            5, 4, 7, 2, 6, 4,
            3, 4, 6, 3, 7, 7},
        std::vector<int>{
            0, 1, 2, 3, 3, 3,
            0, 1, 2, 3, 3, 3,
            0, 1, 2, 3, 3, 3,
            0, 1, 2, 2, 2, 2,
            0, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0}
    );
    run_test("Hitori 6x6", hitori6);

    /* Test 3 */
    Matrix hitori7(7,
        std::vector<int>{
           -1,  1,  2, 3, 5, 4, 7,
            1, -1,  3, 9, 7, 2, 4,
            2,  4, -1, 1, 3, 9, 5,
            3,  8,  4,-1, 6, 1, 2,
            4,  5,  8, 2,-1, 7, 1,
            7,  6,  5, 4, 2,-1, 3,
            6,  2,  9, 5, 1, 3,-1},
        std::vector<int>{
            0, 0, 0,  2,  3,  3,  3,
            0, 0, 1,  2,  2,  3,  3,
            0, 1, 1,  2,  2,  2,  3,
            1, 1, 1, -1,  4,  4,  4,
            6, 5, 5,  5,  4,  4,  7,
            6, 6, 5,  5,  4,  7,  7,
            6, 6, 6,  5,  7,  7,  7}
    );
    run_test("Hitori 7x7", hitori7);

    /* Test 4 */
    Matrix hitori10(10, 
        std::vector<int>{
            10, 6, 7, 4, 5, 9, 7, 5, 3, 10,
            4, 7, 8, 7, 3, 7, 6, 9, 7, 5,
            1, 2, 5, 6, 6, 4, 6, 3, 10, 7,
            5, 6, 9, 6, 4, 6, 8, 6, 1, 10,
            6, 9, 6, 2, 6, 8, 5, 10, 4, 6,
            6, 1, 10, 7, 7, 5, 5, 6, 8, 4,
            2, 7, 6, 10, 9, 7, 4, 8, 7, 1,
            9, 4, 10, 5, 10, 6, 9, 2, 7, 9,
            7, 8, 4, 9, 5, 8, 3, 1, 1, 2,
            8, 5, 2, 8, 8, 7, 4, 4, 9, 8
    });
    run_test("Hitori 10x10", hitori10);

    Matrix hitori12(12, 
        std::vector<int>{
         8, 12,  3,  6, 11,  2,  9,  4,  7,  1,  5, 10,
         5,  7, 11,  9,  3, 12,  6,  2, 10,  8,  4,  1,
         3,  1,  7, 12,  8,  5,  2, 10,  4, 11,  9,  6,
        11,  6,  9,  2,  7,  4, 12,  8,  1,  5, 10,  3,
         2,  4,  5,  1,  9, 10,  3,  7, 12,  6, 11,  8,
         9,  3, 12,  8,  4,  6,  5, 11,  2, 10,  1,  7,
         6, 11,  2,  7,  5,  1,  8,  3,  9, 12,  4,  2,
         4,  8,  1, 10,  2,  3,  7,  6, 11,  9, 12,  5,
        12,  5, 10,  3,  6,  9, 11,  1,  8,  4,  2, 11,
         7,  9,  4,  5, 12,  8,  1,  9,  3,  2,  6, 11,
         1, 10,  6, 11,  4,  7,  9,  5,  2,  3,  8, 12,
        10,  2,  8,  4,  1, 11,  4, 12,  6,  7,  3,  9
    });
    run_test("Hitori 12x12", hitori12);

    return 0;
}
