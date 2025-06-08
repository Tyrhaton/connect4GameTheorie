#ifndef MOVE_RECORDER_H
#define MOVE_RECORDER_H

#include "include.h"

struct Move
{
    Player player;
    Column column;
    int moveNumber;
    Move(Player p, Column c, int n) : player(p), column(c), moveNumber(n) {}
};

class MoveRecorder
{
private:
    vector<Move> history;

public:
    void recordMove(Player player, Column column)
    {
        int moveNumber = history.size() + 1;
        history.emplace_back(player, column, moveNumber);
    }

    void clear()
    {
        history.clear();
    }

    const vector<Move> &getHistory() const
    {
        return history;
    }

    void print() const
    {
        for (size_t i = 0; i < history.size(); ++i)
        {
            const Move &m = history[i];
            char colChar = static_cast<char>('A' + static_cast<int>(m.column));
            cout << "Move " << (i + 1) << ": Player "
                 << (m.player == Player::USER ? "U" : "B")
                 << " -> " << colChar << "\n";
        }
    }
};

#endif
