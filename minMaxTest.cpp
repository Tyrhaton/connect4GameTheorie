#include "include.h"

int main()
{
    // Initialisation
    cout << "Starting game theorie" << endl;

    Connect4Board board;
    Player player = Player::PLAYER1;
    Player opponent = board.getOponent(player);
    GameTheorie brain = GameTheorie(board, player, opponent);

    Tree *tree = brain.tree;
    // tree.toDot();
    // tree.dotToSvg();

    bool run = true;

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

        bool player1Won = board.dropDisc(col, player);
        if (player1Won)
        {
            cout << "Player 1 won!" << endl;
            break;
        }
        tree->moveRootUp(col);
        tree->grow(board, opponent, player, 1);
        tree->toDot();
        tree->dotToSvg();
        // Column bestMove0 = brain.getBestMove(board, opponent, GameTheorie::Level::EASY, true);

        board.print();

        // player 2

        cout << "Enter your move (A-G) or 'exit' to quit: " << endl;

        getline(cin, move);
        Column bestMove = Connect4Board::charToColumn(move[0]);

        bool player2Won = board.dropDisc(bestMove, opponent);

        if (player2Won)
        {
            cout << "Player 2 won!" << endl;
            break;
        }

        tree->moveRootUp(bestMove);
        tree->grow(board, player, opponent, 1);
        tree->toDot();
        tree->dotToSvg();
        // Column bestMove2 = brain.getBestMove(board, opponent, GameTheorie::Level::EASY, true);

        board.print();

        // for (const Column &col : board.getPossibleMoves())
        // {
        //     int row = board.findRow(col);
        //     int p = GameTheorie::getTilePressure(board, player, row, col);
        //     cout << "Pressure p " << colToChar(col) << ": " << p << endl;
        // }
    }
    cout << "Game over!" << endl;

    return 0;
}
