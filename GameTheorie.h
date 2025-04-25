#ifndef GAMETHEORIE_H
#define GAMETHEORIE_H

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include "Connect4Board.h"

using namespace std;

class TileContent
{
public:
    Connect4Board::Cell OWNER;
    int PRESSURE;
    bool THREAD;
    bool WINCHANCE;
    int MINOR_THREADS;
    int MINOR_WINCHANCE;
    bool AUTOMATIC_WIN;

    TileContent()
        : OWNER(Connect4Board::EMPTY),
          PRESSURE(0),
          THREAD(false),
          WINCHANCE(false),
          MINOR_THREADS(0),
          MINOR_WINCHANCE(0),
          AUTOMATIC_WIN(false)
    {
    }

    bool operator==(const TileContent &other) const
    {
        return OWNER == other.OWNER &&
               PRESSURE == other.PRESSURE &&
               THREAD == other.THREAD &&
               WINCHANCE == other.WINCHANCE &&
               MINOR_THREADS == other.MINOR_THREADS &&
               MINOR_WINCHANCE == other.MINOR_WINCHANCE &&
               AUTOMATIC_WIN == other.AUTOMATIC_WIN;
    }
};

class GameTheorie
{
public:
    static constexpr int ROWS = Connect4Board::ROWS;
    static constexpr int COLS = Connect4Board::COLS;

    std::array<std::array<TileContent, COLS>, ROWS> grid;

    GameTheorie()
    {
        for (auto &row : grid)
        {
            row.fill(TileContent());
        }
    }

    Column getBestMove(Connect4Board board)
    {
        std::vector<Column> possibleMoves = getPossibleMoves(board);
        if (possibleMoves.empty())
        {
            throw std::runtime_error("No possible moves available.");
        }

        std::vector<int> pressure = computePressureSum(board, Connect4Board::PLAYER2); // computePressure(Connect4Board::PLAYER2, board);

        std::cout << "\nSpanning per kolom voor speler 2:\n";
        for (int c = 0; c < Connect4Board::COLS; ++c)
        {
            char col = 'A' + c;
            if (pressure[c] < 0)
                std::cout << col << ": kolom vol\n";
            else
                std::cout << col << ": " << pressure[c] << "\n";
        }

        int bestPressure = -1;
        Column bestMove = possibleMoves.front();
        for (Column col : possibleMoves)
        {
            int p = pressure[col];
            if (p > bestPressure)
            {
                bestPressure = p;
                bestMove = col;
            }
        }

        // TODO: implement best-move logic here
        std::cout << "Possible moves: ";
        for (const auto &move : possibleMoves)
        {
            std::cout << move << " ";
        }
        std::cout << std::endl;
        return bestMove; // Placeholder: return the first possible move
    }

    std::vector<Column> getPossibleMoves(Connect4Board board)
    {
        // Check for possible moves in the current board state
        std::vector<Column> validMoves;

        for (int col = 0; col < COLS; ++col)
        {
            for (int row = ROWS - 1; row >= 0; --row)
            {
                if (board.getCell(row, col) == Connect4Board::EMPTY)
                {
                    validMoves.push_back(static_cast<Column>(col));
                    break;
                }
            }
        }

        return validMoves;
    }
    /*
    loop through all possible tiles(lowest tile for each column that is empty(0))
    for each tile check its 4 neighbours left, right, up, down, upright, upleft, downright, downleft.
    using that calculate how many win options for player there are that include the selected tile.
    then return for each tile how many tiles are within this reach from the player, that help create win positions, but if there is en opponent tile between the selected tile and the other tile, the other tile wont count.
    */

    const int dr[4] = {0, 1, 1, 1};
    const int dc[4] = {1, 0, 1, -1};

    vector<int> computeWinOptionsPerColumn(const Connect4Board &board,
                                           Connect4Board::Cell player)
    {
        // bepaal wie de opponent is
        auto opponent = (player == Connect4Board::PLAYER1
                             ? Connect4Board::PLAYER2
                             : Connect4Board::PLAYER1);

        vector<int> options(Connect4Board::COLS, 0);

        // 4 basisrichtingen: rechts, omhoog, diag-up-right, diag-up-left
        static constexpr int dr[4] = {0, 1, 1, 1};
        static constexpr int dc[4] = {1, 0, 1, -1};

        for (int c = 0; c < Connect4Board::COLS; ++c)
        {
            // 1) vind de speelbare tegel (laagste lege rij) in kolom c
            int r_play = -1;
            for (int r = Connect4Board::ROWS - 1; r >= 0; --r)
            {
                if (board.getCell(r, c) == Connect4Board::EMPTY)
                {
                    r_play = r;
                    break;
                }
            }
            if (r_play < 0)
            {
                // kolom vol: geen opties
                options[c] = 0;
                continue;
            }

            int count = 0;
            // 2) scan alle 4-op-een-rij-vensters in 4 richtingen
            for (int dir = 0; dir < 4; ++dir)
            {
                // we laten het raam “sliden” over G door offset 0..3
                for (int off = 0; off < 4; ++off)
                {
                    int sr = r_play - dr[dir] * off;
                    int sc = c - dc[dir] * off;

                    // 2.a) check of heel window binnen bord en vrij van opponent
                    bool ok = true;
                    for (int i = 0; i < 4; ++i)
                    {
                        int rr = sr + dr[dir] * i;
                        int cc = sc + dc[dir] * i;
                        if (rr < 0 || rr >= Connect4Board::ROWS ||
                            cc < 0 || cc >= Connect4Board::COLS ||
                            board.getCell(rr, cc) == opponent)
                        {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok)
                        continue;

                    // 2.b) check dat G echt in dat window zit (offset garandeert dat,
                    // maar we check defensief):
                    bool coversG = false;
                    for (int i = 0; i < 4; ++i)
                    {
                        if (sr + dr[dir] * i == r_play &&
                            sc + dc[dir] * i == c)
                        {
                            coversG = true;
                            break;
                        }
                    }
                    if (!coversG)
                        continue;

                    //   *** als we hier zijn: het is een geldige win-optie ***
                    count++;
                }
            }

            options[c] = count;
        }

        return options;
    }

    vector<int> computePressureSum(const Connect4Board &board,
                                   Connect4Board::Cell player)
    {
        // bepaal opponent
        auto opponent = (player == Connect4Board::PLAYER1
                             ? Connect4Board::PLAYER2
                             : Connect4Board::PLAYER1);

        vector<int> pressure(Connect4Board::COLS, -1);

        // 4 basisrichtingen (horizontaal, verticaal, diag up-right, diag up-left)
        static constexpr int dr[4] = {0, 1, 1, 1};
        static constexpr int dc[4] = {1, 0, 1, -1};

        for (int c = 0; c < Connect4Board::COLS; ++c)
        {
            // 1) Vind speelbare tegel (laagste lege rij) in kolom c
            int r_play = -1;
            for (int r = Connect4Board::ROWS - 1; r >= 0; --r)
            {
                if (board.getCell(r, c) == Connect4Board::EMPTY)
                {
                    r_play = r;
                    break;
                }
            }
            if (r_play < 0)
            {
                // kolom vol
                pressure[c] = -1;
                continue;
            }

            int sumTiles = 0;
            // 2) ga over alle 4-op-een-rij vensters in 4 richtingen
            for (int dir = 0; dir < 4; ++dir)
            {
                for (int off = 0; off < 4; ++off)
                {
                    int sr = r_play - dr[dir] * off;
                    int sc = c - dc[dir] * off;

                    // a) Check window binnen bord en vrij van opponent
                    bool ok = true;
                    for (int i = 0; i < 4; ++i)
                    {
                        int rr = sr + dr[dir] * i;
                        int cc = sc + dc[dir] * i;
                        if (rr < 0 || rr >= Connect4Board::ROWS ||
                            cc < 0 || cc >= Connect4Board::COLS ||
                            board.getCell(rr, cc) == opponent)
                        {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok)
                        continue;

                    // b) Check dat de speelbare tegel erin zit
                    bool covers = false;
                    for (int i = 0; i < 4; ++i)
                    {
                        if (sr + dr[dir] * i == r_play && sc + dc[dir] * i == c)
                        {
                            covers = true;
                            break;
                        }
                    }
                    if (!covers)
                        continue;

                    // c) Tel hoeveel eigen schijven de speler al heeft in dit window (excl. G zelf)
                    int ownCount = 0;
                    for (int i = 0; i < 4; ++i)
                    {
                        int rr = sr + dr[dir] * i;
                        int cc = sc + dc[dir] * i;
                        if (rr == r_play && cc == c)
                            continue; // niet G zelf
                        if (board.getCell(rr, cc) == player)
                            ++ownCount;
                    }
                    sumTiles += ownCount;
                }
            }

            pressure[c] = sumTiles;
        }
        return pressure;
    }
};
#endif // GAMETHEORIE_H
