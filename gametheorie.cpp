#include "include.h"

int main()
{
    // Initialisation
    cout << "Starting game theorie" << endl;

    Connect4Board initBoard;

    // initBoard.dropDisc(Column::B, Player::PLAYER1);
    // initBoard.dropDisc(Column::B, Player::PLAYER1);
    // initBoard.dropDisc(Column::B, Player::PLAYER1);

    Player startingPlayer = Player::PLAYER1; // PLAYER1 (user) or PLAYER2 (system)
    Player opponentPlayer = initBoard.getOponent(startingPlayer);

    // Player player = Player::PLAYER1;
    // Player opponent = initBoard.getOponent(player);
    GameTheorie::Level level = GameTheorie::Level::EASY;

    GameTheorie brain = GameTheorie(initBoard, startingPlayer, 3, level);
    Connect4Board board = brain.getBoard(); // Use the initial board state
    // board.dropDisc(Column::D, Player::PLAYER1);
    Tree *tree = brain.tree;

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
        tree->moveRootUp(col);
        tree->grow(board, 1);
        tree->toDot();
        tree->dotToSvg();

        board.print();
        if (player1Won)
        {
            cout << "Player 1 won!" << endl;
            break;
        }

        // player 2
        Column bestMove = brain.getBestMove(board, opponentPlayer, level, debug);
        // Column bestMove = brain.getBestMoveV2();
        Column test = brain.getBestMoveV2();
        cout << "Best move for player 2: " << Connect4Board::colToChar(test) << endl;
        bool player2Won = board.dropDisc(test, opponentPlayer);
        tree->moveRootUp(bestMove);
        tree->grow(board, 1);
        tree->toDot();
        tree->dotToSvg();

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
