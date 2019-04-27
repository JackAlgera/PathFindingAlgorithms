#pragma once
#include <iostream>
#include <vector>
#include <list>

class Node
{
public:
	Node(int value, std::pair<int, int> position);
	~Node();

	int GetValue() { return value; };
	int GetDistance() { return distanceToStart; };
	int GetGraphePositionId() { return graphIdPosition; };
	std::pair<int, int> GetNodePos() { return position; };
	std::vector<int> GetEdges() { return edges; };
	int GetPred() { return pred; };

	void SetPred(int pred) { this->pred = pred; };
	void SetId(int newId) { this->graphIdPosition = newId; };
	void SetDistanceToStart(int newDistance) { this->distanceToStart = newDistance; };

	bool isEqual(Node otherNode) { return (this->position.first == otherNode.position.first && this->position.second == otherNode.position.second); };
	void AddEdge(int nodeId) { edges.push_back(nodeId); };

private:
	int value;
	int graphIdPosition;
	int distanceToStart = -10;
	int pred = -1;

	std::pair<int, int> position;
	std::vector<int> edges;
};

