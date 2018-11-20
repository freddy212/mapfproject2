#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <array>
#include <math.h>
#include <queue>
using namespace std;
int nrOfNodes;
vector<sf::CircleShape*> pointsInGrid;
vector<std::array<sf::Vertex, 2>> linesInGrid;
vector<int> *adjacentNodes;
void addEdge(vector<std::array<sf::Vertex, 2>> &linesInGrid, vector<sf::CircleShape*> &pointsInGrid, int x, int y) {
	std::array<sf::Vertex, 2> line{
		sf::Vertex(sf::Vector2f(pointsInGrid[x]->getPosition()) + sf::Vector2f(5,5)),
		sf::Vertex(sf::Vector2f(pointsInGrid[y]->getPosition()) + sf::Vector2f(5,5))
	};
	linesInGrid.push_back(line);
	adjacentNodes[x].push_back(y);
	adjacentNodes[y].push_back(x);
}

vector<int>& getPath(int parent[],vector<int> &result, int current) {
	result.push_back(current);
	if (parent[current] == -1) {
		return result;
	}
	getPath(parent, result, parent[current]);
}

vector<int> BFS(int s , int g) {
	bool *visited = new bool[nrOfNodes];
	int *parent = new int[nrOfNodes];
	for (int i = 0; i < nrOfNodes; i++) {
		visited[i] = false;
	}
	queue<int> queueOfNodes;
	queueOfNodes.push(s);
	visited[s] = true;
	parent[s] = -1;


	while (!(queueOfNodes.empty())) {
		s = queueOfNodes.front();
		queueOfNodes.pop();
		//We have reached the end
		if (s == g) {
			vector<int> resultVector;
			return getPath(parent,resultVector,g);
		}

		for (auto node : adjacentNodes[s]) {
			if (!(visited[node])) {
				visited[node] = true;
				queueOfNodes.push(node);
				parent[node] = s;
			}
		}
	}
	vector<int> result;
	result.push_back(-1);
	return result;
}

void getPath() {

}

int main()
{
	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");
	sf::CircleShape shape(100.f);
	//Variable nodes
	cin >> nrOfNodes;
	adjacentNodes = new vector<int>[nrOfNodes];
	int x = sqrt(nrOfNodes);
	int j = 0;
	int current = 0;
	for (int i = 0; i < nrOfNodes; i++) {
		current = i % x;
		if (current == 0) {
			j++;
		}
		auto point = new sf::CircleShape(5);
		point->setPosition(current * 55 + 50, j * 45 + 30);
		point->setFillColor(sf::Color::Red);
		pointsInGrid.push_back(point);
	}

	int nrOfEdges;
	cin >> nrOfEdges;

	for (int i = 0; i < nrOfEdges; i++) {
		int from, to;
		cin >> from >> to;
		addEdge(linesInGrid, pointsInGrid, from, to);
	}

	vector<int> resvec = BFS(0, 8);

	for (auto res : resvec) {
		if (res != -1) {
			pointsInGrid[res]->setFillColor(sf::Color::Green);
		}
	}

	/*for (int i = 0; i < pointsInGrid.size(); i++) {
		if (i + 10 < pointsInGrid.size()) {
			std::array<sf::Vertex,2> line{
				sf::Vertex(sf::Vector2f(pointsInGrid[i]->getPosition()) + sf::Vector2f(5,5)),
				sf::Vertex(sf::Vector2f(pointsInGrid[i + 10]->getPosition()) + sf::Vector2f(5,5))
			};
			linesInGrid.push_back(line);

		}
		if (i + 1 < pointsInGrid.size() && (i + 1) % 10 != 0) {
			std::array<sf::Vertex, 2> line{
				sf::Vertex(sf::Vector2f(pointsInGrid[i]->getPosition()) + sf::Vector2f(5,5)),
				sf::Vertex(sf::Vector2f(pointsInGrid[i + 1]->getPosition()) + sf::Vector2f(5,5))
			};
			linesInGrid.push_back(line);
		}
	}
	*/

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		for (auto p : pointsInGrid) {
			window.draw(*p);
		}
		for (auto l : linesInGrid) {
			window.draw(l.data(), 2, sf::Lines);
		}
		window.display();
	}

	return 0;
}