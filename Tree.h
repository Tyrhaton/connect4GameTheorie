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
    // bool win;
    TileMetrics metrics;
    int id;
    static int nextId;
    int row;
    vector<TreeNode *> children;

    TreeNode(Column move_, const string label_, int level_ = 0, Player owner_ = Player::EMPTY, TileMetrics metrics_ = {0, 0, false, false, false}, int row_ = -1)
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
                 << " W=" << (metrics.winningMove ? "1" : "0") << " T=" << (metrics.immediateThreat ? "1" : "0") << " t=" << (metrics.minorThreat ? "1" : "0") << " p=" << metrics.pressure << " w=" << metrics.winOptions << endl;
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
     * @return True if the layer was added successfully, false otherwise.
     */
    bool addLayer(
        Connect4Board &board,
        int depth,
        int currentLayer)
    {
        Player player = board.getOponent(owner);
        Player opponent = owner;

        if (depth <= 0 || board.full())
        {
            return true;
        }

        vector<Column> moves = board.getPossibleMoves();
        children.clear();

        // Prune non-winning siblings if any win exists
        bool hasWin = false;

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

            // cout << to_string(board.ROWS - row) << " " << Connect4Board::colToChar(col) << " " << player << " " << metrics.pressure << " " << metrics.winOptions << endl;
            TreeNode *child = new TreeNode(
                col,
                Connect4Board::colToChar(col),
                currentLayer + 1,
                player,
                metrics,
                board.ROWS - row);

            addChild(child);

            if (child->metrics.winningMove)
            {
                hasWin = true;
                break;
            }
        }
        if (hasWin)
        {
            for (auto it = children.begin(); it != children.end();)
            {
                if (!(*it)->metrics.winningMove)
                {
                    delete *it;
                    it = children.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        // Recurse deeper
        for (TreeNode *child : children)
        {
            // Reconstruct the board state up to this child (drop child->move on a copy of board)
            Connect4Board nextBoard = board;
            nextBoard.dropDisc(child->move, player);
            child->addLayer(
                nextBoard,
                depth - 1,
                child->level);
        }

        return true;
    }

    /**
     * Grow the node by adding layers to it.
     * If this node is a leaf, it will expand "levels" deeper layers.
     * If it has children, it will recurse on its children untill it reaches the leaf nodes.
     * @param board The current state of the game board.
     * @param levels The number of levels to grow.
     * @param currentLevel The current level of this node in the tree.
     */
    void growNode(
        Connect4Board &board,
        int levels,
        int currentLevel)
    {
        if (children.empty())
        {
            addLayer(board, levels, currentLevel);
        }
        else
        {
            for (TreeNode *child : children)
            {
                Connect4Board childBoard = board;
                childBoard.dropDisc(child->move, owner);

                child->growNode(
                    childBoard,
                    levels,
                    child->level);
            }
        }
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
    void removeAllBranchesExcept(Column col)
    {
        // cout << children.size() << " children before removing branches except " << Connect4Board::colToChar(col) << endl;
        for (TreeNode *it : children)
        {
            if (it->move != col)
            {
                removeBranch(it->move);
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
    TreeNode *root;
    int layers = 0;

    Tree(Connect4Board &board,
         Player startingPlayer,
         int depth)
    {

        root = new TreeNode(Column::A, "Root", 0, board.getOponent(startingPlayer));

        root->addLayer(board, depth, 0);

        toDot("tree.dot");
    }

    /**
     * Print the tree structure.
     */
    void print() const
    {
        if (root)
        {
            root->print(0, true);
        }
    }

    /**
     * Export the tree structure to a Graphviz DOT file.
     * @param filename The name of the output DOT file.
     */
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

    /**
     * Depth-first search (DFS) for exporting the tree structure.
     * @param node The current node.
     * @param root Whether this is the root node.
     * @return The DOT representation of the subtree.
     */
    string dfs(const TreeNode *node, bool root) const
    {

        string color;
        switch (node->owner)
        {
        case Player::PLAYER1:
            color = "lightblue";
            break;
        case Player::PLAYER2:
            color = "lightcoral";
            break;
        default:
            color = "white";
        }
        // Node definition with label and fillcolor
        string ofs = "";
        if (root)
        {
            ofs += "  node" + to_string(node->id) + " [label=\"Root\", fillcolor=\"lightgrey\"];\n";
        }
        else
        {
            ofs += "  node" + to_string(node->id) + " [label=\"" + to_string(node->level) + ") " + Connect4Board::colToChar(node->move) + to_string(node->row) +
                   " W=" + (node->metrics.winningMove ? "1" : "0") +
                   " T=" + (node->metrics.immediateThreat ? "1" : "0") +
                   " t=" + (node->metrics.minorThreat ? "1" : "0") +
                   " p=" + to_string(node->metrics.pressure) +
                   " w=" + to_string(node->metrics.winOptions) +
                   "\", fillcolor=\"" + color + "\"];\n";
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
    void emitEdges(const TreeNode *node, ofstream &ofs) const
    {
        if (!node)
            return;
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
    void dotToSvg(const string &dotFile = "tree.dot", const string &svgFile = "tree.svg") const
    {
        string command = "dot -Tsvg " + dotFile + " -o " + svgFile;
        int ret = system(command.c_str());
        if (ret != 0)
        {
            cerr << "Graphviz failed with code " << ret << "\n";
        }
    }

    /**
     * Prune the tree to keep only the best branches based on the metrics.
     * This will keep the best child for each node based on the player's turn.
     * @param node The current node to prune.
     * @param isPlayerTurn Whether it is the player's turn (true) or the opponent's turn (false).
     */
    void pruneToBest(TreeNode *node, bool isPlayerTurn)
    {
        if (!node || node->children.empty())
        {
            return;
        }
        for (auto *child : node->children)
        {
            pruneToBest(child, !isPlayerTurn);
        }
        TreeNode *best = node->children.front();
        for (auto *child : node->children)
        {
            auto &m1 = child->metrics, &m2 = best->metrics;
            if (isPlayerTurn)
            {
                if (m1.winningMove != m2.winningMove)
                {
                    best = (m1.winningMove ? child : best);
                }
                else if (m1.winOptions != m2.winOptions)
                {
                    best = (m1.winOptions > m2.winOptions ? child : best);
                }
                else if (m1.pressure != m2.pressure)
                {
                    best = (m1.pressure > m2.pressure ? child : best);
                }
            }
            else
            {
                if (m1.winningMove != m2.winningMove)
                {
                    best = (m1.winningMove ? best : child);
                }
                else if (m1.winOptions != m2.winOptions)
                {
                    best = (m1.winOptions < m2.winOptions ? child : best);
                }
                else if (m1.pressure != m2.pressure)
                {
                    best = (m1.pressure < m2.pressure ? child : best);
                }
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
            {
                ++it;
            }
        }
    }

    /**
     * Prune the tree to keep only the best branches based on the player's turn.
     * This will prune the tree to the best move for the current player.
     * @param isPlayerTurn Whether it is the player's turn (true) or the opponent's turn (false).
     */
    void prune(bool isPlayerTurn)
    {
        pruneToBest(root, isPlayerTurn);
    }

    /**
     * Grow the tree by adding a new layer of nodes.
     * This will expand the tree by one level based on the current board state.
     * @param currentBoard The current state of the Connect 4 board.
     * @param levels The number of levels to grow (default is 1).
     */
    void grow(Connect4Board &currentBoard,
              int levels = 1)
    {
        root->growNode(currentBoard, levels, layers);
        layers++;
    }

    /**
     * Set a new root for the tree.
     * This will replace the current root with the specified node.
     * @param newRoot The new root node to set.
     */
    void setRoot(TreeNode *newRoot)
    {
        root = newRoot;
    }

    /**
     * Move the root node up by removing all branches except the specified column.
     * This will effectively make the child of the current root the new root.
     * @param column The column to keep as the new root.
     */
    void moveRootUp(Column column)
    {
        root->removeAllBranchesExcept(column);

        TreeNode *child = root->children[0];
        setRoot(child);
    }

    /**
     * Update the tree by growing it from the current root.
     * This will remove all branches except the specified column and grow the tree.
     * @param board The current state of the Connect 4 board.
     * @param column The column to keep as the new root.
     */
    void updateTree(Connect4Board &board, Column column)
    {
        if (!root)
        {
            throw runtime_error("Tree root is not initialized.");
        }
        moveRootUp(column);
        grow(board, 1);
        toDot();
        dotToSvg();
    }
};

#endif // TREE_H