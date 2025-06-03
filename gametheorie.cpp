#include "include.h"

int main()
{
    // Initialisation
    cout << "Starting game theorie" << endl;


    // Config
    bool debug = true;
    bool run = true;

    Connect4Board initBoard;
    Player startingPlayer = Player::PLAYER1; // PLAYER1 (user) or PLAYER2 (system)
    Player opponentPlayer = initBoard.getOponent(startingPlayer);

    GameTheorie::Level level = GameTheorie::Level::EASY;

    GameTheorie brain = GameTheorie(initBoard, startingPlayer, 1, level); // depth 4 should compile fast enough, 5 or higher will be slow
    Connect4Board board = brain.getBoard();                              
    Tree *tree = brain.tree;

    board.print();

    string move;
    while (run)
    {

        cout << "Enter your move (A-G) or 'exit' to quit: " << endl;
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
        // Column bestMove = brain.getBestMoveV2();
        Column test = brain.getBestMoveV2();
        cout << "Best move for player 2: " << Connect4Board::colToChar(test) << endl;

        cout << "Enter your move (A-G) or 'exit' to quit: " << endl;
        getline(cin, move);
        Column col2 = Connect4Board::charToColumn(move[0]);

        bool player2Won = board.dropDisc(col2, opponentPlayer);
        tree->updateTree(board, col2);

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
