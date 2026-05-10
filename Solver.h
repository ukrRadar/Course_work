/* ----------------------------------------------------------------<Header>-
 Name:        Solver.h
 Title:       Hitori Optimized Solver — interface.
 Description: Constraint propagation (Rules A/B/C) followed by
              backtracking with MRV and forward checking.
              Supports standard Hitori and the block variant.
 ------------------------------------------------------------------</Header>-*/
#pragma once
#include "Matrix.h"
#include <vector>
#include <unordered_map>
#include <utility>

class Solver {
public:
    explicit Solver(Matrix &board);
    bool solve();

private:
    /* Return type for safe cell-state setters.
       CONFLICT means the cell already holds the opposite state. */
    enum class Result { OK, CONFLICT };

    Matrix &board;
    int    n;

    /* Reusable buffers — avoids allocation on every connectivity check. */
    mutable std::vector<bool> visited_flat;
    mutable std::vector<int>  stack_buf;

    /* Constraint propagation */
    bool apply_initial_logic();
    bool apply_rule_A(bool &changed);
    bool apply_rule_B(bool &changed);
    bool apply_rule_C(bool &changed);

    /* Safe setters: return CONFLICT on contradiction, OK otherwise. */
    Result set_black(int r, int c, bool &changed);
    Result set_white(int r, int c, bool &changed);

    /* Backtracking */
    bool                  can_place(int r, int c, Cell::State state) const;
    bool                  forward_check_violated(int r, int c) const;
    std::pair<int, int>   pick_next_cell() const;
    bool                  backtrack();

    /* Connectivity */
    bool whites_are_connected() const;
};
