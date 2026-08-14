#include "Tile.h"

Tile::Tile() :
	mined(false),
	flagged(false),
	hidden(true),
	number(0)
{};

Tile::Tile(bool mined, bool flagged, bool hidden, int MinesAroundMe) :
	mined{ mined },
	flagged{ flagged },
	hidden{ hidden },
	number{ MinesAroundMe } 
{};

bool Tile::isMine() {
		return mined;
	};
bool Tile::isFlagged() {
		return flagged;
	};
bool Tile::isHidden() {
		return hidden;
	};

int Tile::getAdjMines() {
	return number;
};



bool Tile::setMine()
{
	mined = !mined;
	return mined;
};
bool Tile::flipFlag()
{
	flagged = !flagged;
	return  flagged;
};
bool Tile::flipHidden()
{
	hidden = !hidden;
	return hidden;
};
int Tile::setAdjMines()
{
	return 0;
};
void Tile::increase()
{
	number++;
};


