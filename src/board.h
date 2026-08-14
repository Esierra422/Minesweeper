#pragma once
#include "Tile.h"
#include <vector>
#include <random>
using namespace std;

class Board{
private:
	bool won = false;
	bool inProgress = true;
	vector<vector<Tile>> tileMap;

public:
	Board(vector<vector<Tile>>& tileMap);
	void generateMines(vector<pair<int, int>> coordinates, int mineCount);
	void generateNumbers();
	bool didYouWin();
	bool getProgress();
	vector<vector<Tile>>& getBoard();
	Tile& getTile(int x, int y);

	

};