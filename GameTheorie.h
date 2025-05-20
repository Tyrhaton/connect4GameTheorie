#ifndef GAMETHEORIE_H
#define GAMETHEORIE_H

#include "include.h"

// class TileContent
// {
// public:
//     Connect4Board::Player OWNER;
//     int PRESSURE;
//     bool THREAD;
//     bool WINCHANCE;
//     int MINOR_THREADS;
//     int MINOR_WINCHANCE;
//     bool AUTOMATIC_WIN;

//     TileContent()
//         : OWNER(Connect4Board::Player::EMPTY),
//           PRESSURE(0),
//           THREAD(false),
//           WINCHANCE(false),
//           MINOR_THREADS(0),
//           MINOR_WINCHANCE(0),
//           AUTOMATIC_WIN(false)
//     {
//     }

//     bool operator==(const TileContent &other) const
//     {
//         return OWNER == other.OWNER &&
//                PRESSURE == other.PRESSURE &&
//                THREAD == other.THREAD &&
//                WINCHANCE == other.WINCHANCE &&
//                MINOR_THREADS == other.MINOR_THREADS &&
//                MINOR_WINCHANCE == other.MINOR_WINCHANCE &&
//                AUTOMATIC_WIN == other.AUTOMATIC_WIN;
//     }
// };

class GameTheorie
{
public:
    static constexpr int ROWS = Connect4Board::ROWS;
    static constexpr int COLS = Connect4Board::COLS;

    Connect4Board::Player PLAYER = Connect4Board::Player::EMPTY;
    Connect4Board::Player OPPONENT = Connect4Board::Player::EMPTY;

    // array<array<TileContent, COLS>, ROWS> grid;

    static constexpr int dr4[4] = {0, 1, 1, 1};
    static constexpr int dc4[4] = {1, 0, 1, -1};

    static constexpr int dr8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static constexpr int dc8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    enum Level
    {
        EASY = 0,
        MEDIUM = 1,
        HARD = 2
    };

    GameTheorie(Connect4Board::Player player, Connect4Board::Player opponent)
    {
        // for (auto &row : grid)
        // {
        //     row.fill(TileContent());
        // }
        PLAYER = player;
        OPPONENT = opponent;
    }

    Column getBestMove(Connect4Board board, Connect4Board::Player player, GameTheorie::Level level, bool debug)
    {
        (void)level; // Unused parameter
        vector<Column> possibleMoves = getPossibleMoves(board);
        if (possibleMoves.empty())
        {
            throw runtime_error("No possible moves available.");
        }
        vector<int> pressure = countPressureSum(board, player);
        vector<int> winOptions = countWinOptions(board, player);
        vector<bool> threats = computeImmediateThreats(board, player);
        vector<bool> minorThreats = computeMinorThreats(board, player);
        vector<bool> winMoves = computeWinningMoves(board, player);

        // TODO: implement best-move logic here
        if (debug)
        {
            cout << "Possible moves: " << endl;

            for (const Column &move : possibleMoves)
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
                if (board.getCell(row, col) == Connect4Board::Player::EMPTY)
                {
                    validMoves.push_back(static_cast<Column>(col));
                    break;
                }
            }
        }

        return validMoves;
    }

    vector<int> countPressureSum(const Connect4Board &board,
                                 Connect4Board::Player player)
    {
        Connect4Board::Player opponent = board.getOponent(player);

        vector<int> pressure(COLS, -1);

        for (int column = 0; column < COLS; ++column)
        {
            // 1) Vind speelbare tegel (laagste lege rij) in kolom c
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; --row)
            {
                if (board.getCell(row, column) == Connect4Board::Player::EMPTY)
                {
                    r_play = row;
                    break;
                }
            }
            if (r_play < 0)
            {
                // kolom vol
                pressure[column] = -1;
                continue;
            }

            int sumTiles = 0;
            // 2) ga over alle 4-op-een-rij vensters in 4 richtingen
            for (int dir = 0; dir < 4; ++dir)
            {
                for (int off = 0; off < 4; ++off)
                {
                    int sr = r_play - dr4[dir] * off;
                    int sc = column - dc4[dir] * off;

                    // a) Check window binnen bord en vrij van opponent
                    bool ok = true;
                    for (int i = 0; i < 4; ++i)
                    {
                        int rr = sr + dr4[dir] * i;
                        int cc = sc + dc4[dir] * i;
                        if (rr < 0 || rr >= ROWS ||
                            cc < 0 || cc >= COLS ||
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
                        if (sr + dr4[dir] * i == r_play && sc + dc4[dir] * i == column)
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
                        int rr = sr + dr4[dir] * i;
                        int cc = sc + dc4[dir] * i;
                        if (rr == r_play && cc == column)
                            continue; // niet G zelf
                        if (board.getCell(rr, cc) == player)
                            ++ownCount;
                    }
                    sumTiles += ownCount;
                }
            }

            pressure[column] = sumTiles;
        }
        return pressure;
    }

    /**
     * return per kolom (A…G) het aantal win‐opties voor de speelbare tegel in die kolom.
     * Als kolom vol is, krijg je -1.
     */
    vector<int> countWinOptions(
        const Connect4Board &board,
        Connect4Board::Player player)
    {
        // bepaal de tegenstander
        Connect4Board::Player opponent = board.getOponent(player);

        // output per kolom
        vector<int> result(COLS, -1);

        for (int column = 0; column < COLS; ++column)
        {
            // 1) vind de speelbare tegel in kolom c (eerste lege van onder)
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; --row)
            {
                if (board.getCell(row, column) == Connect4Board::Player::EMPTY)
                {
                    r_play = row;
                    break;
                }
            }
            if (r_play < 0)
            {
                // kolom is vol
                result[column] = -1;
                continue;
            }

            int count = 0;
            // 2) scan alle 4‐lange vensters in 4 richtingen
            for (int dir = 0; dir < 4; ++dir)
            {
                for (int off = 0; off < 4; ++off)
                {
                    // schuif het raam zo dat (r_play,c) op positie 'off' ligt
                    int sr = r_play - dr4[dir] * off;
                    int sc = column - dc4[dir] * off;

                    // a) check of raam binnen bord en geen tegenstander
                    bool valid = true;
                    for (int k = 0; k < 4; ++k)
                    {
                        int rr = sr + dr4[dir] * k;
                        int cc = sc + dc4[dir] * k;
                        if (!board.inBoard(rr, cc) || board.getCell(rr, cc) == opponent)
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
                        if (sr + dr4[dir] * k == r_play && sc + dc4[dir] * k == column)
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

            result[column] = count;
        }

        return result;
    }

    /**
     * isImmediateThreat(board, opponent)
     *   Retourneert een vector van size COLS met per kolom:
     *     • true  als de tegenstander, wanneer hij in die kolom speelt, direct
     *            4-op-een-rij heeft
     *     • false anders (of kolom vol)
     */
    vector<bool> computeImmediateThreats(
        Connect4Board &board,
        Connect4Board::Player player)
    {
        Connect4Board::Player opponent = board.getOponent(player);

        vector<bool> threat(COLS, false);

        // voor elke kolom c
        for (int column = 0; column < COLS; ++column)
        {
            int row = board.findRow(column);
            if (row < 0)
            {
                // kolom vol: geen threat
                threat[column] = false;
                continue;
            }
            // simuleer tegenstander speelt hier:
            Connect4Board copy = board;
            copy.setCell(row, column, opponent);
            // check of hij gewonnen heeft
            if (copy.checkWin(opponent))
            {
                threat[column] = true;
            }
            // anders blijft false
        }

        return threat;
    }

    vector<bool> computeMinorThreats(
        const Connect4Board &board,
        Connect4Board::Player player)
    {
        Connect4Board::Player opponent = board.getOponent(player);

        vector<bool> result(COLS, false);

        // scan elke kolom
        for (int column = 0; column < COLS; column++)
        {
            // spelbare rij zoeken
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; row--)
            {
                if (board.getCell(row, column) == Connect4Board::Player::EMPTY)
                {
                    r_play = row;
                    break;
                }
            }
            if (r_play < 0)
                continue; // kolom vol => false

            // is er een 4-window dat G=r_play,c minor-threat is?
            bool isMinor = false;

            for (int dir = 0; dir < 4 && !isMinor; dir++)
            {
                for (int off = 0; off < 4 && !isMinor; off++)
                {
                    int sr = r_play - dr4[dir] * off;
                    int sc = column - dc4[dir] * off;
                    if (!board.inBoard(sr, sc))
                        continue;
                    // tel in dit window
                    int countOp = 0, countEmp = 0, countMe = 0;
                    bool covers = false;
                    for (int k = 0; k < 4; k++)
                    {
                        int rr = sr + dr4[dir] * k,
                            cc = sc + dc4[dir] * k;
                        if (!board.inBoard(rr, cc))
                        {
                            countMe = 1;
                            break; // ongeldig
                        }
                        Connect4Board::Player cell = board.getCell(rr, cc);
                        if (rr == r_play && cc == column)
                        {
                            covers = true;
                            countEmp++;
                        }
                        else if (cell == Connect4Board::Player::EMPTY)
                        {
                            countEmp++;
                        }
                        else if (cell == opponent)
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
                    cc = column + dc8[k];
                if (board.inBoard(rr, cc) && board.getCell(rr, cc) == opponent)
                {
                    adj = true;
                }
            }
            result[column] = adj;
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
        Connect4Board::Player player)
    {
        vector<bool> result(COLS, false);

        // voor elke kolom
        for (int column = 0; column < COLS; column++)
        {
            // 1) vind de speelbare rij in kolom c
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; row--)
            {
                if (board.getCell(row, column) == Connect4Board::Player::EMPTY)
                {
                    r_play = row;
                    break;
                }
            }
            if (r_play < 0)
            {
                // kolom vol => niet winnend
                result[column] = false;
                continue;
            }

            // 2) simuleer de zet
            board.setCell(r_play, column, player);

            // 3) check direct vier-op-een-rij
            bool win = false;

            for (int dir = 0; dir < 4 && !win; dir++)
            {
                int count = 1;
                // beide kanten van (r_play,c) uitchecken
                for (int dsign = -1; dsign <= 1; dsign += 2)
                {
                    int rr = r_play + dr4[dir] * dsign;
                    int cc = column + dc4[dir] * dsign;
                    while (board.inBoard(rr, cc) && board.getCell(rr, cc) == player)
                    {
                        count++;
                        rr += dr4[dir] * dsign;
                        cc += dc4[dir] * dsign;
                    }
                }
                if (count >= 4)
                    win = true;
            }

            result[column] = win;

            // 4) undo de zet
            board.setCell(r_play, column, Connect4Board::Player::EMPTY);
        }

        return result;
    }
};
#endif // GAMETHEORIE_H
