#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <list>
#include <vector>
#include <queue>
#include <stack>

#include "Node.h"

/*
Path finding programme
8 : starting position
9 : destination
0 : Wall
1 : Walkable terrain
*/

// Number of tiles in both X and Y directions
#define SIZE_I 77
#define SIZE_J 77

// Size of screen in pixels
float const screenSizeX = 900;
float const screenSizeY = 900;

// Space between each white tile
float const interTileX = 3.0f;
float const interTileY = 3.0f;

std::vector<std::pair<int,int>> visitOrder;

float const tileSizeX = (screenSizeX - (SIZE_J - 1.0f) * interTileX) / SIZE_J;
float const tileSizeY = (screenSizeY - (SIZE_I - 1.0f) * interTileY) / SIZE_I;

#define START 8
#define END 9

sf::Color startColor = sf::Color(50,255,50);
sf::Color endColor = sf::Color::Magenta;
sf::Color visitedColor = sf::Color::Red;
sf::Color pathColor = sf::Color::Green;

// Used to easily acces neighbor nodes
int rowNumber[] = { -1, 0, 1, 0 };
int colNumber[] = { 0, 1, 0, -1 };

void DisplayMaze(int *maze);
bool TileInBounds(int i, int j);
void FindStartPosition(std::pair<int, int> *pathFound, int * maze);
int GetPosition(int i, int j);
void ClearArray(int * someArray, int size);

std::vector<Node> CreateGraphe(int * maze);
int FindNode(std::pair<int, int> position, std::vector<Node>& graph);

int DepthFirstSearch(std::pair<int, int> startPosition, int * maze, std::vector<Node>& graph);
int BreadthFirstSearch(std::pair<int, int> startPosition, int * maze, std::vector<Node>& graph);
bool FindPathPoorly(int i, int j, int *visited, int *maze);

void ShowMoves(int endPositionGraphId, int * startMaze, sf::RenderWindow &window, std::vector<Node>& graph, std::pair<int, int> startPos);

void DrawTile(int i, int j, sf::Color color, sf::RenderWindow &window);
void ResetScreen(int * maze, sf::RenderWindow &window);
sf::Color GetColor(int value);

int main()
{
	// Random maze generated using an algorithm developped by #####
	int startMaze[SIZE_I * SIZE_J] =
	{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,
0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,
0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,
0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,
0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,
0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,
0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,
0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,
0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,
0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,
0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,
0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,
0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,
0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,
0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,
0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,
0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,
0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,
0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,
0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,
0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,
0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,
0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,
0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,
0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,
0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,
0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,
0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,
0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,
0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,
0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,
0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,
0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,
0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,
0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,
0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,
0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,
0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,
0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,
0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,
0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,
0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,
0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,
0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,
0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,
0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,
0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,
0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,
0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,
0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,
0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,
0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,
0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,
0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,
0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,
0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,
0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,
0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,
0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,
0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,
0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,
0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,
0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,
0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,
0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,
0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,
0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,
0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,
0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,
0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,
0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,
0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,9,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	//int startMaze[SIZE_I 0, SIZE_J] =
	//{
	//	8, 0, 0, 0, 0,
	//	1, 0, 0, 0, 0,
	//	1, 0, 0, 0, 0,
	//	1, 0, 0, 0, 0,
	//	1, 1, 1, 1, 9
	//};

	// Find start position
	std::pair<int, int> startPos = { 0,0 };
	FindStartPosition(&startPos, startMaze);

	// Display start position and maze
	//std::cout << "i:" << startPos.first << "   j:" << startPos.second << "\n\n";
	//DisplayMaze(startMaze);

	// Display maze using SFML library
	sf::RenderWindow window(sf::VideoMode(screenSizeX, screenSizeY), "Path finder");
	window.clear();
	ResetScreen(startMaze, window);
	window.display();

	// Create graph
	std::vector<Node> graph = CreateGraphe(startMaze);
	graph[FindNode(startPos, graph)].SetPred(0);

	//// Find path poorly
	//visitOrder.clear();
	//ClearArray(visitedTiles, SIZE_I 0, SIZE_J);
	//bool poorPathFind = FindPathPoorly(startPos.first, startPos.second, visitedTiles, startMaze);
	//ShowMoves(startMaze, window);

	// Breadth First Search
	std::cout	<< "-------------------------------------------------------------\n"
				<< "-------------------Breadth First Search----------------------\n" 
				<< "-------------------------------------------------------------"
				<< std::endl;
	visitOrder.clear();
	int BFSEndNode = BreadthFirstSearch(startPos, startMaze, graph);
	ShowMoves(BFSEndNode, startMaze, window, graph, startPos);

	// Depth First Search
	std::cout	<< "-------------------------------------------------------------\n"
				<< "---------------------Depth First Search----------------------\n"
				<< "-------------------------------------------------------------"
				<< std::endl;
	visitOrder.clear();
	int DFSEndNode = DepthFirstSearch(startPos, startMaze, graph);
	ShowMoves(DFSEndNode, startMaze, window, graph, startPos);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}

	return 0;
}

std::vector<Node> CreateGraphe(int * maze)
{
	std::vector<Node> graph;
	int graphSize = 0;

	// Create all nodes
	for (int i = 0; i < SIZE_I; i++)
	{
		for (int j = 0; j < SIZE_J; j++)
		{
			int posId = GetPosition(i, j);
			if (maze[posId] != 0)
			{
				Node newNode(maze[posId], std::pair<int, int>(i, j));
				newNode.SetId(graphSize);

				graph.push_back(newNode);
				graphSize++;
			}
		}
	}

	// Add edges
	for (int k = 0; k < graph.size(); k++)
	{
		Node* currentNode = &graph[k];
		int i = currentNode->GetNodePos().first;
		int j = currentNode->GetNodePos().second;
		
		// Check 4 neighbor nodes of currentNode 
		for (int k = 0; k < 4; k++)
		{
			int neighborI = i + rowNumber[k];
			int neighborJ = j + colNumber[k];

			// Check to see if current node is a path && is in bounds
			if (TileInBounds(neighborI, neighborJ) && maze[GetPosition(neighborI, neighborJ)] != 0)
			{
				int graphPositionOfNode = FindNode(std::pair<int, int>(neighborI, neighborJ), graph);
				currentNode->AddEdge(graphPositionOfNode);
			}
		}
	}
	std::cout << "------------------------- Created Graph ----------------------\n\n";
	return graph;
}

void ClearArray(int * someArray, int size)
{
	for (int i = 0; i < size; i++)
	{
		someArray[i] = 0;
	}
}

int FindNode(std::pair<int, int> position, std::vector<Node>& graph)
{
	Node nodeToTest(-1, position);
	bool foundNode = false;
	int p = 0;

	while (!foundNode && p < graph.size())
	{
		Node testNode = graph[p];
		if (nodeToTest.isEqual(testNode))
		{
			foundNode = true;
			return testNode.GetGraphePositionId();
		}
		else
		{
			p++;
		}
	}
	return -1;
}

int BreadthFirstSearch(std::pair<int,int> startPosition, int * maze, std::vector<Node>& graph)
{
	// Find start node in graph
	Node startNode = graph[FindNode(startPosition, graph)];		

	// Queue to keep track of nodes still left to check
	std::queue<Node> Q;
	Q.push(startNode);

	// Array to keep track of which nodes have already been visited
	std::vector<bool> visitedNode;
	for (int k = 0; k < graph.size(); k++)
	{
		visitedNode.push_back(false);
	}

	// Visit first node
	visitedNode[startNode.GetGraphePositionId()] = true;
	int nodesVisited = 0;
	//std::cout << nodesVisited << std::endl;

	while (!Q.empty())
	{
		Node currentNode = Q.front();
		Q.pop();

		nodesVisited++;
		//std::cout << nodesVisited << std::endl;

		visitOrder.push_back(currentNode.GetNodePos());

		// We've reached the end and the shortest distance is distance
		if (currentNode.GetValue() == END)
		{
			return currentNode.GetGraphePositionId();
		}

		// Add 4 neighbors to queue
		for (int k = 0; k < currentNode.GetEdges().size(); k++)
		{
			Node* newNode = &graph[currentNode.GetEdges()[k]];

			int i = newNode->GetNodePos().first + rowNumber[k];
			int j = newNode->GetNodePos().second + colNumber[k];

			// Check to see if current node is a path && is in bounds && we haven't visited it yet
			if (!visitedNode[newNode->GetGraphePositionId()])
			{
				visitedNode[newNode->GetGraphePositionId()] = true;
				newNode->SetPred(currentNode.GetGraphePositionId());
				newNode->SetDistanceToStart(currentNode.GetDistance() + 1);

				Q.push(*newNode);
			}
		}
	}
	std::cout << std::endl << "Number of nodes visited: " << nodesVisited << std::endl;
	return -1;
}

int DepthFirstSearch(std::pair<int, int> startPosition, int * maze, std::vector<Node>& graph)
{
	// Find start node in graph
	Node startNode = graph[FindNode(startPosition, graph)];

	// Queue to keep track of nodes still left to check
	std::stack<Node> Q;
	Q.push(startNode);

	// Array to keep track of which nodes have already been visited
	std::vector<bool> visitedNode;
	for (int k = 0; k < graph.size(); k++)
	{
		visitedNode.push_back(false);
	}

	// Visit first node
	visitedNode[startNode.GetGraphePositionId()] = true;
	int nodesVisited = 0;
	//std::cout << nodesVisited << std::endl;

	while (!Q.empty())
	{
		Node currentNode = Q.top();
		Q.pop();

		nodesVisited++;
		//std::cout << nodesVisited << std::endl;

		visitOrder.push_back(currentNode.GetNodePos());

		// We've reached the end and the shortest distance is distance
		if (currentNode.GetValue() == END)
		{
			return currentNode.GetGraphePositionId();
		}

		// Add 4 neighbors to queue
		for (int k = 0; k < currentNode.GetEdges().size(); k++)
		{
			Node* newNode = &graph[currentNode.GetEdges()[k]];

			int i = newNode->GetNodePos().first + rowNumber[k];
			int j = newNode->GetNodePos().second + colNumber[k];

			// Check to see if current node is a path && is in bounds && we haven't visited it yet
			if (!visitedNode[newNode->GetGraphePositionId()])
			{
				visitedNode[newNode->GetGraphePositionId()] = true;
				newNode->SetPred(currentNode.GetGraphePositionId());
				newNode->SetDistanceToStart(currentNode.GetDistance() + 1);

				Q.push(*newNode);
			}
		}
	}
	std::cout << std::endl << "Number of nodes visited: " << nodesVisited << std::endl;
	return -1;
}

void ShowMoves(int endPositionGraphId, int * startMaze, sf::RenderWindow &window, std::vector<Node>& graph, std::pair<int, int> startPos)
{
	// Draw each step
	for (int i = 0; i < visitOrder.size(); i++)
	{
		window.clear();
		ResetScreen(startMaze, window);
		for (int j = 0; j < i + 1; j++)
		{
			DrawTile(visitOrder[j].first, visitOrder[j].second, visitedColor, window);
		}
		DrawTile(startPos.first, startPos.second, startColor, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	// Draw final step and shortest path
	window.clear();
	ResetScreen(startMaze, window);
	for (int j = 0; j < visitOrder.size(); j++)
	{
		DrawTile(visitOrder[j].first, visitOrder[j].second, visitedColor, window);
	}

	std::vector<int> path;
	int crawl = endPositionGraphId;
	path.push_back(crawl);

	while (graph[crawl].GetPred() != 0) {
		path.push_back(graph[crawl].GetPred());
		crawl = graph[crawl].GetPred();
	}
	for (int i = 0; i < path.size(); i++)
	{
		Node currentNode = graph[path[i]];
		DrawTile(currentNode.GetNodePos().first, currentNode.GetNodePos().second, pathColor, window);
	}
	DrawTile(startPos.first, startPos.second, startColor, window);
	window.display();

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void ResetScreen(int * maze, sf::RenderWindow &window)
{
	for (int i = 0; i < SIZE_I; i++)
	{
		for (int j = 0; j < SIZE_J; j++)
		{
			DrawTile(i, j, GetColor(maze[GetPosition(i, j)]), window);
		}
	}
}

sf::Color GetColor(int value)
{
	sf::Color theColor = sf::Color::White;
	switch (value)
	{
	case 0: // Wall
		theColor = sf::Color(20,20,20);
		break;

	case 1: // Path
		theColor = sf::Color(255,248,240);
		break;

	case 8: // Start position
		theColor = startColor;
		break;

	case 9: // End
		theColor = endColor;
		break;

	default:
		break;
	}
	return theColor;
}

void DrawTile(int i, int j, sf::Color color, sf::RenderWindow &window)
{
	sf::VertexArray square(sf::Quads, 4);

	square[0].position = sf::Vector2f(j * (tileSizeX + interTileX), i * (tileSizeY + interTileY));
	square[1].position = sf::Vector2f((j + 1) * (tileSizeX + interTileX) - interTileX, i * (tileSizeY + interTileY));
	square[2].position = sf::Vector2f((j + 1) * (tileSizeX + interTileX) - interTileX, (i + 1) * (tileSizeY + interTileY) - interTileY);
	square[3].position = sf::Vector2f(j * (tileSizeX + interTileX), (i + 1) * (tileSizeY + interTileY) - interTileY);

	square[0].color = color;
	square[1].color = color;
	square[2].color = color;
	square[3].color = color;

	window.draw(square);
}

bool FindPathPoorly(int i, int j, int *visited, int *maze)
{
	int positionId = GetPosition(i, j);
	// Tile already visited or out of bounds or it's a wall, so we can't keep going down this path
	if (visited[positionId] == 1 || maze[positionId] == 0 || !TileInBounds(i, j))
		return false;

	// Visit tile
	visited[positionId] = 1;
	visitOrder.push_back(std::pair<int,int>(i,j));

	// We found a path, so return true
	if (maze[positionId] == 9)
		return true;

	// Else we're on walkable terrain or starting position
	else
	{
		return 
		// Check tile above current tile
		(FindPathPoorly(i - 1, j, visited, maze) ||
		// Check tile to the right of current tile
		FindPathPoorly(i, j + 1, visited, maze) ||
		// Check tile below current tile
		FindPathPoorly(i + 1, j, visited, maze) ||
		// Check tile to the left of current tile
		FindPathPoorly(i, j - 1, visited, maze));
	}
}

int GetPosition(int i, int j)
{
	return (i * SIZE_J + j);
}

void FindStartPosition(std::pair<int,int> *pathFound, int * maze)
{
	int i = 0, j = 0;
	bool foundStart = false;

	while (i < SIZE_I && !foundStart)
	{
		j = 0;
		while (j < SIZE_J && !foundStart)
		{
			if (maze[GetPosition(i,j)] == START)
			{
				foundStart = true;
				pathFound->first = i;
				pathFound->second = j;
			}
			j++;
		}
		i++;
	}
}

bool TileInBounds(int i, int j)
{
	return (i >= 0 && j >= 0 && i < SIZE_I && j < SIZE_J);
}

void DisplayMaze(int * maze)
{
	for (int i = 0; i < SIZE_I; i++)
	{
		for (int j = 0; j < SIZE_J; j++)
		{
			std::cout << maze[GetPosition(i, j)] << " ";
		}
		std::cout << "\n";
	}
}
