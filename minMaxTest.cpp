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
    GameTheorie brain = GameTheorie(player, opponent);

    board.dropDisc(Column::C, Player::PLAYER2);
    board.dropDisc(Column::B, Player::PLAYER1);
    board.dropDisc(Column::C, Player::PLAYER2);
    board.dropDisc(Column::B, Player::PLAYER1);
    board.dropDisc(Column::C, Player::PLAYER2);

    board.dropDisc(Column::D, Player::PLAYER1);
    // board.dropDisc(Column::D, Player::PLAYER1);
    board.dropDisc(Column::D, Player::PLAYER1);
    board.dropDisc(Column::G, Player::PLAYER1);
    board.dropDisc(Column::G, Player::PLAYER1);
    board.dropDisc(Column::C, Player::PLAYER1);
    board.dropDisc(Column::D, Player::PLAYER2);
    // board.dropDisc(Column::G, Player::PLAYER1);
    // board.dropDisc(Column::D, Player::PLAYER1);

    // board.print();
    // board.dropDisc(Column::B, Player::PLAYER1);
    // board.dropDisc(Column::B, Player::PLAYER1);
    // int best = brain.getBestMoveVisual(board, player, opponent, 3, "tree.dot");

    // TreeNode *root = new TreeNode("Root");

    // Tree tree = Tree(root);

    // TreeNode *child = new TreeNode("Child",1);
    // TreeNode *child2 = new TreeNode("Child2",2);
    // root->addChild(child);
    // root->addChild(child2);
    // TreeNode *grandChild = new TreeNode("GrandChild");
    // child->addChild(grandChild);
    // for (int i = 0; i < 7; ++i)
    // {
    //     TreeNode *child = new TreeNode("Child" + std::to_string(i), 1);
    //     root->addChild(child);

    //     for (int j = 0; j < 7; ++j)
    //     {
    //         TreeNode *grandChild = new TreeNode("GrandChild" + std::to_string(j), 2);
    //         child->addChild(grandChild);

    //         for (int k = 0; k < 7; ++k)
    //         {
    //             TreeNode *grandGrandChild = new TreeNode("GrandGrandChild" + std::to_string(k), 1);
    //             grandChild->addChild(grandGrandChild);
    //         }
    //     }
    // }

    // tree.print();
    brain.generateTree(board, player, opponent, 4);
    board.print();

    // tree.toDot("tree.dot");

    // Config
    // bool debug = true;
    // GameTheorie::Level level = GameTheorie::Level::EASY;
    // Connect4Board::Cell startingPlayer = Connect4Board::PLAYER1;

    // Preset
    // board.dropDisc(Column::B, Player::PLAYER1);
    // board.dropDisc(Column::C, Player::PLAYER2);
    // board.dropDisc(Column::D, Player::PLAYER1);
    // board.dropDisc(Column::B, Player::PLAYER2);
    // board.dropDisc(Column::D, Player::PLAYER1);
    // board.dropDisc(Column::G, Player::PLAYER2);

    // bool run = true;

    // string move;
    // while (run)
    // {

    //     cout << "Enter your move (A-G) or 'exit' to quit: " << endl;
    //     getline(cin, move);
    //     if (move == "exit" || move == "Exit" || move == "quit" || move == "q")
    //     {
    //         run = false;
    //         break;
    //     }
    //     // player 1

    //     Column col = charToColumn(move[0]);
    //     bool player1Won = board.dropDisc(col, player);
    //     board.print();
    //     if (player1Won)
    //     {
    //         cout << "Player 1 won!" << endl;
    //         break;
    //     }

    //     // player 2

    //     Column bestMove = brain.getBestMove(board, opponent, level, debug);
    //     bool player2Won = board.dropDisc(bestMove, opponent);
    //     board.print();
    //     if (player2Won)
    //     {
    //         cout << "Player 2 won!" << endl;
    //         break;
    //     }
    // }
    cout << "Game over!" << endl;

    return 0;
}
