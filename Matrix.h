/* ----------------------------------------------------------------<Header>-
 Name:        Matrix.h
 Title:       Hitori grid container.
 Description: Declares the Matrix class that holds an n×n grid of
              Cell objects and an optional block layout.
              Validation logic lives in Solver.
 ------------------------------------------------------------------</Header>-*/
#pragma once
#include "Cell.h"
#include <vector>

class Matrix {
public:
    Matrix(int n, const std::vector<int> &values, const std::vector<int> &blocks = {});

    int  get_size()               const;
    int  get_number(int r, int c) const;
    int  get_block (int r, int c) const;

    const std::vector<std::vector<int>> &get_block_map() const;
    const std::vector<int>                &get_blocks()    const;

    void set_state(int r, int c, Cell::State s);

    bool is_black(int r, int c) const;
    bool is_white(int r, int c) const;
    bool is_none(int r, int c) const;
    bool is_empty(int r, int c) const;

private:
    int                            n;
    std::vector<std::vector<Cell>> cells;
    std::vector<int>               blocks;
    std::vector<std::vector<int>> block_map;

    void build_block_index();
};
