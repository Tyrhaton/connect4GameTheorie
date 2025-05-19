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
    Connect4Board::Player player = Connect4Board::Player::PLAYER1;
    Connect4Board::Player opponent = board.getOponent(player);
    GameTheorie brain = GameTheorie(player, opponent);

    // Config
    bool debug = true;
    GameTheorie::Level level = GameTheorie::Level::EASY;
    // Connect4Board::Cell startingPlayer = Connect4Board::PLAYER1;

    // Preset
    // b.dropDisc(Column::B, Connect4Board::PLAYER1);
    // b.dropDisc(Column::C, Connect4Board::PLAYER2);
    // b.dropDisc(Column::D, Connect4Board::PLAYER1);
    // b.dropDisc(Column::B, Connect4Board::PLAYER2);
    // b.dropDisc(Column::D, Connect4Board::PLAYER1);
    // b.dropDisc(Column::G, Connect4Board::PLAYER2);

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
        board.print();
        if (player1Won)
        {
            cout << "Player 1 won!" << endl;
            break;
        }

        // player 2

        Column bestMove = brain.getBestMove(board, opponent, level, debug);
        bool player2Won = board.dropDisc(bestMove, opponent);
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
