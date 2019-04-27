#include "Node.h"

Node::Node(int value, std::pair<int, int> position)
{
	this->value = value;
	this->position = position;
	this->graphIdPosition = graphIdPosition;
}

Node::~Node()
{
}

