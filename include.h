#ifndef INCLUDE_H
#define INCLUDE_H

#include <iostream>
#include <string>
#include <array>
#include <stdexcept>
#include <vector>
#include <set>
#include <limits>
#include <fstream>
#include <functional>
#include <sstream>
#include <cstdlib>

using namespace std;

#include "Connect4Board.h"
using Column = Connect4Board::Column;
using Player = Connect4Board::Player;

#include "Metrics.h"
#include "Tree.h"
#include "GameTheorie.h"

#endif // INCLUDE_H