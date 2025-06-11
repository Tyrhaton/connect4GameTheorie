#ifndef METRICS_H
#define METRICS_H

#include "include.h"
struct TileMetrics
{
    int pressure;
    int winOptions;
    bool immediateThreat;
    bool minorThreat;
    bool winningMove;
    int preferredWinningRow;
    bool enablesOpponentThreat;
};

class Metrics
{
private:
    /**
     * Direction vectors for 4 and 8 directions
     */
    static constexpr int dr4[4] = {0, 1, 1, 1};
    static constexpr int dc4[4] = {1, 0, 1, -1};

    static constexpr int dr8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static constexpr int dc8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    static constexpr int DIRECTIONS[4][2] = {
        {0, 1}, // horizontal
        {1, 0}, // vertical
        {1, 1}, // diagonal down-right
        {1, -1} // diagonal down-left
    };

public:
    /**
     * Count the pressure sum for each column
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of pressure values for each column
     */
    static vector<int> countPressureSum(
        Connect4Board &board,
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
                    {
                        continue;
                    }
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
                    {
                        continue;
                    }
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
                    {
                        continue;
                    }
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
                    {
                        continue;
                    }
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
            {
                continue;
            }
            bool isMinor = false;

            for (int dir = 0; dir < 4 && !isMinor; dir++)
            {
                for (int off = 0; off < 4 && !isMinor; off++)
                {
                    int sr = r_play - dr4[dir] * off;
                    int sc = column - dc4[dir] * off;
                    if (!board.inBoard(sr, sc))
                    {
                        continue;
                    }
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
            {
                continue;
            }
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
    static int getTilePressure(
        const Connect4Board &board,
        Player player,
        int r_play,
        Column column)
    {
        Player opponent = board.getOponent(player);
        if (!board.inBoard(r_play, column) ||
            board.getCell(r_play, column) != Player::EMPTY)
        {
            return -1;
        }
        int sum = 0;

        for (int dir = 0; dir < 4; ++dir)
        {
            for (int step = 1; step < 4; ++step)
            {
                int rr = r_play + dr4[dir] * step;
                int cc = column + dc4[dir] * step;
                if (!board.inBoard(rr, cc))
                {
                    break;
                }
                Player c = board.getCell(rr, cc);
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
    static int getTileWinOptions(
        const Connect4Board &board,
        Player player,
        int r_play,
        int column)
    {
        Player opponent = board.getOponent(player);
        if (!board.inBoard(r_play, column) ||
            board.getCell(r_play, column) != Player::EMPTY)
        {
            return -1;
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
                {
                    continue;
                }
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
                {
                    continue;
                }
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
    static bool getTileThreat(
        const Connect4Board &board,
        Player player,
        int r_play,
        Column column)
    {
        Player opponent = board.getOponent(player);
        if (r_play < 0 || r_play >= board.ROWS ||
            column < 0 || column >= board.COLS ||
            board.getCell(r_play, column) != Player::EMPTY)
        {
            return false;
        }

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
    static bool getTileMinorThreat(
        const Connect4Board &board,
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

        for (int dir = 0; dir < 4; ++dir)
        {
            for (int off = 0; off < 4; ++off)
            {
                int sr = r_play - dr4[dir] * off;
                int sc = column - dc4[dir] * off;

                bool covers = false;
                int coverIndex = -1;
                int oppCount = 0;
                bool windowOK = true;

                for (int k = 0; k < 4; ++k)
                {
                    int rr = sr + dr4[dir] * k;
                    int cc = sc + dc4[dir] * k;
                    if (!board.inBoard(rr, cc))
                    {
                        windowOK = false;
                        break;
                    }
                    Player cell = board.getCell(rr, cc);
                    if (rr == r_play && cc == column)
                    {
                        covers = true;
                        coverIndex = k;
                    }
                    if (cell == opponent)
                    {
                        ++oppCount;
                    }
                    else if (cell != Player::EMPTY)
                    {
                        windowOK = false;
                        break;
                    }
                }
                if (!windowOK || !covers || oppCount != 2)
                {
                    continue;
                }

                int r1 = sr + dr4[dir] * 1, c1 = sc + dc4[dir] * 1;
                int r2 = sr + dr4[dir] * 2, c2 = sc + dc4[dir] * 2;
                if (board.getCell(r1, c1) == opponent &&
                    board.getCell(r2, c2) == opponent)
                {
                    if (coverIndex == 0 || coverIndex == 3)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    /**
     * Check if the given tile is a winning move for the player
     * @param board The current state of the board
     * @param player The current player
     * @param r_play The row index of the tile to analyze
     * @param column The column index of the tile to analyze
     * @return True if the tile is a winning move, false otherwise
     */
    static bool getTileWinningMove(
        const Connect4Board &board,
        Player player,
        int r_play,
        Column column)
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

    static int getTilePreferredWinningRow(
        const Connect4Board &board,
        int row,
        Column col,
        Player player)
    {
        const int requiredToWin = 4;

        for (int d = 0; d < 4; ++d)
        {
            int dr = DIRECTIONS[d][0];
            int dc = DIRECTIONS[d][1];

            int count = 1;
            vector<pair<int, int>> positions;

            for (int i = 0; i < requiredToWin; ++i)
            {
                int r = row - dr * i;
                int c = col - dc * i;
                if (!board.inBoard(r, c))
                {
                    break;
                }
                Player cell = board.getCell(r, c);
                if (cell == player)
                {
                    count++;
                }
                else if (cell == Player::EMPTY)
                {
                    positions.push_back({r, c});
                    break;
                }
                else
                {
                    break;
                }
            }

            for (int i = 0; i < requiredToWin; ++i)
            {
                int r = row + dr * i;
                int c = col + dc * i;
                if (!board.inBoard(r, c))
                {
                    break;
                }
                Player cell = board.getCell(r, c);
                if (cell == player)
                {
                    count++;
                }
                else if (cell == Player::EMPTY)
                {
                    positions.push_back({r, c});
                    break;
                }
                else
                {
                    break;
                }
            }

            if (count == 3)
            {
                for (auto &pos : positions)
                {
                    int r = pos.first;
                    int c = pos.second;

                    if (board.getCell(r + 1, c) != Player::EMPTY || r == board.ROWS - 1)
                    {
                        return r;
                    }
                }
            }
        }

        return -1;
    }

    static bool getTileEnablesOpponentThreat(
        const Connect4Board &board,
        Column move,
        Player botPlayer)
    {
        Player opponent = board.getOponent(botPlayer);
        Connect4Board simBoard = board;

        int row = simBoard.findRow(move);
        if (row < 0)
        {
            return false;
        }

        simBoard.setCell(row, move, botPlayer);

        vector<Column> replyMoves = simBoard.getPossibleMoves();
        for (Column reply : replyMoves)
        {
            int r = simBoard.findRow(reply);
            if (r < 0)
            {
                continue;
            }
            simBoard.setCell(r, reply, opponent);
            bool isThreat = getTileThreat(simBoard, opponent, r, reply);

            simBoard.setCell(r, reply, Player::EMPTY); // undo

            if (isThreat)
            {
                return true; // the bot move enables an immediate threat
            }
        }

        return false;
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
        Connect4Board &board,
        Player player,
        int r_play,
        Column column,
        bool debug = false)
    {
        TileMetrics metrics = {-1, -1, false, false, false, -1, false};

        // Get the pressure for the tile
        metrics.pressure = getTilePressure(board, player, r_play, column);

        // Get the win options for the tile
        metrics.winOptions = getTileWinOptions(board, player, r_play, column);

        // Get the immediate threat for the tile
        metrics.immediateThreat = getTileThreat(board, player, r_play, column);

        // Get the minor threat for the tile
        metrics.minorThreat = getTileMinorThreat(board, player, r_play, column);

        // Get the winning move for the tile
        metrics.winningMove = getTileWinningMove(board, player, r_play, column);

        // Get the preferred winning row for the tile
        metrics.preferredWinningRow = getTilePreferredWinningRow(board, r_play, column, player);

        // Get if the tile enables an opponent threat
        metrics.enablesOpponentThreat = getTileEnablesOpponentThreat(board, column, player);

        if (debug)
        {
            cout << "Metrics for tile (" << board.ROWS - r_play << ", " << Connect4Board::colToChar(column) << "): "
                 << "Owner: " << (player == Connect4Board::BOT ? "BOT" : "USER")
                 << ", Pressure: " << metrics.pressure
                 << ", Win Options: " << metrics.winOptions
                 << ", Immediate Threat: " << (metrics.immediateThreat ? "Yes" : "No")
                 << ", Minor Threat: " << (metrics.minorThreat ? "Yes" : "No")
                 << ", Winning Move: " << (metrics.winningMove ? "Yes" : "No")
                 << ", Preferred Winning Row: " << metrics.preferredWinningRow
                 << endl;
        }

        return metrics;
    }

    static array<TileMetrics, 7> generateMetricsForLayer(
        Connect4Board &board,
        Player player)
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
                winMoves[col],
                getTilePreferredWinningRow(board, board.findRow(col), static_cast<Column>(col), player),
                getTileEnablesOpponentThreat(board, static_cast<Column>(col), player)

            };
        }
        return metrics;
    }
};
#endif // METRICS_H
