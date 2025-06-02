#ifndef TREE_H
#define TREE_H

#include "include.h"

class TreeNode;
inline void deleteSubtree(TreeNode *node);

class TreeNode
{
public:
    Column move;
    string label;
    int level;
    int owner;
    bool win;
    TileMetrics metrics;
    int id;
    static int nextId;
    vector<TreeNode *> children;

    TreeNode(Column move_, const string label_, int level_ = 0, int owner_ = 0, bool win_ = false, TileMetrics metrics_ = {0, 0, false, false, false})
        : move(move_), label(label_), level(level_), owner(owner_), win(win_), metrics(metrics_), id(nextId++), children()
    {
    }

    // Add an existing TreeNode as child
    void addChild(TreeNode *child)
    {
        if (child)
            children.push_back(child);
    }
    // Print this node and its subtree with indentation
    void print(int depth = 0, bool root = false) const
    {
        if (root)
        {
            cout << "Root Node: " << label << endl;
        }
        else
        {

            for (int i = 0; i < depth; ++i)
                cout << "  ";
            cout << "Node " << id << ": " << label
                      << " W=" << (win ? "1" : "0") << " T=" << (metrics.immediateThreat ? "1" : "0") << " t=" << (metrics.minorThreat ? "1" : "0") << " p=" << metrics.pressure << " w=" << metrics.winOptions << endl;
        }
        for (const auto *child : children)
        {
            child->print(depth + 1);
        }
    }

    bool addLayer(
        Connect4Board &board,
        Player player,
        Player opponent,
        int depth,
        int currentLayer)
    {
        if (depth <= 0 || board.full())
        {
            return true;
        }

        vector<Column> moves = board.getPossibleMoves();
        // cout << moves.size() << " possible moves for player " << endl;
        children.clear();

        for (const Column &col : moves)
        {
            if (board.findRow(col) < 0)
            {
                continue;
            }

            Connect4Board copy = board;
            int row = copy.findRow(col);
            TileMetrics metrics = Metrics::generateMetricsForTile(copy, player, row, col);

            copy.dropDisc(col, player);

            bool oppCanWin = false;
            for (const Column &oppCol : copy.getPossibleMoves())
            {
                Connect4Board reply = copy;

                reply.dropDisc(oppCol, opponent);
                if (reply.checkWin(opponent))
                {
                    oppCanWin = true;
                    break;
                }
            }
            if (oppCanWin)
            {
                continue;
            }

            TreeNode *child = new TreeNode(
                col,
                Connect4Board::colToChar(col),
                currentLayer + 1,
                (player == Player::PLAYER1 ? 1 : 2),
                copy.checkWin(player),
                metrics);

            addChild(child);
        }

        // Prune non-winning siblings if any win exists
        bool hasWin = false;
        for (auto *c : children)
            if (c->win)
            {
                hasWin = true;
                break;
            }
        if (hasWin)
        {
            for (auto it = children.begin(); it != children.end();)
            {
                if (!(*it)->win)
                {
                    delete *it;
                    it = children.erase(it);
                }
                else
                    ++it;
            }
        }
        // Recurse deeper
        for (const Column &col : moves)
        {
            Connect4Board next = board;
            next.dropDisc(col, player);
            children[col]->addLayer(next, opponent, player, depth - 1, currentLayer + 1);
        }

        return true;
    }
    // void growLayer(
    //     Connect4Board &board,
    //     Player player,
    //     Player opponent)
    // {
    //     if (children.empty())
    //     {
    //         // Reconstruct board state at this node (TODO)
    //         // Then add one layer beneath
    //         addLayer(board, player, opponent, 1);
    //     }
    //     else
    //     {
    //         // Descend to next level; swap players
    //         for (TreeNode *child : children)
    //         {
    //             // Make a local copy of board state for this path (TODO)
    //             child->growLayer(board, opponent, player);
    //         }
    //     }
    // }
    void growNode(
        Connect4Board &board,
        Player player,
        Player opponent,
        int levels,
        int currentLevel)
    {
        if (children.empty())
        {
            // This is a deepest leaf: grow levels layers here
            addLayer(board, player, opponent, levels, currentLevel);
        }
        else
        {
            // Not a leaf: for each child, reconstruct its board state and recurse
            for (TreeNode *child : children)
            {
                // TODO: reconstruct board state up to this child
                Connect4Board childBoard = board; // replace with actual state
                // Apply moves down the path to `child` to get childBoard
                child->growNode(childBoard, opponent, player, levels, currentLevel);
            }
        }
    }

    bool removeChild(TreeNode *child)
    {
        auto it = find(children.begin(), children.end(), child);
        if (it != children.end())
        {
            children.erase(it);
            return true;
        }
        return false;
    }

    void removeAllBranchesExcept(Column col)
    {
        cout << children.size() << " children before removing branches except " << Connect4Board::colToChar(col) << endl;
        for (TreeNode *it : children)
        {
            if (it->move != col)
            {
                removeBranch(it->move);
            }
        }
    }

    void removeBranch(Column col)
    {
        for (auto child : children)
        {
            if (child->move == col)
            {
                deleteSubtree(child);
                removeChild(child);
                // free(child); // Free the memory of the deleted subtree
            }
        }
    }
};

inline void deleteSubtree(TreeNode *node)
{
    if (!node)
        return;
    // First delete all children
    for (TreeNode *child : node->children)
    {
        deleteSubtree(child);
    }
    // Clear child pointers
    node->children.clear();
    // Then delete the node itself
    delete node;
}
int TreeNode::nextId = 0;

class Tree
{
public:
    TreeNode *root;
    int layers = 0;

    Tree(Connect4Board &board,
         Player player,
         Player opponent,
         int depth)
    {

        root = new TreeNode(Column::A, "Root", 0);

        root->addLayer(board, player, opponent, depth, 0);

        toDot("tree.dot");
    }

    void print() const
    {
        if (root)
        {
            root->print(0, true);
        }
    }

    // Export tree to Graphviz DOT file with node colors
    void toDot(const string &filename = "tree.dot") const
    {
        ofstream ofs(filename);
        ofs << "digraph G {\n"
               "  rankdir=LR;\n"
               "  node [shape=box, style=filled, fontcolor=black];\n"
               "  edge [arrowsize=0.7];\n";

        if (root)
        {
            string dfs_ = dfs(root, true);
            ofs << dfs_;
        }
        ofs << "}\n";
        ofs.close();
    }
    string dfs(const TreeNode *n, bool root) const
    {

        string color;
        switch (n->owner)
        {
        case 1:
            color = "lightblue";
            break;
        case 2:
            color = "lightcoral";
            break;
        default:
            color = "white";
        }
        // Node definition with label and fillcolor
        string ofs = "";
        if (root)
        {
            ofs += "  node" + to_string(n->id) + " [label=\"Root\", fillcolor=\"lightgrey\"];\n";
        }
        else
        {
            ofs += "  node" + to_string(n->id) + " [label=\"" + to_string(n->level) + ") " + n->label +
                   " W=" + (n->win ? "1" : "0") +
                   " T=" + (n->metrics.immediateThreat ? "1" : "0") +
                   " t=" + (n->metrics.minorThreat ? "1" : "0") +
                   " p=" + to_string(n->metrics.pressure) +
                   " w=" + to_string(n->metrics.winOptions) +
                   "\", fillcolor=\"" + color + "\"];\n";
        }
        // Edges and recurse
        for (const TreeNode *c : n->children)
        {
            ofs += "  node" + to_string(n->id) + " -> node" + to_string(c->id) + ";\n";
            string dfsChild = dfs(c, false);
            ofs += dfsChild;
        }

        return ofs;
    };

    // Recursive helper to emit edges (parent -> child)
    void emitEdges(const TreeNode *node, ofstream &ofs) const
    {
        if (!node)
            return;
        for (const TreeNode *c : node->children)
        {
            ofs << "  node" << node->id << " -> node" << c->id << ";\n";
            emitEdges(c, ofs);
        }
    }

    void dotToSvg(const string &dotFile = "tree.dot", const string &svgFile = "tree.svg") const
    {
        // Call Graphviz dot command to convert DOT to SVG
        string command = "dot -Tsvg " + dotFile + " -o " + svgFile;
        int ret = system(command.c_str());
        if (ret != 0)
        {
            cerr << "Graphviz failed with code " << ret << "\n";
        }
    }

    void pruneToBest(TreeNode *node, bool isPlayerTurn)
    {
        if (!node || node->children.empty())
            return;
        for (auto *c : node->children)
            pruneToBest(c, !isPlayerTurn);
        TreeNode *best = node->children.front();
        for (auto *c : node->children)
        {
            auto &m1 = c->metrics, &m2 = best->metrics;
            if (isPlayerTurn)
            {
                if (m1.winningMove != m2.winningMove)
                    best = (m1.winningMove ? c : best);
                else if (m1.winOptions != m2.winOptions)
                    best = (m1.winOptions > m2.winOptions ? c : best);
                else if (m1.pressure != m2.pressure)
                    best = (m1.pressure > m2.pressure ? c : best);
            }
            else
            {
                if (m1.winningMove != m2.winningMove)
                    best = (m1.winningMove ? best : c);
                else if (m1.winOptions != m2.winOptions)
                    best = (m1.winOptions < m2.winOptions ? c : best);
                else if (m1.pressure != m2.pressure)
                    best = (m1.pressure < m2.pressure ? c : best);
            }
        }
        for (auto it = node->children.begin(); it != node->children.end();)
        {
            if (*it != best)
            {
                delete *it;
                it = node->children.erase(it);
            }
            else
                ++it;
        }
    }

    // pruneToBest: recursively prune each layer to the optimal move
    // isPlayerTurn == true means choose best for root player;
    // false means choose worst for root player (opponent layer).
    // void pruneToBest(TreeNode *node, bool isPlayerTurn)
    // {
    //     if (!node || node->children.empty())
    //         return;

    //     // Recurse into children first
    //     for (TreeNode *child : node->children)
    //     {
    //         pruneToBest(child, !isPlayerTurn);
    //     }

    //     // Select the single best child according to minimax
    //     TreeNode *bestChild = node->children.front();
    //     for (TreeNode *child : node->children)
    //     {
    //         if (isPlayerTurn)
    //         {
    //             // Maximize player's advantage
    //             TileMetrics &m1 = child->metrics;
    //             TileMetrics &m2 = bestChild->metrics;
    //             // Use win > non-win, then winOptions, then pressure
    //             if (m1.winningMove != m2.winningMove)
    //             {
    //                 bestChild = m1.winningMove ? child : bestChild;
    //             }
    //             else if (m1.winOptions != m2.winOptions)
    //             {
    //                 bestChild = (m1.winOptions > m2.winOptions) ? child : bestChild;
    //             }
    //             else if (m1.pressure != m2.pressure)
    //             {
    //                 bestChild = (m1.pressure > m2.pressure) ? child : bestChild;
    //             }
    //         }
    //         else
    //         {
    //             // Minimize player's advantage (best for opponent)
    //             TileMetrics &m1 = child->metrics;
    //             TileMetrics &m2 = bestChild->metrics;
    //             // Reverse logic: prefer child worse for player
    //             if (m1.winningMove != m2.winningMove)
    //             {
    //                 bestChild = m1.winningMove ? bestChild : child;
    //             }
    //             else if (m1.winOptions != m2.winOptions)
    //             {
    //                 bestChild = (m1.winOptions < m2.winOptions) ? child : bestChild;
    //             }
    //             else if (m1.pressure != m2.pressure)
    //             {
    //                 bestChild = (m1.pressure < m2.pressure) ? child : bestChild;
    //             }
    //         }
    //     }

    //     // Remove siblings not equal to bestChild
    //     for (auto it = node->children.begin(); it != node->children.end();)
    //     {
    //         if (*it != bestChild)
    //         {
    //             delete *it;
    //             it = node->children.erase(it);
    //         }
    //         else
    //         {
    //             ++it;
    //         }
    //     }
    // }

    void prune(bool isPlayerTurn)
    {
        pruneToBest(root, isPlayerTurn);
    }

    void grow(Connect4Board &initialBoard,
              Player player,
              Player opponent,
              int levels = 1)
    {
        root->growNode(initialBoard, player, opponent, levels, layers);
        layers++;
    }
    void setRoot(TreeNode *newRoot)
    {
        root = newRoot;
    }

    void moveRootUp(Column column)
    {
        root->removeAllBranchesExcept(column);

        TreeNode *child = root->children[0];
        setRoot(child);
    }
};

#endif // TREE_H