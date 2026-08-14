#include "board.h"


Board::Board(vector<vector<Tile>>& tileMap)
{
	this->tileMap = tileMap;
}

void Board::generateMines(vector<pair<int, int>> coordinates, int mineCount) {

	for (int x = 0; x < tileMap.size(); x++) {
		for (int y = 0; y < tileMap[x].size(); y++) {
			coordinates.push_back({ x,y });
		}
	}

	//init randomizer
	random_device rd;
	mt19937 gen(rd());

	//shuffle coordinates
	shuffle(coordinates.begin(), coordinates.end(), gen);

	int n = mineCount;
	//pick n = mineCount first coordinates

	while (n != 0) {
		tileMap[coordinates[mineCount - n].first][coordinates[mineCount - n].second].setMine();
		n--;
	}

}

bool Board::didYouWin()
{
	return won;
}

bool Board::getProgress()
{
	if (inProgress) {
		return inProgress;
	}
	else {
		return didYouWin();
	}
}

vector<vector<Tile>>& Board::getBoard()
{
	return tileMap;
}

Tile& Board::getTile(int x, int y)
{
	return tileMap[x][y];
}

void Board::generateNumbers() {
	for (int x = 0; x < tileMap.size(); x++) {
		for (int y = 0; y < tileMap[x].size(); y++) {
			
			if (y - 1 >= 0) {
				//north(x , y-1)
				if (tileMap[x][y - 1].isMine()) {
					tileMap[x][y].increase();
				}
			}

			if (x - 1 >= 0) {
				//west (x-1 , y)
				if (tileMap[x - 1][y].isMine()) {
					tileMap[x][y].increase();
				}
			}

			if (y + 1 < tileMap[x].size()) {
				//south (x , y+1)
				if (tileMap[x][y + 1].isMine()) {
					tileMap[x][y].increase();
				}
			}

			if (x + 1 < tileMap.size()) {
				//east (x+1 , y)
				if (tileMap[x + 1][y].isMine()) {
					tileMap[x][y].increase();
				}
			}

			if (y - 1 >= 0 && x + 1 < tileMap.size()) {
				//north-east (x+1,y-1)
				if (tileMap[x + 1][y - 1].isMine()) {
					tileMap[x][y].increase();
				}
			}

			if (x + 1 < tileMap.size() && y + 1 < tileMap[x].size()) {
				//south-east(x+1,y+1)
				if (tileMap[x + 1][y + 1].isMine()) {
					tileMap[x][y].increase();
				}
			}

			if (x - 1 >= 0 && y + 1 < tileMap[x].size()) {
				//south-west (x-1,y+1)
				if (tileMap[x - 1][y + 1].isMine()) {
					tileMap[x][y].increase();
				}
			}

			if (x - 1 >= 0 && y-1 >= 0) {
				//north-west (x-1, y-1)
				if (tileMap[x - 1][y - 1].isMine()) {
					tileMap[x][y].increase();
				}
			}
		}
	}
}