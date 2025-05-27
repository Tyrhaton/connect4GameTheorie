#ifndef TREE_H
#define TREE_H

#include "include.h"

// TreeNode class with id, label, color, children, and print/toDot functionality
class TreeNode
{
public:
    static int nextId;                // static counter for unique ids
    int id;                           // unique identifier for this node
    std::string label;                // node label
    std::string color;                // node color for DOT output
    int owner;                        // owner of the node: 0=none, 1=player1, 2=player2
    bool win;                         // indicates if this node is a winning move
    std::vector<TreeNode *> children; // child nodes

    // Constructor: assigns unique id, sets label, color, and optional owner
    TreeNode(const std::string &label_, int owner_ = 0, bool win_ = false)
        : id(nextId++), label(label_), owner(owner_), win(win_)
    {
        // Set color based on owner
        switch (owner_)
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
    }

    // Add an existing TreeNode as child
    void addChild(TreeNode *child)
    {
        if (child)
            children.push_back(child);
    }
    // Print this node and its subtree with indentation
    void print(int depth = 0) const
    {
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";
        std::cout << "Node " << id << ": " << label
                  << " (color=" << color << ", owner=" << owner << ", win=" << win << ")" << std::endl;
        for (const auto *child : children)
        {
            child->print(depth + 1);
        }
    }
};

// Initialize static id counter
int TreeNode::nextId = 0;

// Tree class encapsulating a root TreeNode and utility functions
class Tree
{
public:
    TreeNode *root; // root node of the tree

    // Constructor takes an existing TreeNode as root
    Tree(TreeNode *rootNode) : root(rootNode) {}

    // Print the entire tree
    void print() const
    {
        if (root)
            root->print();
    }

    // Export tree to Graphviz DOT file with node colors
    void toDot(const std::string &filename) const
    {
        std::ofstream ofs(filename);
        ofs << "digraph G {\n"
               "  rankdir=LR;\n"
               "  node [shape=box, style=filled, fontcolor=black];\n"
               "  edge [arrowsize=0.7];\n";
        std::function<void(const TreeNode *)> dfs = [&](const TreeNode *n)
        {
            // Node definition with label and fillcolor
            ofs << "  node" << n->id
                << " [label=\"" << n->label << " win=" << (n->win ? "1" : "0") << "\", fillcolor=\"" << n->color << "\"];\n";
            // Edges and recurse
            for (const TreeNode *c : n->children)
            {
                ofs << "  node" << n->id << " -> node" << c->id << ";\n";
                dfs(c);
            }
        };
        if (root)
            dfs(root);
        ofs << "}\n";
        ofs.close();
    }
};

#endif // TREE_H