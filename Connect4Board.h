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

string colToChar(Column col)
{
    // return char('A' + static_cast<int>(col));
    switch (col)
    {
    case Column::A:
        return std::string(1, 'A');
    case Column::B:
        return std::string(1, 'B');
    case Column::C:
        return std::string(1, 'C');
    case Column::D:
        return std::string(1, 'D');
    case Column::E:
        return std::string(1, 'E');
    case Column::F:
        return std::string(1, 'F');
    case Column::G:
        return std::string(1, 'G');
    default:
        throw std::invalid_argument("Invalid column");
    }
}

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
    static void test()
    {
        cout << "Board test function called." << endl;
    }

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
        // for (int row = ROWS - 1; row >= 0; --row)
        // {
        //     if (grid[row][column] == EMPTY)
        //     {
        //         cout << "Dropping disc for player " << static_cast<int>(player) << " in column " << colToChar(column) << endl;
        //         setCell(row, column, player);
        //         bool win = checkWin(player);
        //         return win;
        //     }
        // }
        int row = findRow(column);
        // cout << "row: " << row << endl;
        if (row < 0)
        {
            // Column is full; handle as needed (e.g. return false or throw)
            return false;
        }
        // (Optional) move the debug‐print outside of any hot loop if you’re profiling
        // cout << "Dropping disc for player " << static_cast<int>(player)
        //      << " in column " << colToChar(column) << endl;

        // setCell(ROWS - row - 1, COLS - column - 1, player);
        setCell(row, column, player);
        return checkWin(player);

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
        // return grid[ROWS - 1 - row][COLS - 1 - col];
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
        // grid[ROWS - 1 - row][COLS - 1 - column] = val;
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
    int findRow2(int column)
    {
        // for (int row = ROWS - 1; row >= 0; --row)
        // {
        //     if (getCell(row, column) == Player::EMPTY)
        //         return row;
        // }
        cout << "searching column: " << column << endl;
        for (int row = 0; row < ROWS; ++row)
        {
            // cout << "found row " << row << " [";
            // for (int c = 0; c < COLS; ++c)
            // {
            //     cout << grid[row][c];
            //     if (c < COLS - 1)
            //         cout << ", ";
            // }
            // cout << "]" << endl;

            Player cell = getCell(row, column);
            if (cell == Player::EMPTY && cell != Player::PLAYER1 && cell != Player::PLAYER2)
            {
                return row;
            }
        }
        return -1;
    }
    int findRow(int column)
    {
        // Search from the bottom row up
        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (getCell(row, column) == Player::EMPTY)
                return row;
        }
        return -1; // column is full
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
    bool full() const
    {
        for (int c = 0; c < COLS; ++c)
        {
            if (columnHasSpace(static_cast<Column>(c)))
            {
                return false;
            }
        }
        return true;
    }

    /**
     * Get all possible moves for the current player
     * @param board The current state of the board
     * @return A vector of Columns representing possible moves
     */
    vector<Column> getPossibleMoves()
    {
        // Check for possible moves in the current board state
        vector<Column> validMoves;

        for (int col = 0; col < COLS; ++col)
        {
            for (int row = ROWS - 1; row >= 0; --row)
            {
                if (getCell(row, col) == Player::EMPTY)
                {
                    validMoves.push_back(static_cast<Column>(col));
                    break;
                }
            }
        }

        return validMoves;
    }

    /**
     * Generate metrics for the current layer
     * @param board The current state of the board
     * @param player The current player
     * @return An array of TileMetrics for each column
     */

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