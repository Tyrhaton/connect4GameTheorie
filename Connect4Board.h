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

class Connect4Board
{
public:
    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;

    enum Player
    {
        EMPTY = 0,
        PLAYER1 = 1,
        PLAYER2 = 2
    };

    // enum Cell
    // {
    //     EMPTY = Player::EMPTY,
    //     PLAYER1 = Player::PLAYER1,
    //     PLAYER2 = Player::PLAYER2
    // };
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
    bool dropDisc(Column col, Player player)
    {
        if (col < 0 || col >= COLS)
            throw out_of_range("Column index out of range");
        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (grid[row][col] == EMPTY)
            {
                setCell(row, col, player);
                bool win = checkWin(player);
                return win;
            }
        }
        throw runtime_error("Column is full");
    }
    bool checkDirection(int row, int col, Player player, int dx, int dy) const
    {
        int count = 1;
        count += countInDirection(row, col, player, dx, dy);
        count += countInDirection(row, col, player, -dx, -dy);
        return count >= 4;
    }

    int countInDirection(int row, int col, Player player, int dx, int dy) const
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

    // bool checkWin(int row, int col, Player player) const
    // {
    //     return checkDirection(row, col, player, 1, 0) || checkDirection(row, col, player, 0, 1) || checkDirection(row, col, player, 1, 1) || checkDirection(row, col, player, 1, -1);
    // }

    Player getCell(int row, int col) const
    {
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
            throw out_of_range("Row or column index out of range");
        return grid[row][col];
    }
    void setCell(int row, int col, Player val)
    {
        // optionele bound-checks:
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        {
            throw out_of_range("Connect4Board::setCell: index out of range");
        }
        grid[row][col] = val;
    }
    Player getOponent(Player player) const
    {
        return (player == PLAYER1) ? PLAYER2 : PLAYER1;
    }
    bool checkWin(Connect4Board::Player player) const
    {
        // vier richtingen: horizontaal, verticaal, diag-up, diag-down
        static constexpr int dr[4] = {0, 1, 1, 1};
        static constexpr int dc[4] = {1, 0, 1, -1};

        auto inBoard = [&](int r, int c)
        {
            return r >= 0 && r < ROWS && c >= 0 && c < COLS;
        };

        // voor elke cel als start
        for (int r = 0; r < ROWS; ++r)
        {
            for (int c = 0; c < COLS; ++c)
            {
                if (getCell(r, c) != player)
                    continue;
                // in elke richting 4 op een rij checken
                for (int dir = 0; dir < 4; ++dir)
                {
                    int cnt = 1;
                    int rr = r + dr[dir], cc = c + dc[dir];
                    // tel zo lang er dezelfde speler op staat
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
    bool inBoard(int r, int c) const
    {
        return r >= 0 && r < ROWS && c >= 0 && c < COLS;
    }
    int findRow(int c)
    {
        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (getCell(row, c) == Connect4Board::Player::EMPTY)
                return row;
        }
        return -1;
    };
};

#endif // CONNECT4_BOARD_H
