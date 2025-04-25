#include <iostream>
#include "Connect4Board.h"
#include "GameTheorie.h"
#include <string>
using namespace std;

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
    cout << "Starting game theorie" << endl;

    Connect4Board b;
    GameTheorie g;

    b.dropDisc(Column::B, Connect4Board::PLAYER1);
    b.dropDisc(Column::C, Connect4Board::PLAYER2);
    b.dropDisc(Column::D, Connect4Board::PLAYER1);
    b.dropDisc(Column::B, Connect4Board::PLAYER2);
    b.dropDisc(Column::D, Connect4Board::PLAYER1);
    b.dropDisc(Column::G, Connect4Board::PLAYER2);

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

        Column col = charToColumn(move[0]);
        b.dropDisc(col, Connect4Board::PLAYER1);
        b.print();
        Column bestMove = g.getBestMove(b);
        b.dropDisc(bestMove, Connect4Board::PLAYER2);
        b.print();
    }
    cout << "Game over!" << endl;

    return 0;
}
