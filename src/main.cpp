#include <SFML/Graphics.hpp>
#include <iostream>
#include <cctype>
#include <fstream>
using namespace std;

//helps center text (provided)
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

		sf::RenderWindow gameBoard(sf::VideoMode({ width, height }), "Minesweeper", sf::Style::Close);

//------FILE I/O------//

		vector<int> boardInfo{};
		string line;
		ifstream params(filepath + "config.cfg");

		if (params.is_open()) {
			while (getline(params,line)) {
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

		//set window size
		const unsigned int width = boardInfo[0] * 32;
		const unsigned int height = (boardInfo[1] * 32) + 100;

		//counts
		int mineCount = boardInfo[2];
		int tileCount = boardInfo[0] * boardInfo[1];

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


//------TEXTURES & SPRITES------//

		//Faces
		sf::Texture happyFaceTexture(filepath + "images/face_happy.png", false, sf::IntRect( {0,0}, {64,64} ));
		sf::Sprite happyFaceS(happyFaceTexture);
		happyFaceS.setPosition({FaceButton_xCoordinate,FaceButton_yCoordinate});

		//Debug
		sf::Texture debugTexture(filepath + "images/debug.png", false, sf::IntRect( {0,0}, {64,64} ));
		sf::Sprite debugS(debugTexture);
		debugS.setPosition({debugButton_xCoordinate,debugButton_yCoordinate});

		//Play & Pause
		sf::Texture pauseTexture(filepath + "images/pause.png", false, sf::IntRect( {0,0}, {64,64} ));
		sf::Texture playTexture(filepath + "images/play.png", false, sf::IntRect({ 0,0 }, { 64,64 }));

		sf::Sprite ppS(pauseTexture);
		ppS.setPosition({pp_xCoordinate, pp_yCoordinate});

		//Hidden Tile
		sf::Texture hiddenTileTexture(filepath + "images/tile_hidden.png", false, sf::IntRect({ 0,0 }, { 32,32 }));
		sf::Sprite hiddenTileS(hiddenTileTexture);

		//Flag
		sf::Texture flagTexture(filepath + "images/flag.png", false, sf::IntRect({ 0,0 }, { 64,64 }));
		sf::Sprite flagS(flagTexture);

//------BACK END BOARD GENERATION------//

		/*
		1) vector<vector<Tile>> board; 2d vec of Tile objects of size (cols * rows)

		Tile
		{
		pointer to tiles around itself
		value of tile (number)
		is bomb true/false
		}

		2) when user clicks a hidden_Tile sprite -> get coordinates of the click
		3) divide coordinates (x/32, y/32) to get relative position of Tile (rounded to nearest whole number I guess)

		*/


//------GAME STATE ------//
		bool isPaused = false;

		while (gameBoard.isOpen()) {
			while (const std::optional event = gameBoard.pollEvent())
			{
				// close x -> close gameBoard
				if (event->is<sf::Event::Closed>())
					gameBoard.close();

				// if mouse click -> capture click event then process below...
				if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
				{

					sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(gameBoard));

					//if left click -> check coordinates to see what was clicked...
					if (mousePressed->button == sf::Mouse::Button::Left)
					{

						//if play/pause button clicked -> pause game (tbd)
						if (ppS.getGlobalBounds().contains(mousePos))
						{
							isPaused = !isPaused;
							isPaused ? ppS.setTexture(playTexture) : ppS.setTexture(pauseTexture);
						}
					}

//I THINK THE REASON WHY THIS DOESN'T WORK IS BECAUSE WE DON'T KNOW THE POSITION OF EACH INDIVIDUAL SPRITE YET? 
// 
					//if right click -> check which tile was clicked to place flag...
					else if (mousePressed->button == sf::Mouse::Button::Right) 
					{
						if (hiddenTileS.getGlobalBounds().contains(mousePos)) {
							cout << "flag placed?" << endl;
							gameBoard.draw(flagS);
							//flagS.setPosition(hiddenTileS.getGlobalBounds());
						}

					}

				}
			}
			gameBoard.clear(sf::Color::Yellow);
			gameBoard.draw(happyFaceS);
			gameBoard.draw(debugS);
			gameBoard.draw(ppS);

			//------FRONT END BOARD GENERATION------//
			
//TODO: POPULATE 2D VECTOR WITH TILES TO KEEP TRACK OF EACH INDIVIDUAL SPRITES POSITION?

			float x_offset = 0;
			float y_offset = 0;
			//generate columns
			for (int i = 0; i < boardInfo[0]; i++) {
				
				//generate rows
				for (int j = 0; j < boardInfo[1]; j++) {
					hiddenTileS.setPosition({ x_offset, y_offset });
					gameBoard.draw(hiddenTileS);
					y_offset += 32;
				}
				x_offset += 32;
				y_offset = 0;
			}

			gameBoard.display();
		}

	}
}
