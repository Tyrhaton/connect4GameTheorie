#ifndef GAMETHEORIE_H
#define GAMETHEORIE_H

#include "include.h"
class GameTheorie
{
public:
    /**
     * Constants for the dimensions of the Connect 4 board.
     * ROWS = 6, COLS = 7
     */
    static constexpr int ROWS = Connect4Board::ROWS;
    static constexpr int COLS = Connect4Board::COLS;

    /**
     * Pointer to the game tree used for decision making
     */
    Tree *tree = nullptr;

    /**
     * Pointer to the Connect4Board instance
     */
    Connect4Board *BOARD = nullptr;

    /**
     * The current player
     */
    Player PLAYER = Player::BOT;    // The Player who we are the brains for
    Player OPPONENT = Player::USER; // The enemy

    Player STARTINGPLAYER = Player::EMPTY; // The player who starts the game
    Player CURRENTPLAYER = Player::EMPTY;  // The player who is currently playing

    /**
     * Enumeration for the difficulty levels of the game theory algorithm.
     * EASY: Basic heuristics
     * MEDIUM: More advanced heuristics
     * HARD: Full tree search with pruning
     */
    enum Level
    {
        EASY = 0,
        MEDIUM = 1,
        HARD = 2
    };

    /**
     * The level of the game theory algorithm
     * EASY: Basic heuristics
     * MEDIUM: More advanced heuristics
     * HARD: Full tree search with pruning
     */
    Level LEVEL = Level::EASY;

    /**
     * If true, the bot will use advanced pruning techniques to speed up adding layers to the game tree
     */
    bool ADVANCEDPRUNING = true;

    /**
     * Record all the played moves in the Move Recorder Instance
     */
    MoveRecorder MOVERECORDER;

    /**
     * Default constructor for GameTheorie
     * Initializes the game theory with a default board and players
     */
    GameTheorie(Connect4Board &board, Player startingPlayer = Player::BOT,
                int depth = 2, Level level = Level::EASY, bool advancedPruning = true)
        : STARTINGPLAYER(startingPlayer), CURRENTPLAYER(startingPlayer), LEVEL(level), ADVANCEDPRUNING(advancedPruning)
    {
        BOARD = &board;
        tree = new Tree(board, startingPlayer, depth, advancedPruning);
        tree->toDot();
        tree->dotToSvg();
        MOVERECORDER = MoveRecorder();
    }

    /**
     * Play a move in the specified column for the given player and update the game tree
     * @param column The column to play in
     * @param player The player making the move
     * @return true if the player won, false otherwise
     */
    bool playMove(Column column, Player player, bool debug = false)
    {
        if (!BOARD)
        {
            throw runtime_error("Board is not initialized.");
        }

        if (!tree)
        {
            throw runtime_error("Tree is not initialized.");
        }

        if (column < 0 || column >= COLS)
        {
            throw invalid_argument("Invalid column: " + to_string(column));
        }

        if (BOARD->findRow(column) < 0)
        {
            throw runtime_error("Column " + Connect4Board::colToChar(column) + " is full.");
        }

        if (debug)
        {
            cout << "Player " << (player == Player::BOT ? "Bot" : "User") << " plays in column: " << Connect4Board::colToChar(column) << endl;
        }

        bool playerWon = BOARD->dropDisc(column, player);

        tree->updateTree(*BOARD, column);

        setCurrentPlayer(BOARD->getOponent(player));

        MOVERECORDER.recordMove(player, column);
        return playerWon;
    }

    /**
     * print the current state of the board
     */
    void printBoard()
    {
        if (!BOARD)
        {
            throw runtime_error("Board is not initialized.");
        }
        BOARD->print();
    }

    /**
     * print the current state of the game tree
     */
    void printTree()
    {
        if (!tree)
        {
            throw runtime_error("Tree is not initialized.");
        }
        tree->print();
    }

    /**
     * Get the best move for the current player with the specified difficulty level
     * @param level The difficulty level
     * @param debug If true, enables debug output
     * @return The best move as a Column
     */
    Column getBestMove(GameTheorie::Level level = MEDIUM, bool debug = false)
    {
        if (ADVANCEDPRUNING)
        {
            if (!tree->ROOT->children.empty())
            {
                TreeNode *firstChild = tree->ROOT->children.front();
                return firstChild->move;
            }
        }

        if (level == EASY)
        {

            return getBestMoveEasy(debug);
        }
        else if (level == MEDIUM)
        {
            return getBestMoveMedium(debug);
        }
        else if (level == HARD)
        {
            return getBestMoveHard(debug);
        }
        else
        {
            throw invalid_argument("Invalid game theory level.");
        }
    }

    /**
     * Get the best move for the current player with Level = Easy
     * @param player The current player
     * @param debug if there should be extra output to help debugging
     * @return The best move as a Column
     */
    Column getBestMoveEasy(bool debug = false)
    {
        Player player = CURRENTPLAYER;
        vector<Column> possibleMoves = BOARD->getPossibleMoves();
        if (possibleMoves.empty())
        {
            throw runtime_error("No possible moves available.");
        }
        vector<int> pressure = Metrics::countPressureSum(*BOARD, player);
        vector<int> winOptions = Metrics::countWinOptions(*BOARD, player);
        vector<bool> threats = Metrics::computeImmediateThreats(*BOARD, player);
        vector<bool> minorThreats = Metrics::computeMinorThreats(*BOARD, player);
        vector<bool> winMoves = Metrics::computeWinningMoves(*BOARD, player);

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
                {
                    cout << "X ";
                }
                else
                {
                    cout << pressure[c] << " ";
                }
            }

            cout << endl
                 << endl;

            cout << "Aantal win-opties per kolom voor speler 2:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (winOptions[c] < 0)
                {
                    cout << "X ";
                }
                else
                {
                    cout << winOptions[c] << " ";
                }
            }
            cout << endl;
            cout << endl;
            cout << "Threat:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (threats[c] == true)
                {
                    cout << "T ";
                }
                else
                {
                    cout << "X ";
                }
            }
            cout << endl;
            cout << endl;
            cout << "Minor Threat:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (minorThreats[c] == true)
                {
                    cout << "t ";
                }
                else
                {
                    cout << "X ";
                }
            }
            cout << endl;
            cout << endl;
            cout << "win moves:" << endl;

            for (int c = 0; c < 7; ++c)
            {
                if (winMoves[c] == true)
                {
                    cout << "y ";
                }
                else
                {
                    cout << "X ";
                }
            }
            cout << endl;
            cout << endl;
        }

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
                if (pressure[col] > bestPressure)
                {
                    bestPressure = pressure[col];
                    bestMove = col;
                }
            }
        }

        return bestMove;
    }

    /**
     * Get the best move for the current player with Level = Medium
     * @param debug if there should be extra output to help debugging
     * @return The best move as a Column
     */
    Column getBestMoveMedium(bool debug = false)
    {
        if (!tree)
        {
            throw runtime_error("Tree is not initialized.");
        }

        TreeNode *root = tree->ROOT;
        if (!root)
        {
            throw runtime_error("Tree root is not initialized.");
        }

        Tree copy = *tree;
        vector<Column> possibleMoves = BOARD->getPossibleMoves();

        int bestPressure = -1;
        int bestWinOptions = -1;
        Column threatTile = Column::INVALID;
        Column minorThreatTile = Column::INVALID;
        Column bestMove = possibleMoves.front();

        for (auto child : copy.ROOT->children)
        {
            if (debug)
            {
                cout << "Child: " << Connect4Board::colToChar(child->move) << to_string(child->row)
                     << " Owner: " << (child->owner == 1 ? "Player 1" : "Player 2")
                     << " Win: " << (child->metrics.winningMove ? "True" : "False")
                     << " Threat: " << (child->metrics.immediateThreat ? "True" : "False")
                     << " Minor Threat: " << (child->metrics.minorThreat ? "True" : "False")
                     << " Win Options: " << child->metrics.winOptions
                     << " Pressure: " << child->metrics.pressure
                     << endl;
            }

            if (child->metrics.winningMove)
            {
                return child->move;
            }
            if (child->metrics.immediateThreat)
            {
                if (threatTile == Column::INVALID || child->metrics.pressure > bestPressure)
                {
                    threatTile = child->move;
                    bestPressure = child->metrics.pressure;
                }
            }
            if (child->metrics.minorThreat)
            {
                if (minorThreatTile == Column::INVALID || child->metrics.pressure > bestPressure)
                {
                    minorThreatTile = child->move;
                    bestPressure = child->metrics.pressure;
                }
            }

            int p = child->metrics.winOptions;
            if (p > bestWinOptions)
            {
                bestWinOptions = p;
                bestPressure = child->metrics.pressure;
                bestMove = child->move;
            }
            else if (p == bestWinOptions)
            {
                if (child->metrics.pressure > bestPressure)
                {
                    bestPressure = child->metrics.pressure;
                    bestMove = child->move;
                }
            }
        }
        if (threatTile != Column::INVALID)
        {
            return threatTile;
        }
        if (minorThreatTile != Column::INVALID)
        {
            return minorThreatTile;
        }

        return bestMove;
    }

    /**
     * Get the best move for the current player with Level = HARD
     * @param debug if there should be extra output to help debugging
     * @return The best move as a Column
     */
    Column getBestMoveHard(bool debug = false)
    {
        if (!tree)
        {
            throw runtime_error("Tree is not initialized.");
        }
        if (!tree->ROOT)
        {
            throw runtime_error("Tree root is not initialized.");
        }
        Tree copy = *tree;
        vector<Column> possibleMoves = BOARD->getPossibleMoves();

        Column bestMove = possibleMoves.front();
        Column threatTile = Column::INVALID;
        Column minorThreatTile = Column::INVALID;
        bool enablesOpponentThreatFound = false;

        int bestScore = -1;
        int pressure = -1;

        bool botPrefersOddWin = (STARTINGPLAYER == Player::BOT);

        for (TreeNode *child : copy.ROOT->children)
        {
            if (!child)
            {
                continue;
            }

            if (debug)
            {
                cout << "Child: " << Connect4Board::colToChar(child->move) << child->row
                     << " Owner: " << (child->owner == 1 ? "Player 1" : "Player 2")
                     << " Win: " << (child->metrics.winningMove ? "True" : "False")
                     << " Threat: " << (child->metrics.immediateThreat ? "True" : "False")
                     << " Minor Threat: " << (child->metrics.minorThreat ? "True" : "False")
                     << " Win Options: " << child->metrics.winOptions
                     << " Pressure: " << child->metrics.pressure
                     << " FutureWinRow: " << child->metrics.preferredWinningRow
                     << " EnablesOpponentThreat: " << (child->metrics.enablesOpponentThreat ? "True" : "False")
                     << endl;
            }

            if (child->metrics.winningMove)
            {
                return child->move;
            }

            if (child->metrics.immediateThreat)
            {
                bool better = false;

                if (threatTile == Column::INVALID)
                {
                    better = true;
                }
                else if (child->metrics.pressure > pressure)
                {
                    better = true;
                }
                else if (child->metrics.pressure == pressure && child->metrics.enablesOpponentThreat == false && enablesOpponentThreatFound == true)
                {
                    better = true;
                }

                if (better)
                {
                    threatTile = child->move;
                    pressure = child->metrics.pressure;
                    enablesOpponentThreatFound = child->metrics.enablesOpponentThreat;
                }
            }

            if (child->metrics.minorThreat)
            {
                bool better = false;

                if (minorThreatTile == Column::INVALID)
                {
                    better = true;
                }
                else if (child->metrics.pressure > pressure)
                {
                    better = true;
                }
                else if (child->metrics.pressure == pressure && child->metrics.enablesOpponentThreat == false && enablesOpponentThreatFound == true)
                {
                    better = true;
                }

                if (better)
                {
                    minorThreatTile = child->move;
                    pressure = child->metrics.pressure;
                    enablesOpponentThreatFound = child->metrics.enablesOpponentThreat;
                }
            }

            int bonus = 0;

            if (!child->metrics.enablesOpponentThreat)
            {
                bonus += 5;
            }

            int futureRow = child->metrics.preferredWinningRow;

            if (futureRow != -1)
            {
                bool isOdd = (futureRow % 2 == 1);
                if (isOdd == botPrefersOddWin)
                {
                    bonus += 5;
                }
            }

            int score = child->metrics.winOptions * 10 + child->metrics.pressure + bonus;

            if (score > bestScore)
            {
                bestScore = score;
                bestMove = child->move;
            }
        }

        if (threatTile != Column::INVALID)
        {
            return threatTile;
        }
        if (minorThreatTile != Column::INVALID)
        {
            return minorThreatTile;
        }
        return bestMove;
    }
    /**
     * Set the board and player for this game theory instance
     * @param newBoard The new Connect4Board instance
     */
    void setBoard(Connect4Board &newBoard)
    {
        BOARD = &newBoard;
    }

    /**
     * Set the player for this game theory instance
     * @param player The player to set
     */
    void setPlayer(Player player)
    {
        PLAYER = player;
    }

    /**
     * Set the opponent for this game theory instance
     * @param opponent The opponent to set
     */
    void setOpponent(Player opponent)
    {
        OPPONENT = opponent;
    }

    /**
     * Set the starting player for this game theory instance
     * @param player The starting player to set
     */
    void setStartingPlayer(Player player)
    {
        STARTINGPLAYER = player;
    }

    /**
     * Set the current player for this game theory instance
     * @param player The current player to set
     */
    void setCurrentPlayer(Player player)
    {
        CURRENTPLAYER = player;
    }

    /**
     * Get the current player
     * @return The current player
     */
    Player getPlayer() const
    {
        return PLAYER;
    }

    /**
     * Get the opponent player
     * @return The opponent player
     */
    Player getOpponent() const
    {
        return OPPONENT;
    }

    /**
     * Get the starting player
     * @return The starting player
     */
    Player getStartingPlayer() const
    {
        return STARTINGPLAYER;
    }

    /**
     * Get the current board
     * @return The Connect4Board instance
     */
    Connect4Board &getBoard() const
    {
        return *BOARD;
    }

    /**
     * Get the current tree
     * @return The Tree instance
     */
    Tree *getTree() const
    {
        return tree;
    }

    /**
     * Set the tree for this game theory instance
     * @param newTree The new Tree instance
     */
    void setTree(Tree *newTree)
    {
        tree = newTree;
    }

    /**
     * Print the move history
     */
    void printHistory()
    {
        MOVERECORDER.print();
    }
};

#endif // GAMETHEORIE_H
