#ifndef CONNECT4_BOARD_H
#define CONNECT4_BOARD_H

#include "include.h"

class Connect4Board
{
public:
    /**
     * Constants for the dimensions of the Connect 4 board.
     * ROWS = 6, COLS = 7
     */
    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;

    /**
     * Direction vectors for checking winning conditions.
     * These represent the 4 possible directions: horizontal, vertical, diagonal down-right, and diagonal down-left.
     */
    static constexpr int dr[4] = {0, 1, 1, 1};
    static constexpr int dc[4] = {1, 0, 1, -1};

    /**
     * Enumeration for the different marks on the board.
     */
    enum Mark
    {
        EMPTYMARK = '.',
        BOTMARK = 'X',
        USERMARK = 'O'
    };

    /**
     * Enumeration for the columns of the Connect 4 board.
     * Each column is represented by a character from A to G.
     */
    enum Column
    {
        A = 0,
        B = 1,
        C = 2,
        D = 3,
        E = 4,
        F = 5,
        G = 6,
        INVALID = -1
    };

    /**
     * Convert a character to a Column enumeration.
     * @param column The character representing the column (A-G).
     * @return The corresponding Column enumeration.
     */
    static Column charToColumn(char column)
    {
        switch (column)
        {
        case 'a':
            return Column::A;
        case 'A':
            return Column::A;
        case 'b':
            return Column::B;
        case 'B':
            return Column::B;
        case 'c':
            return Column::C;
        case 'C':
            return Column::C;
        case 'd':
            return Column::D;
        case 'D':
            return Column::D;
        case 'e':
            return Column::E;
        case 'E':
            return Column::E;
        case 'f':
            return Column::F;
        case 'F':
            return Column::F;
        case 'g':
            return Column::G;
        case 'G':
            return Column::G;
        default:
            throw invalid_argument("Invalid column character");
        }
    }

    /**
     * Convert a character to a Column enumeration.
     * @param column The character representing the column (A-G).
     * @return The corresponding Column enumeration.
     */
    static string colToChar(Column column)
    {
        switch (column)
        {
        case Column::A:
            return string(1, 'A');
        case Column::B:
            return string(1, 'B');
        case Column::C:
            return string(1, 'C');
        case Column::D:
            return string(1, 'D');
        case Column::E:
            return string(1, 'E');
        case Column::F:
            return string(1, 'F');
        case Column::G:
            return string(1, 'G');
        case Column::INVALID:
            return string(1, '?');
        default:
            cout << "Invalid column: " << column << endl;
            throw invalid_argument("Invalid column");
        }
    }

    /**
     * Enumeration for the players in the game.
     * EMPTY represents an empty cell, BOT and USER represent the two players.
     */
    enum Player
    {
        EMPTY = 0,
        BOT = 1,
        USER = 2
    };

    /**
     * Convert a Player enumeration to a character representation.
     * @param player The Player enumeration.
     * @return The corresponding character representation.
     */
    static string playerToChar(Player player)
    {
        switch (player)
        {
        case Player::EMPTY:
            return string(1, '.');
        case Player::BOT:
            return string(1, 'X');
        case Player::USER:
            return string(1, 'O');
        default:
            throw invalid_argument("Invalid player");
        }
    }

    /**
     * The grid representing the Connect 4 board.
     * Each cell can be EMPTY, BOT, or USER.
     */
    array<array<Player, COLS>, ROWS> grid;

    /**
     * Constructor for the Connect4Board class.
     * Initializes the board to an empty state.
     */
    Connect4Board()
    {
        for (auto &row : grid)
        {
            row.fill(Player::EMPTY);
        }
    }

    /**
     * Print the current state of the board.
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
                if (grid[r][c] == BOT)
                {
                    ch = BOTMARK;
                }
                else if (grid[r][c] == USER)
                {
                    ch = USERMARK;
                }
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
        {
            throw out_of_range("Column index out of range (" + colToChar(column) + ")");
        }
        int row = findRow(column);
        if (row < 0)
        {
            return false;
        }
        setCell(row, column, player);

        return checkWin(player);
    }

    /**
     * Get the current state of a cell in the grid.
     * @param row The row index.
     * @param col The column index.
     * @return The Player at the specified cell.
     */
    Player getCell(int row, int col) const
    {
        if (!inBoard(row, col, true))
        {
            cout << "Connect4Board::getCell: Row or column index out of range" << endl;
            // throw out_of_range("Connect4Board::getCell: Row or column index out of range");
        }
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
        if (!inBoard(row, column, true))
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
        return (player == BOT) ? USER : BOT;
    }

    /**
     * Check if the specified player has won the game.
     * @param player The player to check for a win.
     * @return True if the player has won, false otherwise.
     */
    bool checkWin(Player player) const
    {
        for (int r = 0; r < ROWS; ++r)
        {
            for (int c = 0; c < COLS; ++c)
            {
                if (getCell(r, c) != player)
                {
                    continue;
                }
                for (int dir = 0; dir < 4; ++dir)
                {
                    int cnt = 1;
                    int rr = r + dr[dir], cc = c + dc[dir];
                    while (inBoard(rr, cc) && getCell(rr, cc) == player)
                    {
                        ++cnt;
                        if (cnt == 4)
                        {
                            return true;
                        }
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
    bool inBoard(int r, int c, bool debug = false) const
    {
        if (debug)
        {
            if (r < 0 || r >= ROWS)
            {
                cerr << "Row index out of bounds: " << r << endl;
            }
            if (c < 0 || c >= COLS)
            {
                cerr << "Column index out of bounds: " << c << endl;
            }
        }
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

    /**
     * Check if the specified column has space for a new disc.
     * @param column The column to check.
     * @return True if there is space in the column, false otherwise.
     */
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
     * Check if the board is full (no empty cells).
     * @return True if the board is full, false otherwise.
     */
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