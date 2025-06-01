#include "include.h"

Column charToColumn(char c)
{
    switch (c)
    {
    case 'a':
        return Column::A;
    case 'A':
        return Column::A;
    case 'b':
        return Column::B;
    case 'B':
        return Column::B;
    case 'c':
        return Column::C;
    case 'C':
        return Column::C;
    case 'd':
        return Column::D;
    case 'D':
        return Column::D;
    case 'e':
        return Column::E;
    case 'E':
        return Column::E;
    case 'f':
        return Column::F;
    case 'F':
        return Column::F;
    case 'g':
        return Column::G;
    case 'G':
        return Column::G;
    default:
        throw std::invalid_argument("Invalid column character");
    }
}

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

        Column col = charToColumn(move[0]);

        bool player1Won = board.dropDisc(col, player);
        if (player1Won)
        {
            cout << "Player 1 won!" << endl;
            break;
        }
        tree->moveRootUp(board, col);
        tree->grow(board, opponent, player, 1);
        tree->toDot();
        tree->dotToSvg();
        // Column bestMove0 = brain.getBestMove(board, opponent, GameTheorie::Level::EASY, true);

        board.print();

        // player 2

        cout << "Enter your move (A-G) or 'exit' to quit: " << endl;

        getline(cin, move);
        Column bestMove = charToColumn(move[0]);

        bool player2Won = board.dropDisc(bestMove, opponent);

        if (player2Won)
        {
            cout << "Player 2 won!" << endl;
            break;
        }

        tree->moveRootUp(board, bestMove);
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
