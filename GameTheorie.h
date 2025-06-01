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
    Connect4Board *board = nullptr;

    /**
     * The current player
     */
    Player PLAYER = Player::EMPTY;
    Player OPPONENT = Player::EMPTY;

    /**
     * Enumeration for the difficulty levels of the game theory AI
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
     * The level of the game theory AI
     * EASY: Basic heuristics
     * MEDIUM: More advanced heuristics
     * HARD: Full tree search with pruning
     */
    Level level = Level::EASY;

    /**
     * Default constructor for GameTheorie
     * Initializes the game theory with a default board and players
     */
    GameTheorie(Connect4Board &board, Player player = Player::PLAYER1, Player opponent = Player::PLAYER2,
                int depth = 2, Level level = Level::EASY)
        : PLAYER(player), OPPONENT(opponent), level(level)
    {
        (void)depth; // Unused parameter
        this->board = &board;
        tree = new Tree(board, player, opponent, depth);
        tree->toDot();
        tree->dotToSvg();
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
        vector<Column> possibleMoves = board.getPossibleMoves();
        if (possibleMoves.empty())
        {
            throw runtime_error("No possible moves available.");
        }
        vector<int> pressure = Metrics::countPressureSum(board, player);
        vector<int> winOptions = Metrics::countWinOptions(board, player);
        vector<bool> threats = Metrics::computeImmediateThreats(board, player);
        vector<bool> minorThreats = Metrics::computeMinorThreats(board, player);
        vector<bool> winMoves = Metrics::computeWinningMoves(board, player);

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
     * Set the board and player for this game theory instance
     * @param newBoard The new Connect4Board instance
     */
    void setBoard(Connect4Board &newBoard)
    {
        board = &newBoard;
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
     * Get the current board
     * @return The Connect4Board instance
     */
    Connect4Board &getBoard() const
    {
        return *board;
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
};

#endif // GAMETHEORIE_H
