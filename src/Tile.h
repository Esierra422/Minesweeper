#pragma once

class Tile
{
private:
	bool mined;
	bool flagged;
	bool hidden;
	int number;

public:
	Tile();
	Tile(bool mined, bool flagged, bool hidden, int MinesAroundMe);

	bool isMine();
	bool isFlagged();
	bool isHidden();
	int getAdjMines();

	bool setMine();
	bool flipFlag();
	bool flipHidden();
	int setAdjMines();
	void increase();
};