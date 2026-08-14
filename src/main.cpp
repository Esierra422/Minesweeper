#include <SFML/Graphics.hpp>
#include <iostream>
#include <cctype>
#include <fstream>
#include <vector>
#include <random>
#include <unordered_map>
#include "board.h"
#include "Tile.h"



/// <summary>
/*

set
map[key] vs emplace()

get
map[key] vs map.at(key)
*/
/// </summary>

using namespace std;

//helps center text for titlescreen
void setText(sf::Text& text, float x, float y)
{
	auto bounds = text.getLocalBounds();

	text.setOrigin({
		bounds.position.x + bounds.size.x / 2.f,
		bounds.position.y + bounds.size.y / 2.f
		});

	text.setPosition({ x, y });
}

//combines: setString(), setCharacterSize(), and setFillColor()
void setStringSizeAndColor(sf::Text& text, string words, int size, sf::Color color) {
	text.setString(words);
	text.setCharacterSize(size);
	text.setFillColor(color);
}

int main()
{
	//format file path
	string filepath = "C:/Users/semil/Downloads/Minesweeper/files/";
	//create welcome window
	sf::RenderWindow window(sf::VideoMode({ 800,600 }), "SFML window", sf::Style::Close);
	bool gameTime = false;

	//get window size to setup text
	sf::Vector2u size = window.getSize();
	auto [width, height] = size;

	//load font
	sf::Font font;
	if (!font.openFromFile(filepath + "font.ttf")) {
		cout << "font don't work." << endl;
	}

	//create title
	sf::Text title(font);
	title.setStyle(sf::Text::Bold | sf::Text::Underlined);
	setStringSizeAndColor(title, "MINESWEEPER", 24, sf::Color::White);
	setText(title, width / 2.0f, height / 2.0f - 150);

	//create prompt "Enter your name:"
	sf::Text prompt(font);
	prompt.setStyle(sf::Text::Bold);
	setStringSizeAndColor(prompt, "Enter your name:", 20, sf::Color::White);
	setText(prompt, width / 2.0f, height / 2.0f - 75);

	//create name
	sf::Text name(font);
	name.setStyle(sf::Text::Bold);
	setStringSizeAndColor(name, "", 18, sf::Color::Yellow);
	string myName = "";

	//handle events
	while (window.isOpen())
	{

		while (const std::optional event = window.pollEvent())
		{
			//1) close x -> close window
			if (event->is<sf::Event::Closed>())
				window.close();

			//2) if letter is input -> write letter
			//   if backspace is input -> delete letter
			if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
			{
				//check name length and character is valid
				char character = static_cast<char>(textEntered->unicode);
				if (isalpha(character) && myName.length() < 10) {

					myName += (myName.length() == 0) ? toupper(character) : tolower(character);
					name.setString(myName);
					setText(name, width / 2.0f, height / 2.0f - 45);
					std::cout << "character typed: " << character << std::endl;
				}

				//delete when backspace
				else if (character == '\b' && !myName.empty()) {
					myName.pop_back();
					name.setString(myName);
					setText(name, width / 2.0f, height / 2.0f - 45);
					std::cout << "backspace" << std::endl;
				}
			}

			//3) if enter is pressed -> move to game screen!
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->scancode == sf::Keyboard::Scan::Enter) {
					gameTime = true;
				}
			}
		}

		if (gameTime) {
			window.close();
			continue;
		}

		window.clear(sf::Color::Blue);
		window.draw(title);
		window.draw(prompt);
		window.draw(name);
		window.display();
	}

	if (gameTime) {

		sf::RenderWindow gameWindow(sf::VideoMode({ width, height }), "Minesweeper", sf::Style::Close);

//------FILE I/O------//

		vector<int> boardInfo{};
		string line;
		ifstream params(filepath + "config.cfg");

		if (params.is_open()) {
			while (getline(params, line)) {
				cout << line << endl;
				boardInfo.push_back(stoi(line));
			}
			params.close();
		}

		else { cout << "can't open file." << endl; }

		/*
		3 params:
		boardInfo[0] - width / columns
		boardInfo[1] - height / rows
		boardInfo[2] - number of mines
		*/

		int cols = boardInfo[0];
		int rows = boardInfo[1];

		//set window size
		const unsigned int width = boardInfo[0] * 32;
		const unsigned int height = (boardInfo[1] * 32) + 100;

		//counts
		int mineCount = boardInfo[2];
		int tileCount = cols * rows;

//------COORDINATES------//

		//FaceButton (x,y) - face_happy.png, face_win.png, face_lose.png
		float FaceButton_xCoordinate = ((boardInfo[0] / 2.0) * 32) - 32;
		float FaceButton_yCoordinate = 32 * (boardInfo[1] + 0.5f);

		//Debug (x,y) - debug.png
		float debugButton_xCoordinate = (boardInfo[0] * 32) - 304;
		float debugButton_yCoordinate = 32 * (boardInfo[1] + 0.5f);

		//Pause/Play (x,y) - pause.png, play.png
		float pp_xCoordinate = (boardInfo[0] * 32) - 240;
		float pp_yCoordinate = 32 * (boardInfo[1] + 0.5f);

		//Minutes in Ones
		float minutesOnes_xCoordinate = (boardInfo[0] * 32) - 118; //97+21
		float minutesOnes_yCoordinate = 32 * (boardInfo[1] + 0.5f) + 16;

		//Minutes in Tens
		float minutesTens_xCoordinate = (boardInfo[0] * 32) - 97;
		float minutesTens_yCoordinate = 32 * (boardInfo[1] + 0.5f) + 16;

		//Seconds in Ones
		float secondsOnes_xCoordinate = (boardInfo[0] * 32) - 75; //54+21
		float secondsOnes_yCoordinate = 32 * (boardInfo[1] + 0.5f) + 16;

		//Seconds in Tens
		float secondsTens_xCoordinate = (boardInfo[0] * 32) - 54;
		float secondsTens_yCoordinate = 32 * (boardInfo[1] + 0.5f) + 16;


//------TEXTURES & SPRITES------//

		//Faces
		sf::Texture happyFaceTexture(filepath + "images/face_happy.png", false, sf::IntRect({ 0,0 }, { 64,64 }));
		sf::Texture winFaceTexture(filepath + "images/face_win.png", false, sf::IntRect({ 0,0 }, { 64,64 }));
		sf::Texture loseFaceTexture(filepath + "images/face_lose.png", false, sf::IntRect({ 0,0 }, { 64,64 }));

		sf::Sprite winFaceS(winFaceTexture);
		sf::Sprite loseFaceS(loseFaceTexture);

		sf::Sprite happyFaceS(happyFaceTexture);
		happyFaceS.setPosition({ FaceButton_xCoordinate,FaceButton_yCoordinate });

		//Debug
		sf::Texture debugTexture(filepath + "images/debug.png", false, sf::IntRect({ 0,0 }, { 64,64 }));
		sf::Sprite debugS(debugTexture);
		debugS.setPosition({ debugButton_xCoordinate,debugButton_yCoordinate });

		//Play & Pause
		sf::Texture pauseTexture(filepath + "images/pause.png", false, sf::IntRect({ 0,0 }, { 64,64 }));
		sf::Texture playTexture(filepath + "images/play.png", false, sf::IntRect({ 0,0 }, { 64,64 }));

		sf::Sprite ppS(pauseTexture);
		ppS.setPosition({ pp_xCoordinate, pp_yCoordinate });

		//Hidden Tile
		sf::Texture hiddenTileTexture(filepath + "images/tile_hidden.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite hiddenTileS(hiddenTileTexture);

		//Revealed Tile
		sf::Texture revealedTileTexture(filepath + "images/tile_revealed.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite revealedTileS(revealedTileTexture);

		//Mine
		sf::Texture mineTexture(filepath + "images/mine.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite mineS(mineTexture);

		//Flag
		sf::Texture flagTexture(filepath + "images/flag.png", false, sf::IntRect({ 0,0 }, { 64,64 }));
		sf::Sprite flagS(flagTexture);

		//Digits (minutes, seconds, flagCounter)
		sf::Texture digitsTexture(filepath + "images/digits.png", false, sf::IntRect({ 0,0 }, { 231,32 }));

		//minutes in tens place
		sf::Sprite minuteTensS(digitsTexture);
		minuteTensS.setTextureRect(sf::IntRect({ 0,0 }, { 21, 32 }));
		minuteTensS.setPosition({ minutesTens_xCoordinate, minutesTens_yCoordinate });

		//minutes in ones place
		sf::Sprite minuteOnesS(digitsTexture);
		minuteOnesS.setTextureRect(sf::IntRect({ 0,0 }, { 21, 32 }));
		minuteOnesS.setPosition({ minutesOnes_xCoordinate, minutesOnes_yCoordinate });

		//seconds in tens place
		sf::Sprite secondsTensS(digitsTexture);
		secondsTensS.setTextureRect(sf::IntRect({ 0, 0 }, { 21, 32 }));
		secondsTensS.setPosition({ secondsTens_xCoordinate,secondsTens_yCoordinate });

		//seconds in ones place
		sf::Sprite secondsOnesS(digitsTexture);
		secondsOnesS.setTextureRect(sf::IntRect({ 0, 0 }, { 21, 32 }));
		secondsOnesS.setPosition({ secondsOnes_xCoordinate,secondsOnes_yCoordinate });

		sf::Sprite flagCounter(digitsTexture);

		
		//INIT DICTIONARY
		unordered_map<int, sf::Sprite> numberSprites;
		
		//1
		sf::Texture oneTexture(filepath + "images/number_1.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S1(oneTexture);
		numberSprites.emplace(1, S1);

		//2
		sf::Texture twoTexture(filepath + "images/number_2.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S2(twoTexture);
		numberSprites.emplace(2, S2);

		//3
		sf::Texture threeTexture(filepath + "images/number_3.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S3(threeTexture);
		numberSprites.emplace(3, S3);

		//4
		sf::Texture fourTexture(filepath + "images/number_4.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S4(fourTexture);
		numberSprites.emplace(4, S4);

		//5
		sf::Texture fiveTexture(filepath + "images/number_5.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S5(fiveTexture);
		numberSprites.emplace(5, S5);

		//6
		sf::Texture sixTexture(filepath + "images/number_6.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S6(sixTexture);
		numberSprites.emplace(6, S6);

		//7
		sf::Texture sevenTexture(filepath + "images/number_7.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S7(sevenTexture);
		numberSprites.emplace(7, S7);

		//8
		sf::Texture eightTexture(filepath + "images/number_8.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite S8(eightTexture);
		numberSprites.emplace(8, S8);
		

//------BACK END BOARD GENERATION------//

				// vector constructor (cols, vec(rows, default value))
				//1st param: number of cols
				//2nd param: 1D vector
					//3rd param: number of rows
					//4th param: default value (a tile object)
		vector<vector<Tile>> tileMap(cols, vector<Tile>(rows));

		//int colIdx = 0;
		//for (auto& vector : tileMap) {
		//	colIdx++;
		//	int rowIdx = 0;
		//	for (auto& tile : vector) {
		//		cout << "[" << colIdx << "," << rowIdx << "]";
		//		rowIdx++;
		//	}
		//	cout << endl;
		//}
		

//------GENERATE MINES------//

		//generate a vector of coordinates (cols x rows)
		vector< pair<int, int> > coordinates;

		Board board(tileMap);
		board.generateMines(coordinates, mineCount);
		board.generateNumbers();

		cout << "generated board backend..." << endl;


//------GAME STATE ------//
		bool isPaused = false;
		bool debugMode = false;
		bool gameOver = false;

		while (gameWindow.isOpen()) {
			while (const std::optional event = gameWindow.pollEvent())
			{
				// close x -> close gameWindow
				if (event->is<sf::Event::Closed>())
					gameWindow.close();

					// if mouse click -> capture click event then process below...
					if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
					{

						sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(gameWindow));

						//if left click -> check coordinates to see what was clicked...
						if (mousePressed->button == sf::Mouse::Button::Left)
						{
							// if play/pause button clicked -> pause game & timer
							if (ppS.getGlobalBounds().contains(mousePos))
							{
								isPaused = !isPaused;
								isPaused ? ppS.setTexture(playTexture) : ppS.setTexture(pauseTexture);
							}

							// if debug button clicked -> reveal all mines
							if (debugS.getGlobalBounds().contains(mousePos)) {
								debugMode = !debugMode;
							}

							if (isPaused == false) {
								//1) if tile clicked -> reveal (if not already revealed)
								sf::Vector2i localPosition = sf::Mouse::getPosition(gameWindow);
								float x = localPosition.x / 32.f;
								float y = localPosition.y / 32.f;

								if (x < cols && y < rows) {
									if (board.getTile(x, y).isHidden() && !board.getTile(x, y).isFlagged()) {
										board.getTile(x, y).flipHidden();
									}
								}
							}

						}

						//if right click -> check which tile was clicked to place flag...
						else if (mousePressed->button == sf::Mouse::Button::Right)
						{
							if (isPaused == false) {
								// 1) get coordinates of the click
								sf::Vector2i localPosition = sf::Mouse::getPosition(gameWindow);
								float x = localPosition.x / 32.f;
								float y = localPosition.y / 32.f;

								// error handle: check if x and y are within bounds
								if (x < cols && y < rows) {
									// 2) assign that click to a tile in tileMap
									if (board.getTile(x, y).isHidden()) {

										cout << (board.getTile(x, y).flipFlag() ? "flag!" : "removed flag!");
										cout << endl;

									}
								}
							}
						}
					}
			}

			gameWindow.clear(sf::Color::Yellow);
			gameWindow.draw(happyFaceS);
			gameWindow.draw(debugS);
			gameWindow.draw(ppS);
			gameWindow.draw(minuteTensS);
			gameWindow.draw(minuteOnesS);
			gameWindow.draw(secondsTensS);
			gameWindow.draw(secondsOnesS);

/*
		-- How this works --
		sprites are being assigned a position and then drawn at each iteration.

		-- The problem --
		Drawing a sprite object only shows its state at the time it was drawn. So, there was only one object this whole time!
		Drawing doesn't create more objects - it just prints out what that sprite looked like.
		The sprite resides on its last updated position and (since there is only one sprite object), we can only interact with this.

		This shows that we cannot interact with drawings of sprite from .draw(), we can only interact with objects. This was our intention but we got confused!

		- Solution --
		The solution is to make every tile an object so that we can interact with objects and not .draw()ings...

		Each Tile object will keep track of its own state (is it a mine, flagged, revealed, adjacent mines). Whenever we want to access such information, it will be readily available!

		It's a good idea to separate the graphics from the functionality.

			First, there will still only be 2 sprite objects (revealed and hidden).

			Second, we will generate a 2d vector of Tile objects (columns x rows).

			Third, The sprites will be drawn according to whether or not a tile at [col][row] is revealed. The board will be drawn/updated AFTER EVERY TILE CLICK.

*/

//------FRONT END BOARD GENERATION------//

			//generate columns
			for (int x = 0; x < cols; x++) {

				//generate rows
				for (int y = 0; y < rows; y++) {

					//calculate xy-coordinates once for all values and reuse
					float xyCoords[2] = { x * 32.f, y * 32.f };

					//if coords NOT within bounds of tiles, then ignore coordinates (do not update the boardstate)

					//1)  check if tile is revealed and draw corresponding sprite
					if (board.getTile(x, y).isHidden()) {
						hiddenTileS.setPosition({ xyCoords[0], xyCoords[1]});
						gameWindow.draw(hiddenTileS);

						if (board.getTile(x, y).isMine() && debugMode) {
							mineS.setPosition({ xyCoords[0], xyCoords[1] });
							gameWindow.draw(mineS);
						}
					}
					else {
						revealedTileS.setPosition({ xyCoords[0], xyCoords[1] });
						gameWindow.draw(revealedTileS);

						//if mine -> reveal it was a mine!
						if (board.getTile(x, y).isMine() == true) {
							mineS.setPosition({ xyCoords[0],xyCoords[1] });
							gameWindow.draw(mineS);

							loseFaceS.setPosition({ FaceButton_xCoordinate,FaceButton_yCoordinate });
							gameWindow.draw(loseFaceS);
						}
						else {
							//if numbered -> draw number too
							int number = board.getTile(x, y).getAdjMines();
							
							if (number > 0) {
								numberSprites.at(number).setPosition({xyCoords[0], xyCoords[1]});
								gameWindow.draw(numberSprites.at(number));
							}
							
						}


					}



					//draw all flags
					if (board.getTile(x, y).isFlagged()) {
						flagS.setPosition({ xyCoords[0], xyCoords[1] });
						gameWindow.draw(flagS);
					}

				}
			}

			gameWindow.display();
		}

	}
}
//TODO: figure out what to do with board class
//TODO: handle board states for win, lose, and in progress
//TODO: if a mine is clicked - end game
//TODO: reset board by clicking on face

