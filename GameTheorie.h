#ifndef GAMETHEORIE_H
#define GAMETHEORIE_H

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
// #include "Connect4Board.h"

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

    array<array<TileContent, COLS>, ROWS> grid;

    enum Level
    {
        EASY = 0,
        MEDIUM = 1,
        HARD = 2
    };

    GameTheorie()
    {
        for (auto &row : grid)
        {
            row.fill(TileContent());
        }
    }

    Column getBestMove(Connect4Board board, Connect4Board::Cell player,  GameTheorie::Level level, bool debug)
    {
        vector<Column> possibleMoves = getPossibleMoves(board);
        if (possibleMoves.empty())
        {
            throw runtime_error("No possible moves available.");
        }
        vector<int> pressure = computePressureSum(board, player); // computePressure(Connect4Board::PLAYER2, board);
        vector<int> winOptions = countWinOptionsPerColumn(board, player);
        vector<bool> threats = isImmediateThreat(board, Connect4Board::PLAYER1);

        // TODO: implement best-move logic here
        if (debug)
        {
            cout << "Possible moves: " << endl;

            for (const auto &move : possibleMoves)
            {
                cout << static_cast<char>('A' + move) << " ";
            }
            cout << endl
                 << endl;

            cout << "\nSpanning per kolom voor speler 2:" << endl;

            for (int c = 0; c < Connect4Board::COLS; ++c)
            {
                if (pressure[c] < 0)
                    cout << "X ";
                else
                    cout << pressure[c] << " ";
            }

            cout << endl
                 << endl;

            cout << "Aantal win-opties per kolom voor speler 2:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (winOptions[c] < 0)
                    cout << "X ";
                else
                    cout << winOptions[c] << " ";
            }
            cout << endl;
            cout << endl;
            cout << "Threat:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (threats[c] == true)
                    cout << "T ";
                else
                    cout << "X ";
            }
            cout << endl;
            cout << endl;
        }

        // level 0

        int bestPressure = -1;
        int bestWinOptions = -1;
        Column bestMove = possibleMoves.front();
        for (Column col : possibleMoves)
        {
            if (threats[col])
            {
                // opponent has a threat
                return col;
            }
            int p = winOptions[col];
            if (p > bestWinOptions)
            {
                bestWinOptions = p;
                bestPressure = pressure[col];
                bestMove = col;
            }
            else if (p == bestWinOptions)
            {
                // check pressure
                if (pressure[col] > bestPressure)
                {
                    bestPressure = pressure[col];
                    bestMove = col;
                }
            }
        }

        // level 1

        return bestMove; // Placeholder: return the first possible move
    }

    vector<Column> getPossibleMoves(Connect4Board board)
    {
        // Check for possible moves in the current board state
        vector<Column> validMoves;

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

    /**
     * return per kolom (A…G) het aantal win‐opties voor de speelbare tegel in die kolom.
     * Als kolom vol is, krijg je -1.
     */
    vector<int> countWinOptionsPerColumn(
        const Connect4Board &board,
        Connect4Board::Cell player)
    {
        // bepaal de tegenstander
        auto opponent = (player == Connect4Board::PLAYER1
                             ? Connect4Board::PLAYER2
                             : Connect4Board::PLAYER1);

        // output per kolom
        vector<int> result(Connect4Board::COLS, -1);

        // hulp‐lambda om binnen bord te checken
        auto inBoard = [&](int r, int c)
        {
            return r >= 0 && r < Connect4Board::ROWS && c >= 0 && c < Connect4Board::COLS;
        };

        // de vier richtingen (dr, dc)
        static constexpr int dr[4] = {0, 1, 1, -1};
        static constexpr int dc[4] = {1, 0, 1, 1};

        for (int c = 0; c < Connect4Board::COLS; ++c)
        {
            // 1) vind de speelbare tegel in kolom c (eerste lege van onder)
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
                // kolom is vol
                result[c] = -1;
                continue;
            }

            int count = 0;
            // 2) scan alle 4‐lange vensters in 4 richtingen
            for (int dir = 0; dir < 4; ++dir)
            {
                for (int off = 0; off < 4; ++off)
                {
                    // schuif het raam zo dat (r_play,c) op positie 'off' ligt
                    int sr = r_play - dr[dir] * off;
                    int sc = c - dc[dir] * off;

                    // a) check of raam binnen bord en geen tegenstander
                    bool valid = true;
                    for (int k = 0; k < 4; ++k)
                    {
                        int rr = sr + dr[dir] * k;
                        int cc = sc + dc[dir] * k;
                        if (!inBoard(rr, cc) || board.getCell(rr, cc) == opponent)
                        {
                            valid = false;
                            break;
                        }
                    }
                    if (!valid)
                        continue;

                    // b) check dat (r_play,c) er echt in zit
                    bool covers = false;
                    for (int k = 0; k < 4; ++k)
                    {
                        if (sr + dr[dir] * k == r_play && sc + dc[dir] * k == c)
                        {
                            covers = true;
                            break;
                        }
                    }
                    if (!covers)
                        continue;

                    // c) dit raam telt als één win‐optie
                    ++count;
                }
            }

            result[c] = count;
        }

        return result;
    }
    bool checkWin(Connect4Board board, Connect4Board::Cell player) const
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
                if (board.getCell(r, c) != player)
                    continue;
                // in elke richting 4 op een rij checken
                for (int dir = 0; dir < 4; ++dir)
                {
                    int cnt = 1;
                    int rr = r + dr[dir], cc = c + dc[dir];
                    // tel zo lang er dezelfde speler op staat
                    while (inBoard(rr, cc) && board.getCell(rr, cc) == player)
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
    } /**
       * isImmediateThreat(board, opponent)
       *   Retourneert een vector van size COLS met per kolom:
       *     • true  als de tegenstander, wanneer hij in die kolom speelt, direct
       *            4-op-een-rij heeft
       *     • false anders (of kolom vol)
       */
    vector<bool> isImmediateThreat(
        const Connect4Board &board,
        Connect4Board::Cell opponent)
    {
        int R = Connect4Board::ROWS;
        int C = Connect4Board::COLS;
        vector<bool> threat(C, false);

        // helper om speelbare rij in kolom c te vinden
        auto findRow = [&](int c)
        {
            for (int r = R - 1; r >= 0; --r)
            {
                if (board.getCell(r, c) == Connect4Board::EMPTY)
                    return r;
            }
            return -1;
        };

        // voor elke kolom c
        for (int c = 0; c < C; ++c)
        {
            int r = findRow(c);
            if (r < 0)
            {
                // kolom vol: geen threat
                threat[c] = false;
                continue;
            }
            // simuleer tegenstander speelt hier:
            Connect4Board copy = board;
            copy.setCell(r, c, opponent);
            // check of hij gewonnen heeft
            if (checkWin(copy, opponent))
            {
                threat[c] = true;
            }
            // anders blijft false
        }

        return threat;
    }
};
#endif // GAMETHEORIE_H
