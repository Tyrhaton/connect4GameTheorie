#include "include.h"

int main()
{
    // Initialisation
    cout << "Starting game theorie" << endl;

    Connect4Board initBoard;

    // initBoard.dropDisc(Column::B, Player::PLAYER1);
    // initBoard.dropDisc(Column::B, Player::PLAYER1);
    // initBoard.dropDisc(Column::B, Player::PLAYER1);

    Player player = Player::PLAYER1;
    Player opponent = initBoard.getOponent(player);
    GameTheorie::Level level = GameTheorie::Level::EASY;

    GameTheorie brain = GameTheorie(initBoard, player, opponent, 2, level);
    Connect4Board board = brain.getBoard(); // Use the initial board state
    // board.dropDisc(Column::D, Player::PLAYER1);

    board.print();
    // int best = brain.getBestMoveWithVisualization(board, player, opponent, 3, "tree.dot");

    // Config
    bool debug = true;
    // Connect4Board::Cell startingPlayer = Connect4Board::PLAYER1;

    // Preset
    // board.dropDisc(Column::B, Player::PLAYER1);
    // board.dropDisc(Column::C, Player::PLAYER2);
    // board.dropDisc(Column::D, Player::PLAYER1);
    // board.dropDisc(Column::B, Player::PLAYER2);
    // board.dropDisc(Column::D, Player::PLAYER1);
    // board.dropDisc(Column::G, Player::PLAYER2);

    bool run = true;

    string move;
    while (run)
    {

        brain.getBestMoveV2(player);

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
