#ifndef INCLUDE_GAME_THEORIE_H
#define INCLUDE_GAME_THEORIE_H

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
#include <queue>

using namespace std;

#include "Connect4Board.h"
using Column = Connect4Board::Column;
using Player = Connect4Board::Player;

#include "MoveRecorder.h"
#include "Metrics.h"
#include "Tree.h"
#include "GameTheorie.h"

#endif // INCLUDE_GAME_THEORIE_H