/* ----------------------------------------------------------------<Header>-
 Name:        Matrix.cpp
 Title:       Hitori grid container — implementation.
 Description: Implements Matrix construction, block indexing,
              and cell state accessors. Validation logic lives
              in Solver. All cells initialise to NONE (unresolved).
 ------------------------------------------------------------------</Header>-*/
#include "Matrix.h"
#include <stdexcept>

Matrix::Matrix(int n, const std::vector<int> &values, const std::vector<int> &blocks)
    : n(n),
      cells(n, std::vector<Cell>(n)),
      blocks(blocks.empty() ? std::vector<int>(n * n, -1) : blocks)
{
    if ((int)values.size() != n * n) {
        throw std::invalid_argument("Matrix: values.size() must equal n*n");
    }

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cells[r][c].set_number(values[r * n + c]);
            cells[r][c].set_state(Cell::State::NONE);
        }
    }
    build_block_index();
}

/* ---------------------------------------------------------------------[<]-
 Function: build_block_index
 Synopsis: Builds block_map: maps each block id to its flat cell indices.
 ---------------------------------------------------------------------[>]-*/
void Matrix::build_block_index() {
    int max_b = -1;
    for (int b : blocks) {
        if (b > max_b) {
            max_b = b;
        }
    }

    if (max_b >= 0) {
        block_map.assign(max_b + 1, {});
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                int b = blocks[r * n + c];
                if (b >= 0) {
                    block_map[b].push_back(r * n + c);
                }
            }
        }
    }
}

/* ---------------------------------------------------------------------[<]-
 Function: get_size
 Synopsis: Returns the side length of the grid.
 ---------------------------------------------------------------------[>]-*/
int Matrix::get_size() const {
    return n;
}

/* ---------------------------------------------------------------------[<]-
 Function: get_number
 Synopsis: Returns the puzzle number stored at (r, c).
 ---------------------------------------------------------------------[>]-*/
int Matrix::get_number(int r, int c) const {
    return cells[r][c].get_number();
}

/* ---------------------------------------------------------------------[<]-
 Function: get_block
 Synopsis: Returns the block id for cell (r, c), or -1 if none.
 ---------------------------------------------------------------------[>]-*/
int Matrix::get_block(int r, int c) const {
    return blocks[r * n + c];
}

/* ---------------------------------------------------------------------[<]-
 Function: get_block_map
 Synopsis: Returns the full block_map (block id -> flat cell indices).
 ---------------------------------------------------------------------[>]-*/
const std::vector<std::vector<int>> &Matrix::get_block_map() const {
    return block_map;
}

/* ---------------------------------------------------------------------[<]-
 Function: get_blocks
 Synopsis: Returns the raw flat block-id vector.
 ---------------------------------------------------------------------[>]-*/
const std::vector<int> &Matrix::get_blocks() const {
    return blocks;
}

/* ---------------------------------------------------------------------[<]-
 Function: set_state
 Synopsis: Sets the state of cell (r, c).
 ---------------------------------------------------------------------[>]-*/
void Matrix::set_state(int r, int c, Cell::State s) {
    cells[r][c].set_state(s);
}

/* ---------------------------------------------------------------------[<]-
 Function: is_black
 Synopsis: Returns true if cell (r, c) is BLACK.
 ---------------------------------------------------------------------[>]-*/
bool Matrix::is_black(int r, int c) const {
    return cells[r][c].is_black();
}

/* ---------------------------------------------------------------------[<]-
 Function: is_white
 Synopsis: Returns true if cell (r, c) is WHITE.
 ---------------------------------------------------------------------[>]-*/
bool Matrix::is_white(int r, int c) const {
    return cells[r][c].is_white();
}

/* ---------------------------------------------------------------------[<]-
 Function: is_none
 Synopsis: Returns true if cell (r, c) is NONE (unresolved).
 ---------------------------------------------------------------------[>]-*/
bool Matrix::is_none(int r, int c) const {
    return cells[r][c].is_none();
}

/* ---------------------------------------------------------------------[<]-
 Function: is_empty
 Synopsis: Returns true if cell (r, c) holds no puzzle number (<= 0).
 ---------------------------------------------------------------------[>]-*/
bool Matrix::is_empty(int r, int c) const {
    return cells[r][c].get_number() <= 0;
}
