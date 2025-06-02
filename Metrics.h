#ifndef METRICS_H
#define METRICS_H

#include "include.h"

// The minimal signature for generateMetricsForTile that everyone can see:
struct TileMetrics
{
    int pressure;
    int winOptions;
    bool immediateThreat;
    bool minorThreat;
    bool winningMove;
};

class Metrics
{
public:
    static constexpr int dr4[4] = {0, 1, 1, 1};
    static constexpr int dc4[4] = {1, 0, 1, -1};

    static constexpr int dr8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static constexpr int dc8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    /**
     * Count the pressure sum for each column
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of pressure values for each column
     */
    static vector<int> countPressureSum(Connect4Board &board,
                                        Player player)
    {
        Player opponent = board.getOponent(player);

        vector<int> pressure(board.COLS, -1);

        for (int column = 0; column < board.COLS; ++column)
        {
            int r_play = board.findRow(column);
            if (r_play < 0)
            {
                pressure[column] = -1;
                continue;
            }

            int sumTiles = 0;
            for (int dir = 0; dir < 4; ++dir)
            {
                for (int off = 0; off < 4; ++off)
                {
                    int sr = r_play - dr4[dir] * off;
                    int sc = column - dc4[dir] * off;

                    bool ok = true;
                    for (int i = 0; i < 4; ++i)
                    {
                        int rr = sr + dr4[dir] * i;
                        int cc = sc + dc4[dir] * i;
                        if (rr < 0 || rr >= board.ROWS ||
                            cc < 0 || cc >= board.COLS ||
                            board.getCell(rr, cc) == opponent)
                        {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok)
                        continue;

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

                    int ownCount = 0;
                    for (int i = 0; i < 4; ++i)
                    {
                        int rr = sr + dr4[dir] * i;
                        int cc = sc + dc4[dir] * i;
                        if (rr == r_play && cc == column)
                        {
                            continue;
                        }
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
     * Count the number of winning options for each column
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of win option counts for each column, returning -1 for full columns
     */
    static vector<int> countWinOptions(
        Connect4Board &board,
        Player player)
    {
        Player opponent = board.getOponent(player);

        vector<int> result(board.COLS, -1);

        for (int column = 0; column < board.COLS; ++column)
        {
            int r_play = board.findRow(column);
            if (r_play < 0)
            {
                result[column] = -1;
                continue;
            }

            int count = 0;
            for (int dir = 0; dir < 4; ++dir)
            {
                for (int off = 0; off < 4; ++off)
                {
                    int sr = r_play - dr4[dir] * off;
                    int sc = column - dc4[dir] * off;

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

                    ++count;
                }
            }

            result[column] = count;
        }

        return result;
    }

    /**
     * Compute immediate threats for the current player
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of booleans indicating immediate threats for each column
     */
    static vector<bool> computeImmediateThreats(
        Connect4Board &board,
        Player player)
    {
        Player opponent = board.getOponent(player);

        vector<bool> threat(board.COLS, false);

        for (int column = 0; column < board.COLS; ++column)
        {
            int row = board.findRow(column);
            if (row < 0)
            {
                threat[column] = false;
                continue;
            }
            Connect4Board copy = board;
            copy.setCell(row, column, opponent);
            if (copy.checkWin(opponent))
            {
                threat[column] = true;
            }
        }

        return threat;
    }

    /**
     * Compute minor threats for the current player
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of booleans indicating minor threats for each column
     */
    static vector<bool> computeMinorThreats(
        Connect4Board &board,
        Player player)
    {
        Player opponent = board.getOponent(player);

        vector<bool> result(board.COLS, false);

        for (int column = 0; column < board.COLS; column++)
        {
            int r_play = board.findRow(column);
            if (r_play < 0)
                continue;

            bool isMinor = false;

            for (int dir = 0; dir < 4 && !isMinor; dir++)
            {
                for (int off = 0; off < 4 && !isMinor; off++)
                {
                    int sr = r_play - dr4[dir] * off;
                    int sc = column - dc4[dir] * off;
                    if (!board.inBoard(sr, sc))
                        continue;
                    int countOp = 0, countEmp = 0, countMe = 0;
                    bool covers = false;
                    for (int k = 0; k < 4; k++)
                    {
                        int rr = sr + dr4[dir] * k,
                            cc = sc + dc4[dir] * k;
                        if (!board.inBoard(rr, cc))
                        {
                            countMe = 1;
                            break;
                        }
                        Player cell = board.getCell(rr, cc);
                        if (rr == r_play && cc == column)
                        {
                            covers = true;
                            countEmp++;
                        }
                        else if (cell == Player::EMPTY)
                        {
                            countEmp++;
                        }
                        else if (cell == opponent)
                        {
                            countOp++;
                        }
                        else
                        {
                            countMe++;
                        }
                    }
                    if (covers && countOp == 2 && countMe == 0 && countEmp == 2)
                    {
                        isMinor = true;
                    }
                }
            }

            if (!isMinor)
                continue;

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
     * Compute winning moves for the current player
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of booleans indicating winning moves for each column
     */
    static vector<bool> computeWinningMoves(
        Connect4Board board,
        Player player)
    {
        vector<bool> result(board.COLS, false);

        for (int column = 0; column < board.COLS; column++)
        {
            int r_play = board.findRow(column);
            if (r_play < 0)
            {
                result[column] = false;
                continue;
            }

            board.setCell(r_play, column, player);

            bool win = false;

            for (int dir = 0; dir < 4 && !win; dir++)
            {
                int count = 1;
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

            board.setCell(r_play, column, Player::EMPTY);
        }

        return result;
    }

    /**
     * Get the "pressure" for a single empty cell at (r_play, column)
     * @param board The current state of the board
     * @param player The current player
     * @param r_play The row index of the tile to analyze
     * @param column The column index of the tile to analyze
     * @return The pressure value, or -1 if (r_play,column) is off-board.
     */
    static int getTilePressure(const Connect4Board &board, Player player,
                               int r_play, Column column)
    {
        Player opponent = board.getOponent(player);
        if (r_play < 0 || r_play >= board.ROWS || column < 0 || column >= board.COLS)
        {
            return -1;
        }
        int sum = 0;

        for (int dir = 0; dir < 4; ++dir)
        {
            // forward direction
            for (int step = 1; step < 4; ++step)
            {
                int rr = r_play + dr4[dir] * step;
                int cc = column + dc4[dir] * step;
                if (rr < 0 || rr >= board.ROWS || cc < 0 || cc >= board.COLS)
                    break;
                Player c = board.getCell(rr, cc);
                // cout << "Checking cell (" << rr << ", " << cc << ") = " << static_cast<int>(c) << endl;
                if (c == player)
                {
                    ++sum;
                }
                else if (c == opponent)
                {
                    break; // blocked by opponent
                }
                else
                {
                    // empty: ignore but continue scanning
                    continue;
                }
            }
            // backward direction
            for (int step = 1; step < 4; ++step)
            {
                int rr = r_play - dr4[dir] * step;
                int cc = column - dc4[dir] * step;
                if (rr < 0 || rr >= board.ROWS || cc < 0 || cc >= board.COLS)
                    break;
                Player c = board.getCell(rr, cc);
                // cout << "Checking cell (" << rr << ", " << cc << ") = " << static_cast<int>(c) << endl;

                if (c == player)
                {
                    ++sum;
                }
                else if (c == opponent)
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
        }
        return sum;
    }

    /**
     * Check if the given tile is a winning move for the player
     * @param board The current state of the board
     * @param player The current player
     * @param r_play The row index of the tile to analyze
     * @param column The column index of the tile to analyze
     * @return True if the tile is a winning move, false otherwise
     */
    static int getTileWinOptions(const Connect4Board &board,
                                 Player player,
                                 int r_play,
                                 int column)
    {
        Player opponent = board.getOponent(player);
        // Check that (r_play, column) is within bounds and actually empty:
        if (r_play < 0 || r_play >= board.ROWS ||
            column < 0 || column >= board.COLS ||
            board.getCell(r_play, column) != Player::EMPTY)
        {
            return -1;
        }

        int count = 0;

        for (int dir = 0; dir < 4; ++dir)
        {
            // Slide a 4‐cell window so that (r_play, column) occupies each offset [0..3]
            for (int off = 0; off < 4; ++off)
            {
                int sr = r_play - dr4[dir] * off;
                int sc = column - dc4[dir] * off;

                // a) Verify the 4‐cell window is fully on‐board and contains no opponent discs
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

                // b) Ensure (r_play, column) is indeed within this window
                bool covers = false;
                for (int k = 0; k < 4; ++k)
                {
                    if (sr + dr4[dir] * k == r_play &&
                        sc + dc4[dir] * k == column)
                    {
                        covers = true;
                        break;
                    }
                }
                if (!covers)
                    continue;

                // c) If we reach here, this window is a valid potential win‐option
                ++count;
            }
        }

        return count;
    }

    /**
     * Check if the given tile is a threat for the opponent
     * @param board The current state of the board
     * @param player The current player
     * @param r_play The row index of the tile to analyze
     * @param column The column index of the tile to analyze
     * @return True if the tile is a threat, false otherwise
     */
    static bool getTileThreat(const Connect4Board &board, Player player, int r_play, Column column)
    {
        Player opponent = board.getOponent(player);
        // Check that (r_play, column) is within bounds and actually empty:
        if (r_play < 0 || r_play >= board.ROWS ||
            column < 0 || column >= board.COLS ||
            board.getCell(r_play, column) != Player::EMPTY)
        {
            return false;
        }

        // Check for immediate threats
        Connect4Board copy = board;
        copy.setCell(r_play, column, opponent);
        return copy.checkWin(opponent);
    }

    /**
     * Check if the given tile is a minor threat for the opponent
     * @param board The current state of the board
     * @param player The current player
     * @param r_play The row index of the tile to analyze
     * @param column The column index of the tile to analyze
     * @return True if the tile is a minor threat, false otherwise
     */
    static bool getTileMinorThreat(const Connect4Board &board,
                                   Player player,
                                   int r_play,
                                   int column)
    {
        Player opponent = board.getOponent(player);
        if (!board.inBoard(r_play, column) ||
            board.getCell(r_play, column) != Player::EMPTY)
        {
            return false;
        }

        bool isMinor = false;
        for (int dir = 0; dir < 4 && !isMinor; ++dir)
        {
            for (int off = 0; off < 4 && !isMinor; ++off)
            {
                int sr = r_play - dr4[dir] * off;
                int sc = column - dc4[dir] * off;

                int countOp = 0, countEmp = 0, countMe = 0;
                bool covers = false;
                for (int k = 0; k < 4; ++k)
                {
                    int rr = sr + dr4[dir] * k;
                    int cc = sc + dc4[dir] * k;
                    if (!board.inBoard(rr, cc))
                    {
                        countMe = 1;
                        break;
                    }
                    Player cell = board.getCell(rr, cc);
                    if (rr == r_play && cc == column)
                    {
                        covers = true;
                        ++countEmp;
                    }
                    else if (cell == Player::EMPTY)
                    {
                        ++countEmp;
                    }
                    else if (cell == opponent)
                    {
                        ++countOp;
                    }
                    else
                    {
                        ++countMe;
                    }
                }
                if (covers && countOp == 2 && countMe == 0 && countEmp == 2)
                {
                    isMinor = true;
                }
            }
        }
        if (!isMinor)
        {
            return false;
        }

        for (int k = 0; k < 8; ++k)
        {
            int rr = r_play + dr8[k];
            int cc = column + dc8[k];
            if (board.inBoard(rr, cc) && board.getCell(rr, cc) == opponent)
            {
                return true;
            }
        }
        return false;
    }

    static bool getTileWinningMove(const Connect4Board &board, Player player, int r_play, Column column)
    {
        if (!board.inBoard(r_play, column) ||
            board.getCell(r_play, column) != Player::EMPTY)
        {
            return false;
        }

        Connect4Board copy = board;
        copy.setCell(r_play, column, player);
        return copy.checkWin(player);
    }

    /**
     * Generate metrics for a specific tile in the Connect 4 board
     * @param board The current state of the board
     * @param player The current player
     * @param r_play The row index of the tile to analyze
     * @param column The column index of the tile to analyze
     * @return A TileMetrics object containing various metrics for the tile
     */
    static TileMetrics generateMetricsForTile(
        Connect4Board &board, Player player, int r_play, Column column)
    {
        TileMetrics metrics;
        metrics.pressure = getTilePressure(board, player, r_play, column);
        if (metrics.pressure < 0)
        {
            return metrics;
        }

        // Check win options
        // metrics.winOptions = countWinOptions(board, player)[column];
        metrics.winOptions = getTileWinOptions(board, player, r_play, column);
        // cout << "Win options for column " << Connect4Board::colToChar(column) << ": " << metrics.winOptions << ":" << countWinOptions(board, player)[column] << endl;

        // Check immediate threat
        // metrics.immediateThreat = copy.checkWin(opponent);
        metrics.immediateThreat = getTileThreat(board, player, r_play, column);

        // Check minor threat
        // metrics.minorThreat = computeMinorThreats(board, player)[column];
        metrics.minorThreat = getTileMinorThreat(board, player, r_play, column);

        // Check winning move
        metrics.winningMove = getTileWinningMove(board, player, r_play, column);

        return metrics;
    }

    static array<TileMetrics, 7> generateMetricsForLayer(Connect4Board &board, Player player)
    {
        vector<int> pressure = countPressureSum(board, player);
        vector<int> winOptions = countWinOptions(board, player);
        vector<bool> threats = computeImmediateThreats(board, player);
        vector<bool> minorThreats = computeMinorThreats(board, player);
        vector<bool> winMoves = computeWinningMoves(board, player);

        array<TileMetrics, 7> metrics;

        for (int col = 0; col < 7; ++col)
        {
            metrics[col] = TileMetrics{
                pressure[col],
                winOptions[col],
                threats[col],
                minorThreats[col],
                winMoves[col]};
        }
        return metrics;
    }
};
#endif // METRICS_H
