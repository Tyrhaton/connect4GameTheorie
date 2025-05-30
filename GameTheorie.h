#ifndef GAMETHEORIE_H
#define GAMETHEORIE_H

#include "include.h"
#include <limits>
#include <functional>
#include <fstream>
#include <sstream>
#include "tree.h"

class GameTheorie
{
public:
    static constexpr int ROWS = Connect4Board::ROWS;
    static constexpr int COLS = Connect4Board::COLS;

    Player PLAYER = Player::EMPTY;
    Player OPPONENT = Player::EMPTY;

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
    struct TileMetrics
    {
        int pressure;
        int winOptions;
        bool immediateThreat;
        bool minorThreat;
        bool winningMove;
    };

    GameTheorie(Player player = Player::PLAYER1, Player opponent = Player::PLAYER2)
    {
        // for (auto &row : grid)
        // {
        //     row.fill(TileContent());
        // }
        PLAYER = player;
        OPPONENT = opponent;
    }

    /**
     * Generate metrics for the current layer
     * @param board The current state of the board
     * @param player The current player
     * @return An array of TileMetrics for each column
     */
    array<TileMetrics, 7> generateMetricsForLayer(Connect4Board &board, Player player)
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

    // void test(Connect4Board &board, Player player, Player opponent)
    // {

    //     Connect4Board currentBoard = board;

    //     for (int column = 0; column < 7; ++column)
    //     {
    //         Connect4Board copy = board;
    //         copy.dropDisc(static_cast<Column>(column), player);

    //         array<TileMetrics, 7> metricsMe = generateMetricsForLayer(copy, opponent);
    //         for (int i = 0; i < 7; ++i)
    //         {

    //             Connect4Board copy2 = copy;
    //             copy2.dropDisc(static_cast<Column>(i), opponent);

    //             array<TileMetrics, 7> metricsMe = generateMetricsForLayer(copy2, player);
    //         }
    //     }
    // }
    // Check if a column can accept a disc
    inline bool canPlay(const Connect4Board &board, int col)
    {

        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (board.getCell(row, col) == Player::EMPTY)
            {
                return true;
                break;
            }
        }
        return false;
    }

    // Determine if board is full (no more moves)
    bool boardFull(const Connect4Board &board)
    {
        for (int c = 0; c < Connect4Board::COLS; ++c)
            if (canPlay(board, c))
                return false;
        return true;
    }

    /**
     * Get the best move for the current player
     * @param board The current state of the board
     * @param player The current player
     * @return The best move as a Column
     */
    Column getBestMove(Connect4Board board, Player player, GameTheorie::Level level = EASY, bool debug = false)
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

    /**
     * Get all possible moves for the current player
     * @param board The current state of the board
     * @return A vector of Columns representing possible moves
     */
    vector<Column> getPossibleMoves(Connect4Board board)
    {
        // Check for possible moves in the current board state
        vector<Column> validMoves;

        for (int col = 0; col < COLS; ++col)
        {
            for (int row = ROWS - 1; row >= 0; --row)
            {
                if (board.getCell(row, col) == Player::EMPTY)
                {
                    validMoves.push_back(static_cast<Column>(col));
                    break;
                }
            }
        }

        return validMoves;
    }

    /**
     * Count the pressure sum for each column
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of pressure values for each column
     */
    vector<int> countPressureSum(const Connect4Board &board,
                                 Player player)
    {
        Player opponent = board.getOponent(player);

        vector<int> pressure(COLS, -1);

        for (int column = 0; column < COLS; ++column)
        {
            // 1) Vind speelbare tegel (laagste lege rij) in kolom c
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; --row)
            {
                if (board.getCell(row, column) == Player::EMPTY)
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
     * Count the number of winning options for each column
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of win option counts for each column, returning -1 for full columns
     */
    vector<int> countWinOptions(
        const Connect4Board &board,
        Player player)
    {
        Player opponent = board.getOponent(player);

        vector<int> result(COLS, -1);

        for (int column = 0; column < COLS; ++column)
        {
            // 1) vind de speelbare tegel in kolom c (eerste lege van onder)
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; --row)
            {
                if (board.getCell(row, column) == Player::EMPTY)
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
     * Compute immediate threats for the current player
     * @param board The current state of the board
     * @param player The current player
     * @return A vector of booleans indicating immediate threats for each column
     */
    vector<bool> computeImmediateThreats(
        Connect4Board &board,
        Player player)
    {
        Player opponent = board.getOponent(player);

        vector<bool> threat(COLS, false);

        for (int column = 0; column < COLS; ++column)
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
    vector<bool> computeMinorThreats(
        const Connect4Board &board,
        Player player)
    {
        Player opponent = board.getOponent(player);

        vector<bool> result(COLS, false);

        for (int column = 0; column < COLS; column++)
        {
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; row--)
            {
                if (board.getCell(row, column) == Player::EMPTY)
                {
                    r_play = row;
                    break;
                }
            }
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
    vector<bool> computeWinningMoves(
        Connect4Board board,
        Player player)
    {
        vector<bool> result(COLS, false);

        for (int column = 0; column < COLS; column++)
        {
            int r_play = -1;
            for (int row = ROWS - 1; row >= 0; row--)
            {
                if (board.getCell(row, column) == Player::EMPTY)
                {
                    r_play = row;
                    break;
                }
            }
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

    void generateTree(
        Connect4Board &board,
        Player player,
        Player opponent,
        int depth)
    {

        TreeNode *root = new TreeNode("Root");

        Tree tree = Tree(root);

        addLayer(board, player, opponent, depth, root);

        tree.print();
        tree.toDot("tree.dot");
    }
bool addLayer(
    Connect4Board &board,
    Player player,
    Player opponent,
    int depth,
    TreeNode *parent)
{
    if (depth <= 0 || boardFull(board)) return true;

    auto moves = getPossibleMoves(board);
    std::vector<TreeNode*> children;

    // First pass: generate children, but do not delete parent here
    for (const Column &col : moves) {
        // Skip full columns
        if (board.findRow(col) < 0) continue;

        Connect4Board copy = board;
        copy.dropDisc(col, player);

        // Check if opponent can win after this move in any column
        bool allowsOpponentWin = false;
        for (const Column &oppCol : getPossibleMoves(copy)) {
            Connect4Board after = copy;
            after.dropDisc(oppCol, opponent);
            if (after.checkWin(opponent)) {
                allowsOpponentWin = true;
                break;
            }
        }
        if (allowsOpponentWin) continue;  // skip this move entirely

        // Create child node
        TreeNode *child = new TreeNode("Col " + std::to_string(col),
                                       player == PLAYER1 ? 1 : 2);
        if (copy.checkWin(player)) {
            child->win = true;
            child->label += " (win)";
        }
        children.push_back(child);
    }

    // If any winning child, prune siblings
    bool anyWin = std::any_of(children.begin(), children.end(), [](TreeNode* c){ return c->win; });
    if (anyWin) {
        for (TreeNode *c : children) {
            if (c->win)
                parent->addChild(c);
            else
                delete c;
        }
        return true;
    }

    // Attach and recurse
    for (TreeNode *c : children) {
        parent->addChild(c);
        Connect4Board nextBoard = board;
        nextBoard.dropDisc(static_cast<Column>(c->label.back() - '0'), player);
        addLayer(nextBoard, opponent, player, depth - 1, c);
    }

    return true;
}

};
#endif // GAMETHEORIE_H
