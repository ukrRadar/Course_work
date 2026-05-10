/* ----------------------------------------------------------------<Header>-
 Name:        Solver.cpp
 Title:       Hitori Optimized Solver — implementation.
 Description: Constraint propagation (Rules A/B/C) reduces the search
              space before backtracking begins. Backtracking uses MRV
              (Most Constrained Variable) cell selection and forward
              checking to prune dead branches early.
              Supports standard Hitori and the block variant.
 ------------------------------------------------------------------</Header>-*/
#include "Solver.h"
#include <algorithm>

Solver::Solver(Matrix &board)
    : board(board),
      n(board.get_size()),
      visited_flat(n * n, false)
{
    stack_buf.reserve(n * n);
}

/* PUBLIC ENTRY POINT */

/* ---------------------------------------------------------------------[<]-
 Function: solve
 Synopsis: Runs constraint propagation to a fixed point, then calls
           backtracking on any remaining unresolved cells.
           Returns true if a valid solution was found.
 ---------------------------------------------------------------------[>]-*/
bool Solver::solve() {
    if (!apply_initial_logic()) {
        return false;
    }
    return backtrack();
}

/* ---------------------------------------------------------------------[<]-
 Function: apply_initial_logic
 Synopsis: Iterates Rules A, B, C until the board reaches a fixed point
           or a contradiction is found.
 ---------------------------------------------------------------------[>]-*/
bool Solver::apply_initial_logic() {
    bool changed = true;
    while (changed) {
        changed = false;
        if (!apply_rule_A(changed)) {
            return false;
        }
        if (!apply_rule_B(changed)) {
            return false;
        }
        if (!apply_rule_C(changed)) {
            return false;
        }
    }
    return true;
}

/* ---------------------------------------------------------------------[<]-
 Function: apply_rule_A
 Synopsis: Triplet rule.
           LINEAR — three consecutive identical values in a row or column:
             outer two cells -> BLACK, middle cell -> WHITE.
             Reason: the middle cannot be BLACK because that would place
             two adjacent black cells (one outer is always forced BLACK).
           BLOCK — same value appears 3+ times in one block and exactly
             one is already WHITE: all remaining NONE ones -> BLACK.
             (When no WHITE exists yet among 3+ identical values the
              winner is ambiguous; backtracking resolves it.)
 ---------------------------------------------------------------------[>]-*/
bool Solver::apply_rule_A(bool &changed) {
    /* Linear triplets: rows */
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n - 2; ++c) {
            int val = board.get_number(r, c);
            if (val <= 0) {
                continue;
            }
            if (board.get_number(r, c + 1) == val &&
                board.get_number(r, c + 2) == val) {
                if (set_black(r, c,     changed) == Result::CONFLICT) {
                    return false;
                }
                if (set_black(r, c + 2, changed) == Result::CONFLICT) {
                    return false;
                }
                if (set_white(r, c + 1, changed) == Result::CONFLICT) {
                    return false;
                }
            }
        }
    }

    /* Linear triplets: columns */
    for (int c = 0; c < n; ++c) {
        for (int r = 0; r < n - 2; ++r) {
            int val = board.get_number(r, c);
            if (val <= 0) {
                continue;
            }
            if (board.get_number(r + 1, c) == val &&
                board.get_number(r + 2, c) == val) {
                if (set_black(r,     c, changed) == Result::CONFLICT) {
                    return false;
                }
                if (set_black(r + 2, c, changed) == Result::CONFLICT) {
                    return false;
                }
                if (set_white(r + 1, c, changed) == Result::CONFLICT) {
                    return false;
                }
            }
        }
    }

    /* Block: 3+ identical values, one already WHITE -> rest BLACK */
    for (const auto &block_cells : board.get_block_map()) {
        std::unordered_map<int, std::vector<int>> val_positions;
        for (int idx : block_cells) {
            int br  = idx / n;
            int bc  = idx % n;
            int val = board.get_number(br, bc);
            if (val > 0) {
                val_positions[val].push_back(idx);
            }
        }

        for (auto &[val, positions] : val_positions) {
            if ((int)positions.size() < 3) {
                continue;
            }

            int white_count = 0;
            for (int idx : positions) {
                if (board.is_white(idx / n, idx % n)) {
                    white_count++;
                }
            }

            if (white_count > 1) {
                return false;
            }

            if (white_count == 1) {
                for (int idx : positions) {
                    int br = idx / n;
                    int bc = idx % n;
                    if (board.is_none(br, bc)) {
                        if (set_black(br, bc, changed) == Result::CONFLICT) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

/* ---------------------------------------------------------------------[<]-
 Function: apply_rule_B
 Synopsis: Sandwich rule (linear only).
           Same value at column c and c+2 in the same row, or rows r and
           r+2 in the same column, and neither endpoint is BLACK yet ->
           the middle cell must be WHITE.
           Reason: one endpoint must eventually become BLACK (duplicate);
           whichever it is, its neighbour cannot also be BLACK, so the
           middle cell is forced WHITE.
           Skips pairs where either endpoint is already BLACK (that
           duplicate is resolved and creates no sandwich obligation).
           Block sandwich is intentionally omitted — "between" logic is
           only valid for linear (same row/col) adjacency.
 ---------------------------------------------------------------------[>]-*/
bool Solver::apply_rule_B(bool &changed) {
    /* Row sandwich */
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n - 2; ++c) {
            int val = board.get_number(r, c);
            if (val <= 0) {
                continue;
            }
            if (board.is_black(r, c)) {
                continue;
            }
            if (board.get_number(r, c + 2) == val &&
                !board.is_black(r, c + 2)) {
                if (set_white(r, c + 1, changed) == Result::CONFLICT) {
                    return false;
                }
            }
        }
    }

    /*Column sandwich */
    for (int c = 0; c < n; ++c) {
        for (int r = 0; r < n - 2; ++r) {
            int val = board.get_number(r, c);
            if (val <= 0) {
                continue;
            }
            if (board.is_black(r, c)) {
                continue;
            }
            if (board.get_number(r + 2, c) == val &&
                !board.is_black(r + 2, c)) {
                if (set_white(r + 1, c, changed) == Result::CONFLICT) {
                    return false;
                }
            }
        }
    }

    return true;
}

/* ---------------------------------------------------------------------[<]-
 Function: apply_rule_C
 Synopsis: Forced-white-by-adjacency.
           A NONE cell that already borders a BLACK cell (placed during
           this propagation phase by Rule A) cannot itself become BLACK —
           two adjacent black cells are forbidden — so it must be WHITE.
           NOTE: "unique survivor" logic is intentionally absent. A value
           appearing once in a row is not necessarily WHITE (it may be a
           duplicate in its column or block). That heuristic is wrong and
           causes false NO SOLUTION results.
 ---------------------------------------------------------------------[>]-*/
bool Solver::apply_rule_C(bool &changed) {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (!board.is_none(r, c)) {
                continue;
            }

            bool has_black_neighbour =
                (r > 0     && board.is_black(r - 1, c)) ||
                (r < n - 1 && board.is_black(r + 1, c)) ||
                (c > 0     && board.is_black(r, c - 1)) ||
                (c < n - 1 && board.is_black(r, c + 1));

            if (has_black_neighbour) {
                if (set_white(r, c, changed) == Result::CONFLICT) {
                    return false;
                }
            }
        }
    }
    return true;
}

/* SAFE SETTERS */

/* ---------------------------------------------------------------------[<]-
 Function: set_black
 Synopsis: Attempts to mark (r, c) as BLACK.
           Returns CONFLICT if the cell is already WHITE, OK otherwise.
           Sets changed=true on a new assignment.
 ---------------------------------------------------------------------[>]-*/
Solver::Result Solver::set_black(int r, int c, bool &changed) {
    if (board.is_white(r, c)) {
        return Result::CONFLICT;
    }
    if (!board.is_black(r, c)) {
        board.set_state(r, c, Cell::State::BLACK);
        changed = true;
    }
    return Result::OK;
}

/* ---------------------------------------------------------------------[<]-
 Function: set_white
 Synopsis: Attempts to mark (r, c) as WHITE.
           Returns CONFLICT if the cell is already BLACK, OK otherwise.
           Sets changed=true on a new assignment.
 ---------------------------------------------------------------------[>]-*/
Solver::Result Solver::set_white(int r, int c, bool &changed) {
    if (board.is_black(r, c)) {
        return Result::CONFLICT;
    }
    if (!board.is_white(r, c)) {
        board.set_state(r, c, Cell::State::WHITE);
        changed = true;
    }
    return Result::OK;
}

/* VALIDATION */

/* ---------------------------------------------------------------------[<]-
 Function: can_place
 Synopsis: Returns true if placing state at (r, c) does not violate any
           Hitori rule at this moment:
           BLACK — no adjacent cell may already be BLACK.
           WHITE — no duplicate value may already be WHITE in the same
                   row, column, or block.
 ---------------------------------------------------------------------[>]-*/
bool Solver::can_place(int r, int c, Cell::State state) const {
    if (state == Cell::State::BLACK) {
        if (r > 0 && board.is_black(r - 1, c)) {
            return false;
        }
        if (c > 0 && board.is_black(r, c - 1)) {
            return false;
        }
        if (r < n - 1 && board.is_black(r + 1, c)) {
            return false;
        }
        if (c < n - 1 && board.is_black(r, c + 1)) {
            return false;
        }
    } else {
        int val = board.get_number(r, c);
        if (val <= 0) {
            return true;
        }

        for (int i = 0; i < n; ++i) {
            if (i != c && board.is_white(r, i) && board.get_number(r, i) == val) {
                return false;
            }
        }
        for (int i = 0; i < n; ++i) {
            if (i != r && board.is_white(i, c) && board.get_number(i, c) == val) {
                return false;
            }
        }

        int b = board.get_block(r, c);
        if (b >= 0) {
            for (int idx : board.get_block_map()[b]) {
                int br = idx / n;
                int bc = idx % n;
                if ((br != r || bc != c) &&
                    board.is_white(br, bc) &&
                    board.get_number(br, bc) == val) {
                    return false;
                }
            }
        }
    }
    return true;
}

/* ---------------------------------------------------------------------[<]-
 Function: forward_check_violated
 Synopsis: After placing a value at (r, c), scans all NONE cells in the
           same row and column. If any such cell has no valid placement
           (neither WHITE nor BLACK is legal), the current branch is a
           dead end and we return true to trigger early backtracking.
 ---------------------------------------------------------------------[>]-*/
bool Solver::forward_check_violated(int r, int c) const {
    for (int cc = 0; cc < n; ++cc) {
        if (!board.is_none(r, cc)) {
            continue;
        }
        if (!can_place(r, cc, Cell::State::WHITE) &&
            !can_place(r, cc, Cell::State::BLACK)) {
            return true;
        }
    }
    for (int rr = 0; rr < n; ++rr) {
        if (!board.is_none(rr, c)) {
            continue;
        }
        if (!can_place(rr, c, Cell::State::WHITE) &&
            !can_place(rr, c, Cell::State::BLACK)) {
            return true;
        }
    }
    return false;
}

/*BACKTRACKING*/

/* ---------------------------------------------------------------------[<]-
 Function: pick_next_cell
 Synopsis: MRV (Most Constrained Variable) heuristic.
           Scans all NONE cells and returns the one with the fewest
           legal placements (1 or 2). A cell with 0 options signals an
           immediate dead end. A cell with 1 option is returned at once
           since no better choice exists.
           Returns {-1, -1} when no NONE cells remain (board is full).
 ---------------------------------------------------------------------[>]-*/
std::pair<int, int> Solver::pick_next_cell() const {
    int best_r       = -1;
    int best_c       = -1;
    int best_options = 3;

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (!board.is_none(r, c)) {
                continue;
            }

            int options = 0;
            if (can_place(r, c, Cell::State::WHITE)) {
                options++;
            }
            if (can_place(r, c, Cell::State::BLACK)) {
                options++;
            }

            if (options == 0) {
                return {r, c};
            }
            if (options < best_options) {
                best_options = options;
                best_r       = r;
                best_c       = c;
                if (best_options == 1) {
                    return {r, c};
                }
            }
        }
    }
    return {best_r, best_c};
}

/* ---------------------------------------------------------------------[<]-
 Function: backtrack
 Synopsis: Recursive search over unresolved cells.
           Uses MRV to pick the most constrained cell first, reducing
           the branching factor. Applies forward checking after each
           placement to prune dead branches before recursing.
           Tries WHITE first (typically more constrained), then BLACK.
           After all cells are assigned verifies white connectivity.
 ---------------------------------------------------------------------[>]-*/
bool Solver::backtrack() {
    auto [r, c] = pick_next_cell();

    if (r == -1) {
        return whites_are_connected();
    }

    if (!can_place(r, c, Cell::State::WHITE) &&
        !can_place(r, c, Cell::State::BLACK)) {
        return false;
    }

    /* Try WHITE */
    if (can_place(r, c, Cell::State::WHITE)) {
        board.set_state(r, c, Cell::State::WHITE);
        if (!forward_check_violated(r, c) && backtrack()) {
            return true;
        }
        board.set_state(r, c, Cell::State::NONE);
    }

    /* Try BLACK */
    if (can_place(r, c, Cell::State::BLACK)) {
        board.set_state(r, c, Cell::State::BLACK);
        if (!forward_check_violated(r, c) && backtrack()) {
            return true;
        }
        board.set_state(r, c, Cell::State::NONE);
    }

    return false;
}

/* CONNECTIVITY CHECK */

/* ---------------------------------------------------------------------[<]-
 Function: whites_are_connected
 Synopsis: Stack-based flood-fill over all non-BLACK cells.
           Hitori requires that every white cell forms one connected
           region. Returns true if all non-black cells are reachable
           from a single starting cell.
 ---------------------------------------------------------------------[>]-*/
bool Solver::whites_are_connected() const {
    int start_r     = -1;
    int start_c     = -1;
    int white_count = 0;

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (!board.is_black(r, c)) {
                if (start_r == -1) {
                    start_r = r;
                    start_c = c;
                }
                white_count++;
            }
        }
    }

    if (white_count == 0) {
        return true;
    }

    visited_flat.assign(n * n, false);
    stack_buf.clear();
    stack_buf.push_back(start_r * n + start_c);
    visited_flat[start_r * n + start_c] = true;

    int found = 0;
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};

    while (!stack_buf.empty()) {
        int idx = stack_buf.back();
        stack_buf.pop_back();
        found++;

        int r = idx / n;
        int c = idx % n;

        for (int i = 0; i < 4; ++i) {
            int nr   = r + dr[i];
            int nc   = c + dc[i];
            if (nr < 0 || nr >= n || nc < 0 || nc >= n) {
                continue;
            }
            int nidx = nr * n + nc;
            if (!board.is_black(nr, nc) && !visited_flat[nidx]) {
                visited_flat[nidx] = true;
                stack_buf.push_back(nidx);
            }
        }
    }

    return (found == white_count);
}
