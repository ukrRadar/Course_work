/* ----------------------------------------------------------------<Header>-
 Name:        Cell.h
 Title:       Single cell of the Hitori grid.
 Description: Declares the Cell class that stores a numeric value
              and a state (NONE, WHITE, BLACK) for each grid cell.
 ------------------------------------------------------------------</Header>-*/
#pragma once

class Cell {
public:
    enum class State {
        NONE,
        WHITE,
        BLACK
    };

    Cell() : number(0), state(State::NONE) {}
    Cell(int n, State s) : number(n), state(s) {}

    int   get_number() const { return number; }
    State get_state()  const { return state; }

    void set_number(int n) { number = n; }
    void set_state(State s) { state = s; }

    bool is_white() const { return state == State::WHITE; }
    bool is_black() const { return state == State::BLACK; }
    bool is_none()  const { return state == State::NONE; }

private:
    int   number;
    State state;
};
