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
    Player owner;
    TileMetrics metrics;
    int id;
    static int nextId;
    int row;
    vector<TreeNode *> children;

    /**
     * Constructor for TreeNode
     * @param move_ The column where the move is made.
     * @param label_ The label for the node (usually the column name).
     * @param level_ The level of the node in the tree.
     * @param owner_ The player who owns this node.
     * @param metrics_ The metrics associated with this node.
     * @param row_ The row where the move is made (default -1).
     */
    TreeNode(Column move_ = Column::INVALID, const string label_ = "", int level_ = 0, Player owner_ = Player::EMPTY, TileMetrics metrics_ = {0, 0, false, false, false}, int row_ = -1)
        : move(move_), label(label_), level(level_), owner(owner_), metrics(metrics_), id(nextId++), row(row_), children()
    {
    }

    /**
     * Add an existing TreeNode as child
     * @param child The child node to add.
     */
    void addChild(TreeNode *child)
    {
        if (child)
        {
            children.push_back(child);
        }
    }

    /**
     * Print the tree node and its children.
     * @param depth The current depth in the tree (used for indentation).
     * @param root Whether this is the root node (used for special formatting).
     */
    void print(
        int depth = 0,
        bool root = false) const
    {
        if (root)
        {
            cout << "Root Node: " << label << endl;
        }
        else
        {
            for (int i = 0; i < depth; ++i)
            {
                cout << "  ";
                cout << "Node " << id << ": " << label
                     << " W=" << (metrics.winningMove ? "1" : "0") << " T=" << (metrics.immediateThreat ? "1" : "0") << " t=" << (metrics.minorThreat ? "1" : "0") << " p=" << metrics.pressure << " w=" << metrics.winOptions << endl;
            }
        }
        for (const auto *child : children)
        {
            child->print(depth + 1);
        }
    }

    /**
     * Add a layer of child nodes to this node.
     * @param board The current state of the game board.
     * @param depth The remaining depth to explore.
     * @param currentLayer The current layer of the tree.
     * @param advancedPruning Whether to use advanced pruning techniques.
     * @param startingPlayer The player who started the game (used for pruning).
     * @return True if the layer was added successfully, false otherwise.
     */
    bool addLayer(
        Connect4Board &board,
        int depth,
        int currentLayer,
        bool advancedPruning = true,
        Player startingPlayer = Player::EMPTY)
    {
        Player player = board.getOponent(owner);
        Player opponent = owner;

        if (depth <= 0 || board.full() || metrics.winningMove)
        {
            return true;
        }
        if (!children.empty())
        {
            for (TreeNode *child : children)
            {
                int r_child = board.findRow(child->move);
                board.dropDisc(child->move, player);
                child->addLayer(board, depth - 1, child->level, advancedPruning);
                board.setCell(r_child, child->move, Player::EMPTY);
            }
            return true;
        }

        vector<Column> moves = board.getPossibleMoves();
        vector<TreeNode *> candidateChildren;
        bool hasWin = false;

        // calculate possible children
        for (Column column : moves)
        {
            int r_play = board.findRow(column);
            if (r_play < 0)
            {
                continue;
            }
            TileMetrics tileMetrics = Metrics::generateMetricsForTile(board, player, r_play, column);

            board.setCell(r_play, column, player);

            bool oppCanWin = false;
            for (Column oppCol : board.getPossibleMoves())
            {
                int r_opp = board.findRow(oppCol);
                if (r_opp < 0)
                {
                    continue;
                }
                board.setCell(r_opp, oppCol, opponent);
                if (board.checkWin(opponent))
                {
                    oppCanWin = true;
                }
                board.setCell(r_opp, oppCol, Player::EMPTY);
                if (oppCanWin)
                {
                    break;
                }
            }

            if (oppCanWin && player == Player::BOT)
            {
                board.setCell(r_play, column, Player::EMPTY);
                continue;
            }

            TreeNode *child = new TreeNode(
                column,
                Connect4Board::colToChar(column),
                currentLayer + 1,
                player,
                tileMetrics,
                board.ROWS - r_play);

            candidateChildren.push_back(child);
            board.setCell(r_play, column, Player::EMPTY);

            if (tileMetrics.winningMove && player == Player::BOT)
            {
                hasWin = true;
            }
        }

        // If the bot can win, no other children are needed
        if (hasWin && player == Player::BOT)
        {
            for (auto it = candidateChildren.begin(); it != candidateChildren.end();)
            {
                if (!(*it)->metrics.winningMove)
                {
                    deleteSubtree(*it);
                    it = candidateChildren.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        // If advanced pruning is enabled and this is a BOT turn, pick the best move only
        if (advancedPruning && player == Player::BOT)
        {

            TreeNode *bestChild = selectBestBotChild(candidateChildren, startingPlayer);

            if (!bestChild && !candidateChildren.empty())
            {
                bestChild = candidateChildren.front();
            }

            for (auto it = candidateChildren.begin(); it != candidateChildren.end();)
            {
                if (*it == bestChild)

                {
                    ++it; // keep bestChild
                }
                else
                {
                    deleteSubtree(*it);
                    it = candidateChildren.erase(it);
                }
            }
        }

        if (candidateChildren.empty())
        {
            vector<Column> fallbackMoves = board.getPossibleMoves();
            if (!fallbackMoves.empty())
            {
                Column fallbackCol = fallbackMoves.front();
                int r_play = board.findRow(fallbackCol);
                if (r_play >= 0)
                {
                    TileMetrics tileMetrics = Metrics::generateMetricsForTile(board, board.getOponent(player), r_play, fallbackCol);
                    TreeNode *fallback = new TreeNode(
                        fallbackCol,
                        Connect4Board::colToChar(fallbackCol),
                        currentLayer + 1,
                        board.getOponent(player),
                        tileMetrics,
                        board.ROWS - r_play);
                    candidateChildren.push_back(fallback);
                }
            }
        }

        for (TreeNode *child : candidateChildren)
        {
            children.push_back(child);
            int row = board.findRow(child->move);
            board.setCell(row, child->move, player);
            if (!child->metrics.winningMove)
            {
                child->addLayer(board, depth - 1, child->level, advancedPruning);
            }
            board.setCell(row, child->move, Player::EMPTY);
        }

        return true;
    }

    /**
     * Remove a child node from this node.
     * @param child The child node to remove.
     * @return True if the child was removed successfully, false otherwise.
     */
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

    /**
     * Remove all branches except the one specified by the column.
     * This will delete all children that do not match the specified column.
     * @param col The column to keep.
     */
    void removeAllBranchesExcept(Column col = Column::INVALID)
    {
        for (size_t i = 0; i < children.size();)
        {
            TreeNode *child = children[i];

            if (child->move != col)
            {
                children.erase(children.begin() + i);

                deleteSubtree(child);
            }
            else
            {
                ++i;
            }
        }
    }

    /**
     * Remove a branch from the tree based on the specified column.
     * This will delete the subtree rooted at the child node that matches the column.
     * @param column The column to remove.
     */
    void removeBranch(Column column)
    {
        for (auto child : children)
        {
            if (child->move == column)
            {
                deleteSubtree(child);
                removeChild(child);
            }
        }
    }

    /**
     * Remove a branch from the tree based on the specified column.
     * This will delete the subtree rooted at the child node that matches the column.
     * @param column The column to remove.
     */
    void removeBranchByNode()
    {
        deleteSubtree(this);
        removeChild(this);
    }

    /**
     * Select the best child node for the bot player from the given candidates.
     * @param candidates The list of candidate child nodes.
     * @param startingPlayer The player who is making the move.
     * @return The best child node for the bot player.
     */
    TreeNode *selectBestBotChild(
        const std::vector<TreeNode *> &candidates,
        Player startingPlayer)
    {
        bool botPrefersOddWin = (startingPlayer == Player::BOT);
        TreeNode *bestChild = nullptr;
        TreeNode *threatChild = nullptr;
        TreeNode *minorThreatChild = nullptr;

        int bestScore = -1;
        int pressure = -1;
        bool enablesOpponentThreatFound = false;

        for (TreeNode *child : candidates)
        {
            if (!child)
            {
                continue;
            }
            if (child->metrics.winningMove)
            {
                return child;
            }

            if (child->metrics.immediateThreat)
            {
                bool better = false;
                if (!threatChild)
                {
                    better = true;
                }
                else if (child->metrics.pressure > pressure)
                {
                    better = true;
                }
                else if (
                    child->metrics.pressure == pressure &&
                    !child->metrics.enablesOpponentThreat &&
                    enablesOpponentThreatFound)
                {
                    better = true;
                }
                if (better)
                {
                    threatChild = child;
                    pressure = child->metrics.pressure;
                    enablesOpponentThreatFound = child->metrics.enablesOpponentThreat;
                }
            }

            if (child->metrics.minorThreat)
            {
                bool better = false;
                if (!minorThreatChild)
                {
                    better = true;
                }
                else if (child->metrics.pressure > pressure)
                {
                    better = true;
                }
                else if (
                    child->metrics.pressure == pressure &&
                    !child->metrics.enablesOpponentThreat &&
                    enablesOpponentThreatFound)
                {
                    better = true;
                }
                if (better)
                {
                    minorThreatChild = child;
                    pressure = child->metrics.pressure;
                    enablesOpponentThreatFound = child->metrics.enablesOpponentThreat;
                }
            }

            int bonus = 0;
            if (!child->metrics.enablesOpponentThreat)
            {
                bonus += 5;
            }
            int futureRow = child->metrics.preferredWinningRow;
            if (futureRow != -1)
            {
                bool isOdd = (futureRow % 2 == 1);
                if (isOdd == botPrefersOddWin)
                {
                    bonus += 5;
                }
            }

            int score = child->metrics.winOptions * 10 + child->metrics.pressure + bonus;

            if (!bestChild || score > bestScore)
            {
                bestScore = score;
                bestChild = child;
            }
        }

        if (threatChild)
        {
            return threatChild;
        }
        if (minorThreatChild)
        {
            return minorThreatChild;
        }
        return bestChild;
    }
};

/**
 * Delete the entire subtree rooted at the given node.
 * @param node The root of the subtree to delete.
 */
inline void deleteSubtree(TreeNode *node)
{
    if (!node)
    {
        return;
    }
    for (TreeNode *child : node->children)
    {
        deleteSubtree(child);
    }
    node->children.clear();
    delete node;
}

int TreeNode::nextId = 0;

class Tree
{
public:
    TreeNode *ROOT;
    int layers = 0;
    int DEPTH = 0;
    bool ADVANCEDPRUNING = true;
    Player STARTINGPLAYER = Player::EMPTY;

    Tree(Connect4Board &board,
         Player startingPlayer,
         int depth, bool advancedPruning = true) : DEPTH(depth), ADVANCEDPRUNING(advancedPruning), STARTINGPLAYER(startingPlayer)
    {

        ROOT = new TreeNode(Column::A, "Root", 0, board.getOponent(startingPlayer), TileMetrics{-1, -1, false, false, false, -1, false});
        ROOT->addLayer(board, depth, 0, advancedPruning);
    }

    /**
     * Print the tree structure.
     */
    void print() const
    {
        if (ROOT)
        {
            ROOT->print(0, true);
        }
    }

    /**
     * Export the tree structure to a Graphviz DOT file.
     * @param filename The name of the output DOT file, default is "tree.dot".
     */
    void toDot(const string &filename = "tree.dot") const
    {
        ofstream ofs(filename);
        ofs << "digraph G {\n"
               "  rankdir=LR;\n"
               "  node [shape=box, style=filled, fontcolor=black];\n"
               "  edge [arrowsize=0.7];\n";

        if (ROOT)
        {
            string dfs_ = dfs(ROOT, true);
            ofs << dfs_;
        }
        ofs << "}\n";
        ofs.close();
    }

    /**
     * Depth-first search (DFS) for exporting the tree structure.
     * @param node The current node.
     * @param root Whether this is the root node.
     * @return The DOT representation of the subtree.
     */
    string dfs(
        const TreeNode *node,
        bool root = false) const
    {

        bool displayMetrics = true; // Set to false to disable metrics display
        string color;
        switch (node->owner)
        {
        case Player::BOT:
            color = "lightcoral";
            break;
        case Player::USER:
            color = "lightblue";
            break;
        default:
            color = "white";
        }

        if (node->metrics.winningMove)
        {
            color = "lightgreen";
        }
        else if (node->metrics.immediateThreat)
        {
            color = "yellow";
        }
        else if (node->metrics.minorThreat)
        {
            color = "orange";
        }

        string ofs = "";
        if (root)
        {
            ofs += "  node" + to_string(node->id) + " [label=\"Root\", fillcolor=\"lightgrey\"];\n";
        }
        else
        {
            if (displayMetrics)
            {
                ofs += "  node" + to_string(node->id) + " [label=\"" + to_string(node->level) + ") " + Connect4Board::colToChar(node->move) + to_string(node->row) +
                       " W=" + (node->metrics.winningMove ? "1" : "0") +
                       " T=" + (node->metrics.immediateThreat ? "1" : "0") +
                       " t=" + (node->metrics.minorThreat ? "1" : "0") +
                       " p=" + to_string(node->metrics.pressure) +
                       " w=" + to_string(node->metrics.winOptions) +
                       "\", fillcolor=\"" + color + "\"];\n";
            }
            else
            {
                ofs += "  node" + to_string(node->id) + " [label=\"" + to_string(node->level) + ") " + Connect4Board::colToChar(node->move) + to_string(node->row) + ": " + to_string(node->id) +
                       "\", fillcolor=\"" + color + "\"];\n";
            }
        }
        for (const TreeNode *child : node->children)
        {
            ofs += "  node" + to_string(node->id) + " -> node" + to_string(child->id) + ";\n";
            string dfsChild = dfs(child, false);
            ofs += dfsChild;
        }

        return ofs;
    };

    /**
     * Emit edges from the current node to its children.
     * @param node The current node.
     * @param ofs The output stream to write the edges to.
     */
    void emitEdges(
        const TreeNode *node,
        ofstream &ofs) const
    {
        if (!node)
        {
            return;
        }
        for (const TreeNode *child : node->children)
        {
            ofs << "  node" << node->id << " -> node" << child->id << ";\n";
            emitEdges(child, ofs);
        }
    }

    /**
     * Convert the DOT file to SVG format.
     * @param dotFile The name of the input DOT file.
     * @param svgFile The name of the output SVG file.
     */
    void dotToSvg(
        const string &dotFile = "tree.dot",
        const string &svgFile = "tree.svg") const
    {
        string command = "dot -Tsvg " + dotFile + " -o " + svgFile;
        int ret = system(command.c_str());
        if (ret != 0)
        {
            cerr << "Graphviz failed with code " << ret << "\n";
        }
    }

    /**
     * Grow the tree by adding a new layer of nodes.
     * This will expand the tree by one level based on the current board state.
     * @param currentBoard The current state of the Connect 4 board.
     * @param levels The number of levels to grow (default is 1).
     */
    void grow(
        Connect4Board &currentBoard,
        int levels = 1)
    {
        ROOT->addLayer(currentBoard, levels, layers);
        layers++;
    }

    /**
     * Set a new root for the tree.
     * This will replace the current root with the specified node.
     * @param newRoot The new root node to set.
     */
    void setRoot(TreeNode *newRoot)
    {
        ROOT = newRoot;
    }

    /**
     * Move the root node up by removing all branches except the specified column.
     * This will effectively make the child of the current root the new root.
     * @param column The column to keep as the new root.
     */
    void moveRootUp(Column column)
    {
        ROOT->removeAllBranchesExcept(column);

        if (ROOT->children.empty())
        {
            throw runtime_error("No children found for the current root. Cannot move root up.");
        }

        for (TreeNode *it : ROOT->children)
        {
            if (it->move == column)
            {
                setRoot(it);
            }
        }
    }

    /**
     * Update the tree by growing it from the current root.
     * This will remove all branches except the specified column and grow the tree.
     * @param board The current state of the Connect 4 board.
     * @param column The column to keep as the new root.
     * @param debug Whether to enable debug output.
     */
    void updateTree(
        Connect4Board &board,
        Column column,
        bool debug = false)
    {
        if (!ROOT)
        {
            throw runtime_error("Tree root is not initialized.");
        }
        if (debug)
        {
            cout << "Updating tree with root: " << Connect4Board::colToChar(column) << endl;
        }
        moveRootUp(column);
        grow(board, DEPTH);
        toDot();
        dotToSvg();
    }
};

#endif // TREE_H