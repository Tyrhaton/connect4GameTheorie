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

    Column getBestMove(Connect4Board board, Connect4Board::Cell player, GameTheorie::Level level, bool debug)
    {
        (void)level; // Unused parameter
        vector<Column> possibleMoves = getPossibleMoves(board);
        if (possibleMoves.empty())
        {
            throw runtime_error("No possible moves available.");
        }
        vector<int> pressure = computePressureSum(board, player);
        vector<int> winOptions = countWinOptionsPerColumn(board, player);
        vector<bool> threats = isImmediateThreat(board, Connect4Board::PLAYER1);
        vector<bool> minorThreats = computeMinorThreatsBool(board, Connect4Board::PLAYER2);
        vector<bool> winMoves = computeWinningMoves(board, Connect4Board::PLAYER2);

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
            cout << "Minor Threat:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (minorThreats[c] == true)
                    cout << "t ";
                else
                    cout << "X ";
            }
            cout << endl;
            cout << endl;
            cout << "win moves:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (winMoves[c] == true)
                    cout << "y ";
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

            if (winMoves[col])
            {
                // player has a winning move
                return col;
            }
            if (threats[col])
            {
                // opponent has a threat
                return col;
            }
            if (minorThreats[col])
            {
                // opponent has a minor threat
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

        return bestMove; 
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
    }

    /**
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

    vector<bool> computeMinorThreatsBool(
        const Connect4Board &board,
        Connect4Board::Cell me)
    {
        // bepaal opponent
        Connect4Board::Cell op = (me == Connect4Board::PLAYER1
                                      ? Connect4Board::PLAYER2
                                      : Connect4Board::PLAYER1);
        int R = Connect4Board::ROWS, C = Connect4Board::COLS;

        // 8-buren
        static constexpr int dr8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
        static constexpr int dc8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

        auto inB = [&](int r, int c)
        {
            return r >= 0 && r < R && c >= 0 && c < C;
        };

        vector<bool> result(C, false);

        // scan elke kolom
        for (int c = 0; c < C; c++)
        {
            // spelbare rij zoeken
            int r_play = -1;
            for (int r = R - 1; r >= 0; r--)
            {
                if (board.getCell(r, c) == Connect4Board::EMPTY)
                {
                    r_play = r;
                    break;
                }
            }
            if (r_play < 0)
                continue; // kolom vol => false

            // is er een 4-window dat G=r_play,c minor-threat is?
            bool isMinor = false;
            // 4 basisrichtingen (horiz, vert, diag up-right, diag up-left)
            static constexpr int dr4[4] = {0, 1, 1, 1};
            static constexpr int dc4[4] = {1, 0, 1, -1};

            for (int dir = 0; dir < 4 && !isMinor; dir++)
            {
                for (int off = 0; off < 4 && !isMinor; off++)
                {
                    int sr = r_play - dr4[dir] * off;
                    int sc = c - dc4[dir] * off;
                    if (!inB(sr, sc))
                        continue;
                    // tel in dit window
                    int countOp = 0, countEmp = 0, countMe = 0;
                    bool covers = false;
                    for (int k = 0; k < 4; k++)
                    {
                        int rr = sr + dr4[dir] * k,
                            cc = sc + dc4[dir] * k;
                        if (!inB(rr, cc))
                        {
                            countMe = 1;
                            break; // ongeldig
                        }
                        auto cell = board.getCell(rr, cc);
                        if (rr == r_play && cc == c)
                        {
                            covers = true;
                            countEmp++;
                        }
                        else if (cell == Connect4Board::EMPTY)
                        {
                            countEmp++;
                        }
                        else if (cell == op)
                        {
                            countOp++;
                        }
                        else
                        { // me
                            countMe++;
                        }
                    }
                    // conditions minor threat
                    if (covers && countOp == 2 && countMe == 0 && countEmp == 2)
                    {
                        isMinor = true;
                    }
                }
            }

            if (!isMinor)
                continue;

            // check adjacency aan opponent
            bool adj = false;
            for (int k = 0; k < 8 && !adj; k++)
            {
                int rr = r_play + dr8[k],
                    cc = c + dc8[k];
                if (inB(rr, cc) && board.getCell(rr, cc) == op)
                {
                    adj = true;
                }
            }
            result[c] = adj;
        }

        return result;
    }

    /**
     * computeWinningMoves(board, player)
     *   voor elke kolom c:
     *     - vind de speelbare rij r in die kolom (de laagste lege rij)
     *     - als kolom vol: result[c] = false
     *     - anders: simuleer dat `player` daar zet en check of hij/zij wint
     *   return vector<bool>(COLS)
     */
    vector<bool> computeWinningMoves(
        Connect4Board board,
        Connect4Board::Cell player)
    {
        const int R = Connect4Board::ROWS;
        const int C = Connect4Board::COLS;
        vector<bool> result(C, false);

        // bepaal tegenstander (niet strict nodig hier)
        // auto op = (player==Connect4Board::PLAYER1 ? Connect4Board::PLAYER2 : Connect4Board::PLAYER1);

        // hulpfunctie om binnen bord te checken
        auto inB = [&](int r, int c)
        {
            return r >= 0 && r < R && c >= 0 && c < C;
        };

        // voor elke kolom
        for (int c = 0; c < C; c++)
        {
            // 1) vind de speelbare rij in kolom c
            int r_play = -1;
            for (int r = R - 1; r >= 0; r--)
            {
                if (board.getCell(r, c) == Connect4Board::EMPTY)
                {
                    r_play = r;
                    break;
                }
            }
            if (r_play < 0)
            {
                // kolom vol => niet winnend
                result[c] = false;
                continue;
            }

            // 2) simuleer de zet
            board.setCell(r_play, c, player); // of: board.grid[r_play][c]=player; afhankelijk van jouw API

            // 3) check direct vier-op-een-rij
            bool win = false;
            // 4 richtingen
            static constexpr int dr[4] = {0, 1, 1, 1};
            static constexpr int dc[4] = {1, 0, 1, -1};

            for (int dir = 0; dir < 4 && !win; dir++)
            {
                int cnt = 1;
                // beide kanten van (r_play,c) uitchecken
                for (int dsign = -1; dsign <= 1; dsign += 2)
                {
                    int rr = r_play + dr[dir] * dsign;
                    int cc = c + dc[dir] * dsign;
                    while (inB(rr, cc) && board.getCell(rr, cc) == player)
                    {
                        cnt++;
                        rr += dr[dir] * dsign;
                        cc += dc[dir] * dsign;
                    }
                }
                if (cnt >= 4)
                    win = true;
            }

            result[c] = win;

            // 4) undo de zet
            board.setCell(r_play, c, Connect4Board::EMPTY);
        }

        return result;
    }
};
#endif // GAMETHEORIE_H
