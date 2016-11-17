// TickTackToeConsole.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <Windows.h>
#include <ctime>

const int MaxMapX = 3;
const int MaxMapY = 3;

using namespace std;
char Map[MaxMapX][MaxMapY];
char Player = ' ';
char Computer = ' ';
int ViolationCounter = 0;

#pragma region SystemControl
inline void RegisterViolation(string);
void DisplayHelp();
void DisplayMap();
inline void DisplayMessage(string);
inline void DisplayMessage(string, string);
#pragma endregion

#pragma region GameControl
void PlaceSymbol(int, int, char);
void TryPlaceSymbol(int, int, char);
void NewGame();
void EndGame(string);
bool CheckSymbol(char);
bool CheckDraw();
int CheckGameState();
void ComputerMove();
#pragma endregion

int main()
{
	srand(time(NULL));
	::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);//fullscreen console
	while (true) {
		DisplayHelp();
		NewGame();
		DisplayMap();
		DisplayMessage("Please select starting player : [p/c] : ");
		char startingSymbol;
		cin >> startingSymbol;

		int selector = (startingSymbol == 'p') ? 0 : 1;

		for (int i = 0; i < 9 && CheckGameState() == 0 && ViolationCounter != 3; i++) {
			if (i % 2 == selector) {
				int xt = 0, yt = 0;
				int KeyCode = 0;
				cout << "Use arrow keys to select position, then press ENTER twice to place Your symbol :" << endl;
				cout << "x = 0, y = 0" << endl;
				while (KeyCode != VK_RETURN) {
					FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
					_getch(); //format [ Key - skip [
					KeyCode = _getch();
					switch (KeyCode) {
					case 72: //UP
						yt = (yt + 1) % 3;
						break;
					case 80: //DOWN
						yt = (yt - 1) % 3;
						break;
					case 75: //LEFT
						xt = (xt - 1) % 3;
						break;
					case 77: //RIGHT
						xt = (xt + 1) % 3;
						break;
					default:
						if (KeyCode != VK_RETURN) cout << "Hit either arrow key or ENTER" << endl;
						break;
					}

					cout << "x = " << abs(xt) << ", y = " << abs(yt) << endl;
				}
				int x, y;
				x = abs(xt);
				y = abs(yt);
				TryPlaceSymbol(y, x, Player);
				cout << CheckGameState() << endl;
			}
			else {

				int x = rand() % 3;
				int y = rand() % 3;
				TryPlaceSymbol(x, y, Computer);
				ComputerMove();
			}
			DisplayMap();
		}
		int gameState = CheckGameState();
		switch (gameState) {
		case 1:
			EndGame("Player won!");
			break;
		case 2:
			EndGame("Computer won!");
			break;
		case 3:
			EndGame("Draft!");
			break;
		}
		DisplayMap();
		DisplayMessage("New game? [0 - close] : ");
		int option;
		cin >> option;
		if (option == 0) return 0;
	}
	system("cmd /c pause"); //call command from cmd
	return 0;
}

inline void PlaceSymbol(int x, int y, char Symbol) {
	Map[x][y] = Symbol;
}

inline void TryPlaceSymbol(int x, int y, char Symbol) {
	if (Map[x][y] == ' ') {
		PlaceSymbol(x, y, Symbol);
	}
	else RegisterViolation("Cannot place symbol on other player's position");
}

void NewGame() {
	system("cmd /c cls");
	for (int x = 0; x < MaxMapX; x++) for (int y = 0; y < MaxMapY; y++) Map[x][y] = ' ';
	cout << "Enter player's symbol : ";
	cin >> Player;
	while (true) {
		cout << "Enter computer's symbol : ";
		char tmp = ' ';
		cin >> tmp;
		if (tmp == Player) continue;
		else {
			Computer = tmp;
			break;
		}
	}
}

inline void RegisterViolation(string message) {
	cout << "Game rule violation : " << message << endl;
	if (++ViolationCounter == 3) EndGame("Player exceeded permited amount of violations!");
}

inline void EndGame(string message) {
	DisplayMessage("Game finished : ", message);
}

inline void DisplayMessage(string message) {
	cout << "Message : " << message << endl;
}

inline void DisplayMessage(string message, string param) {
	cout << "Message : " << message << " " << param << endl;
}

void DisplayHelp() {
	DisplayMessage("Tick-Tack-Toe");
	DisplayMessage("Player is permited to violate rules 3 times");
	DisplayMessage("Violation is placing own symbol on another player's symbol");
}

void DisplayMap() {
	cout << " _____________ " << endl;
	for (int x = 0; x < MaxMapX; x++) {
		for (int y = 0; y < MaxMapY; y++) {
			cout << " | " << Map[x][y];
		}
		cout << " |";
		cout << endl << " _____________ " << endl;
	}
}

bool CheckSymbol(char Symbol) {
	for (int x = 0; x < MaxMapX; x++) { // horizontal
		if ((Map[x][0] == Map[x][1]) && (Map[x][1] == Map[x][2]) && (Map[x][2] == Symbol)) return true;
	}

	for (int y = 0; y < MaxMapX; y++) { // vertical
		if ((Map[0][y] == Map[1][y]) && (Map[1][y] == Map[2][y]) && (Map[2][y] == Symbol)) return true;
	}

	if ((Map[0][0] == Map[1][1]) && (Map[1][1] == Map[2][2]) && (Map[2][2] == Symbol)) return true;

	if ((Map[2][0] == Map[1][1]) && (Map[1][1] == Map[0][2]) && (Map[0][2] == Symbol)) return true;

	return false;
}

bool CheckDraw() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if(Map[i][j] == ' ') return false;
		}
	}
	return true;
}

// 0 - running
// 1 - player won
// 2 - computer won
// 3 - draft

int CheckGameState() {
	if (CheckDraw()) return 3;
	if (CheckSymbol(Player)) return 1;
	if (CheckSymbol(Computer)) return 2;
	return 0;
}

#pragma region minimax

int minimax(char currentPlayer) {
	int score, maxScore;
	if (CheckSymbol(currentPlayer)) return (currentPlayer == Computer) ? 1 : -1;
	if (CheckDraw()) return 0;

	currentPlayer = (currentPlayer == Computer) ? Player : Computer;

	maxScore = (currentPlayer == Player) ? 10 : -10;
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (Map[i][j] == ' ') {
				Map[i][j] = currentPlayer;
				score = minimax(currentPlayer);
				Map[i][j] = ' ';
				if ((currentPlayer == Player && score < maxScore) || (currentPlayer == Computer && score > maxScore)) maxScore = score;
			}
		}
	}
	return maxScore;
}

void ComputerMove() {
	int score, maxScore;
	maxScore = -10;
	int move[] = { 0,0 };
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (Map[i][j] == ' ') {
				Map[i][j] = Computer;
				score = minimax(Computer);
				Map[i][j] = ' ';
				if (score > maxScore) {
					maxScore = score;
					move[0] = i;
					move[1] = j;
				}
			}
		}
	}
	TryPlaceSymbol(move[0], move[1], Computer);
}

#pragma endregion
