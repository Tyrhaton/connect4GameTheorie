#include "include.h"

int main()
{
    // Initialisation
    cout << "Starting game theorie" << endl;

    // Config
    bool debug = false;
    bool run = true;
    bool letBotPlay = false;     // if false, the user plays both players
    int depth = 8;               // depth of the game tree, higher values will take longer to compute, depth=4 should compile fast enough, 5 or higher will be slow
    bool advancedPruning = true; // if true, the bot will use advanced pruning techniques to speed up adding layers to the game tree, using this will allow for depth=7 for still fast and depth=8 for still decent timing
    Connect4Board initBoard;
    Player startingPlayer = Player::USER; // USER (user) or BOT (system)
    Player opponentPlayer = initBoard.getOponent(startingPlayer);

    Level level = Level::HARD; // EASY, MEDIUM, HARD

    GameTheorie brain = GameTheorie(initBoard, startingPlayer, depth, level, advancedPruning);
    Connect4Board board = brain.getBoard();

    board.print();

    string move;
    while (run)
    {
        if (startingPlayer == Player::USER)
        {
            cout << "User: Enter your move (A-G) or 'exit' to quit: " << endl;
            getline(cin, move);
            if (move == "exit" || move == "Exit" || move == "quit" || move == "q")
            {
                run = false;
                break;
            }
            // player 1

            Column col = Connect4Board::charToColumn(move[0]);

            bool userWon = brain.playMove(col, startingPlayer);
            brain.printBoard();
            if (userWon)
            {
                cout << "User won!" << endl;
                break;
            }

            // player 2
            Column bestMove = brain.getBestMove(level, debug);
            cout << "Best move for bot: " << Connect4Board::colToChar(bestMove) << endl;

            if (!letBotPlay)
            {
                cout << "Best move for user: " << Connect4Board::colToChar(bestMove) << endl;
                cout << "Bot: Enter your move (A-G) or 'exit' to quit: " << endl;
                getline(cin, move);
                bestMove = Connect4Board::charToColumn(move[0]);
            }

            bool botWon = brain.playMove(bestMove, opponentPlayer);

            brain.printBoard();
            if (botWon)
            {
                cout << "Bot won!" << endl;
                break;
            }
        }
        else
        {

            // player 2
            Column bestMove = brain.getBestMove(level, debug);

            if (!letBotPlay)
            {
                cout << "Best move for user: " << Connect4Board::colToChar(bestMove) << endl;
                cout << "Bot: Enter your move (A-G) or 'exit' to quit: " << endl;
                getline(cin, move);
                bestMove = Connect4Board::charToColumn(move[0]);
            }

            bool botWon = brain.playMove(bestMove, opponentPlayer);

            brain.printBoard();
            if (botWon)
            {
                cout << "Bot won!" << endl;
                break;
            }

            cout << "User: Enter your move (A-G) or 'exit' to quit: " << endl;
            getline(cin, move);
            if (move == "exit" || move == "Exit" || move == "quit" || move == "q")
            {
                run = false;
                break;
            }
            // player 1

            Column col = Connect4Board::charToColumn(move[0]);

            bool userWon = brain.playMove(col, startingPlayer);
            brain.printBoard();
            if (userWon)
            {
                cout << "User won!" << endl;
                break;
            }
        }
        if (brain.getBoard().full())
        {
            cout << "The board is full, it's a draw!" << endl;
            break;
        }
    }
    cout << "Game over!" << endl;

    return 0;
}
