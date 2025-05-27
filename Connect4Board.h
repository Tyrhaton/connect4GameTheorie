#ifndef CONNECT4_BOARD_H
#define CONNECT4_BOARD_H

#include "include.h"

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

enum Player
{
    EMPTY = 0,
    PLAYER1 = 1,
    PLAYER2 = 2
};

class Connect4Board
{
public:
    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;

    enum Mark
    {
        EMPTYMARK = '.',
        PLAYER1MARK = 'X',
        PLAYER2MARK = 'O'
    };

    array<array<Player, COLS>, ROWS> grid;

    Connect4Board()
    {
        for (auto &row : grid)
        {
            row.fill(Player::EMPTY);
        }
    }

    /**
     * Print the current state of the board to the console.
     */
    void print() const
    {
        for (int r = 0; r < ROWS; ++r)
        {
            // Row labels: 6 down to 1
            cout << (ROWS - r) << ' ';
            for (int c = 0; c < COLS; ++c)
            {
                char ch = EMPTYMARK;
                if (grid[r][c] == PLAYER1)
                    ch = PLAYER1MARK;
                else if (grid[r][c] == PLAYER2)
                    ch = PLAYER2MARK;

                cout << ch << ' ';
            }
            cout << '\n';
        }
        // Column labels: A B C D E F G
        cout << "  ";
        for (int c = 0; c < COLS; ++c)
        {
            char label = 'A' + c;
            cout << label << ' ';
        }
        cout << "\n";
    }

    /**
     * Drop a disc in the specified column for the given player.
     * @param column The column to drop the disc in.
     * @param player The player dropping the disc.
     * @return True if the move results in a win, false otherwise.
     */
    bool dropDisc(Column column, Player player)
    {
        if (column < 0 || column >= COLS)
            throw out_of_range("Column index out of range");
        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (grid[row][column] == EMPTY)
            {
                setCell(row, column, player);
                bool win = checkWin(player);
                return win;
            }
        }
        throw runtime_error("Column is full");
    }

    /**
     * Get the current state of a cell in the grid.
     * @param row The row index.
     * @param col The column index.
     * @return The Player at the specified cell.
     */
    Player getCell(int row, int col) const
    {
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
            throw out_of_range("Row or column index out of range");
        return grid[row][col];
    }

    /**
     * Set the state of a cell in the grid.
     * @param row The row index.
     * @param column The column index.
     * @param val The Player value to set at the specified cell.
     */
    void setCell(int row, int column, Player val)
    {
        // optionele bound-checks:
        if (row < 0 || row >= ROWS || column < 0 || column >= COLS)
        {
            throw out_of_range("Connect4Board::setCell: index out of range");
        }
        grid[row][column] = val;
    }

    /**
     * Get the opponent of the specified player.
     * @param player The current player.
     * @return The opponent player.
     */
    Player getOponent(Player player) const
    {
        return (player == PLAYER1) ? PLAYER2 : PLAYER1;
    }

    /**
     * Check if the specified player has won the game.
     * @param player The player to check for a win.
     * @return True if the player has won, false otherwise.
     */
    bool checkWin(Player player) const
    {
        static constexpr int dr[4] = {0, 1, 1, 1};
        static constexpr int dc[4] = {1, 0, 1, -1};

        for (int r = 0; r < ROWS; ++r)
        {
            for (int c = 0; c < COLS; ++c)
            {
                if (getCell(r, c) != player)
                    continue;
                for (int dir = 0; dir < 4; ++dir)
                {
                    int cnt = 1;
                    int rr = r + dr[dir], cc = c + dc[dir];
                    while (inBoard(rr, cc) && getCell(rr, cc) == player)
                    {
                        ++cnt;
                        if (cnt == 4)
                            return true;
                        rr += dr[dir];
                        cc += dc[dir];
                    }
                }
            }
        }
        return false;
    }

    /**
     * Check if the specified row and column are within the bounds of the board.
     * @param r The row index.
     * @param c The column index.
     * @return True if the indices are within bounds, false otherwise.
     */
    bool inBoard(int r, int c) const
    {
        return r >= 0 && r < ROWS && c >= 0 && c < COLS;
    }

    /**
     * Find the lowest empty row in the specified column.
     * @param column The column to check.
     * @return The row index of the lowest empty cell, or -1 if the column is full.
     */
    int findRow(int column)
    {
        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (getCell(row, column) == Player::EMPTY)
                return row;
        }
        return -1;
    }

    bool columnHasSpace(Column column) const
    {

        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (getCell(row, column) == Player::EMPTY)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Adds functionality to compare 2 boards their grid
     * @return true if the grids are equal, false otherwise
     */
    bool operator==(const Connect4Board &other) const
    {
        return this->grid == other.grid;
    }

    /**
     * Adds functionality to check if 2 boards their grid are not equal
     * @return true if the grids are not equal, false otherwise
     */
    bool operator!=(const Connect4Board &other) const
    {
        return !(*this == other);
    }
};

#endif // CONNECT4_BOARD_H