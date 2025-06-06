#include "include.h"

int main()
{
    // Initialisation
    cout << "Starting game theorie" << endl;

    // Config
    bool debug = false;
    bool run = true;
    bool letBotPlay = true; // if false, the user plays both players
    int depth = 2;          // depth of the game tree, higher values will take longer to compute, depth 4 should compile fast enough, 5 or higher will be slow

    Connect4Board initBoard;
    Player startingPlayer = Player::PLAYER1; // PLAYER1 (user) or PLAYER2 (system)
    Player opponentPlayer = initBoard.getOponent(startingPlayer);

    GameTheorie::Level level = GameTheorie::Level::EASY;

    GameTheorie brain = GameTheorie(initBoard, startingPlayer, depth, level);
    Connect4Board board = brain.getBoard();

    board.print();

    string move;
    while (run)
    {
        cout << "Player1: Enter your move (A-G) or 'exit' to quit: " << endl;
        getline(cin, move);
        if (move == "exit" || move == "Exit" || move == "quit" || move == "q")
        {
            run = false;
            break;
        }
        // player 1

        Column col = Connect4Board::charToColumn(move[0]);

        bool player1Won = brain.playMove(col, startingPlayer);
        brain.printBoard();
        if (player1Won)
        {
            cout << "Player 1 won!" << endl;
            break;
        }

        // player 2
        Column bestMove = brain.getBestMove(opponentPlayer);

        if (!letBotPlay)
        {
            cout << "Best move for player 2: " << Connect4Board::colToChar(bestMove) << endl;
            cout << "Player2: Enter your move (A-G) or 'exit' to quit: " << endl;
            getline(cin, move);
            bestMove = Connect4Board::charToColumn(move[0]);
        }

        bool player2Won = brain.playMove(bestMove, opponentPlayer);

        brain.printBoard();
        if (player2Won)
        {
            cout << "Player 2 won!" << endl;
            break;
        }
    }
    cout << "Game over!" << endl;

    return 0;
}
