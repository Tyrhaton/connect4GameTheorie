#include "include.h"

int main()
{
    // Initialisation
    cout << "Starting game theorie" << endl;

    // Config
    bool debug = true;
    bool run = true;
    bool letBotPlay = true; // if false, the user plays both players
    int depth = 2; // depth of the game tree, higher values will take longer to compute, depth 4 should compile fast enough, 5 or higher will be slow

    Connect4Board initBoard;
    Player startingPlayer = Player::PLAYER1; // PLAYER1 (user) or PLAYER2 (system)
    Player opponentPlayer = initBoard.getOponent(startingPlayer);

    GameTheorie::Level level = GameTheorie::Level::EASY;

    GameTheorie brain = GameTheorie(initBoard, startingPlayer, depth, level);
    Connect4Board board = brain.getBoard();
    Tree *tree = brain.tree;

    board.print();

    string move;
    while (run)
    {
        if (debug)
        {
            vector<Column> moves = board.getPossibleMoves();

            for (const Column &column : moves)
            {

                Connect4Board copy = board;
                int row = copy.findRow(column);
                if (row < 0)
                {
                    continue;
                }
                TileMetrics metrics = Metrics::generateMetricsForTile(copy, startingPlayer, row, column);

                cout << "Metrics for column " << Connect4Board::colToChar(column) << to_string(board.ROWS - row) << ": " << "Pressure: " << metrics.pressure << " Win Options: " << metrics.winOptions << " Immediate Threat: " << (metrics.immediateThreat ? "Yes" : "No") << " Minor Threat: " << (metrics.minorThreat ? "Yes" : "No") << " Winning Move: " << (metrics.winningMove ? "Yes" : "No") << endl;
            }
        }

        cout << "Player1: Enter your move (A-G) or 'exit' to quit: " << endl;
        getline(cin, move);
        if (move == "exit" || move == "Exit" || move == "quit" || move == "q")
        {
            run = false;
            break;
        }
        // player 1

        Column col = Connect4Board::charToColumn(move[0]);
        bool player1Won = board.dropDisc(col, startingPlayer);
        tree->updateTree(board, col);

        board.print();
        if (player1Won)
        {
            cout << "Player 1 won!" << endl;
            break;
        }

        // player 2
        // Column bestMove = brain.getBestMove(board, opponentPlayer, level, debug);
        Column bestMove = brain.getBestMoveV2(board);

        if (debug)
        {
            vector<Column> moves2 = board.getPossibleMoves();
            for (const Column &column : moves2)
            {
                Connect4Board copy = board;
                int row = copy.findRow(column);
                if (row < 0)
                {
                    continue;
                }
                TileMetrics metrics = Metrics::generateMetricsForTile(copy, opponentPlayer, row, column);

                cout << "Metrics for column " << Connect4Board::colToChar(column) << to_string(board.ROWS - row) << ": " << "Pressure: " << metrics.pressure << " Win Options: " << metrics.winOptions << " Immediate Threat: " << (metrics.immediateThreat ? "Yes" : "No") << " Minor Threat: " << (metrics.minorThreat ? "Yes" : "No") << " Winning Move: " << (metrics.winningMove ? "Yes" : "No") << endl;
            }
        }

        if (!letBotPlay)
        {
            cout << "Best move for player 2: " << Connect4Board::colToChar(bestMove) << endl;
            cout << "Player2: Enter your move (A-G) or 'exit' to quit: " << endl;
            getline(cin, move);
            bestMove = Connect4Board::charToColumn(move[0]);
        }

        bool player2Won = board.dropDisc(bestMove, opponentPlayer);
        tree->updateTree(board, bestMove);

        board.print();
        if (player2Won)
        {
            cout << "Player 2 won!" << endl;
            break;
        }
    }
    cout << "Game over!" << endl;

    return 0;
}
