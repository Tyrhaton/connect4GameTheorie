#ifndef CONNECT4_BOARD_H
#define CONNECT4_BOARD_H

#include <array>
#include <iostream>
#include <stdexcept>

enum Column
{
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6
};

class Connect4Board
{
public:
    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;

    enum Cell
    {
        EMPTY = 0,
        PLAYER1 = 1,
        PLAYER2 = 2
    };

    enum Mark
    {
        EMPTYMARK = '.',
        PLAYER1MARK = 'X',
        PLAYER2MARK = 'O'
    };

    std::array<std::array<Cell, COLS>, ROWS> grid;

    Connect4Board()
    {
        for (auto &row : grid)
        {
            row.fill(EMPTY);
        }
    }
    void print() const
    {
        for (int r = 0; r < ROWS; ++r)
        {
            // Row labels: 6 down to 1
            std::cout << (ROWS - r) << ' ';
            for (int c = 0; c < COLS; ++c)
            {
                char ch = EMPTYMARK;
                if (grid[r][c] == PLAYER1)
                    ch = PLAYER1MARK;
                else if (grid[r][c] == PLAYER2)
                    ch = PLAYER2MARK;

                std::cout << ch << ' ';
            }
            std::cout << '\n';
        }
        // Column labels: A B C D E F G
        std::cout << "  ";
        for (int c = 0; c < COLS; ++c)
        {
            char label = 'A' + c;
            std::cout << label << ' ';
        }
        std::cout << "\n";
    }
    int dropDisc(Column col, Cell player)
    {
        if (col < 0 || col >= COLS)
            throw std::out_of_range("Column index out of range");
        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (grid[row][col] == EMPTY)
            {
                grid[row][col] = player;
                return row;
            }
        }
        throw std::runtime_error("Column is full");
    }
    bool checkDirection(int row, int col, Cell player, int dx, int dy) const
    {
        int count = 1;
        count += countInDirection(row, col, player, dx, dy);
        count += countInDirection(row, col, player, -dx, -dy);
        return count >= 4;
    }

    int countInDirection(int row, int col, Cell player, int dx, int dy) const
    {
        int r = row + dy;
        int c = col + dx;
        int cnt = 0;
        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && grid[r][c] == player)
        {
            ++cnt;
            r += dy;
            c += dx;
        }
        return cnt;
    }

    bool checkWin(int row, int col, Cell player) const
    {
        return checkDirection(row, col, player, 1, 0) || checkDirection(row, col, player, 0, 1) || checkDirection(row, col, player, 1, 1) || checkDirection(row, col, player, 1, -1);
    }

    Cell getCell(int row, int col) const
    {
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
            throw std::out_of_range("Row or column index out of range");
        return grid[row][col];
    }
    void setCell(int row, int col, Cell val)
    {
        // optionele bound-checks:
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        {
            throw std::out_of_range("Connect4Board::setCell: index out of range");
        }
        grid[row][col] = val;
    }
};

#endif // CONNECT4_BOARD_H
