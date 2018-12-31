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
map<vector<int>, bool> locationStates;
vector<int> goals;
int nrOfAgents;
int AT_GOAL = -1;
void addEdge(vector<std::array<sf::Vertex, 2>> &linesInGrid, vector<sf::CircleShape*> &pointsInGrid, int x, int y) {
	std::array<sf::Vertex, 2> line{
		sf::Vertex(sf::Vector2f(pointsInGrid[x]->getPosition()) + sf::Vector2f(5,5)),
		sf::Vertex(sf::Vector2f(pointsInGrid[y]->getPosition()) + sf::Vector2f(5,5))
	};
	linesInGrid.push_back(line);
	adjacentNodes[x].push_back(y);
	adjacentNodes[y].push_back(x);
}

void comb(int num, vector<vector<int>> &result, vector<int> sofar, vector<int> &starts) {
	if (num != starts.size()) {
		//cout << "num is: " << starts[num];
		int agent = starts[num];
		if (agent != -1) {
			for (auto adjVertex : adjacentNodes[starts[num]]) {
				sofar.push_back(adjVertex);
				comb(num + 1, result, sofar, starts);
				sofar.pop_back();
			}
		}
		else {
			sofar.push_back(-1);
			comb(num + 1, result, sofar, starts);
			sofar.pop_back();
		}

	}
	else {
		result.push_back(sofar);
	}
}

bool collition(int p1, int p2, int p1´, int p2´) {
	if (p1´ == AT_GOAL || p2´ == AT_GOAL) {
		return false;
	}
	return p1´ == p2´ || ((p1´ == p2) && (p2´ == p1));
}

bool collitionInStateSpace(vector<int> origposvec, vector<int> newposvec) {
	//cout << "start collition " << "\n";
	bool collided = false;
	for (int i = 0; i < origposvec.size(); i++) {
		for (int j = i + 1; j < origposvec.size(); j++) {
			if (collition(origposvec[i], origposvec[j], newposvec[i], newposvec[j])) {
				collided = true;
			}
		}
	}
	//cout << "end collition " << "\n"; 
	return collided;
}
bool checkIfAgentsAreAtGoal(vector<int> &agentPosisitions, vector<int> &goalPositions) {
	//cout << "start check dsfddddddddddddddddddddd " << "\n";
	bool allAgentsAtGoal = true;
	for (int i = 0; i < goalPositions.size(); i++) {
		if (agentPosisitions[i] == goalPositions[i]) {
			agentPosisitions[i] = AT_GOAL;
		}
		if (agentPosisitions[i] != AT_GOAL) {
			allAgentsAtGoal = false;
		}
	}
	//cout << "end check " << "\n";
	return allAgentsAtGoal;
}

vector<int>& getPath(int parent[],vector<int> &result, int current) {
	result.push_back(current);
	if (parent[current] == -1) {
		return result;
	}
	getPath(parent, result, parent[current]);
}

int BFS(int result,vector<int> starts) {
	bool *visited = new bool[nrOfNodes];
	int *parent = new int[nrOfNodes];
	for (int i = 0; i < nrOfNodes; i++) {
		visited[i] = false;
	}
	queue<pair<int,vector<int>>> queueOfNodes;
	queueOfNodes.push(make_pair(0,starts));
	//visited[start] = true;
	//parent[start] = -1;


	while (!(queueOfNodes.empty())) {
		auto queuePair = queueOfNodes.front();
		starts = queuePair.second;
		queueOfNodes.pop();
		locationStates[starts] = true;
		//We have reached the end
		vector<vector<int>> result;
		vector<int> sofar;
		comb(0,result, sofar, starts);
		//cout << "this happens twice"; 
		//cout << result.size() << " should be 1";
		for (auto statevector = result.begin(); statevector != result.end();) {
			//cout << "current state is : " << (*statevector).size() << "big and amount of states are: " << result.size();
			if (!(collitionInStateSpace(starts, *statevector))) {
				//cout << "hello, no collition"; 
				if (checkIfAgentsAreAtGoal(*statevector, goals)) {
					cout << "a solution is found " << "\n";
					return queuePair.first + 1;
				}
				else {
					if (locationStates.count(*statevector) != 1) {
						//cout << "this is looping " << "\n";
						queueOfNodes.push(make_pair(queuePair.first + 1, *statevector));
					}				
				}
			}
			statevector++;
		}

	}
	cout << "algorithm has not found solution " << "\n";
	return -1;
}
int main()
{
	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");
	sf::CircleShape shape(100.f);
	//Variable nodes
	cout << "enter the number of nodes in the graph" << "\n";
	cin >> nrOfNodes;
	adjacentNodes = new vector<int>[nrOfNodes];
	int x = sqrt(nrOfNodes);
	int j = 0;
	int current = 0;
	int start, goal;
	for (int i = 0; i < nrOfNodes; i++) {
		current = i % x;
		if (current == 0) {
			j++;
		}
		auto point = new sf::CircleShape(5);
		point->setPosition(current * 55 + 50, j * 45 + 30);
		//point->setFillColor(sf::Color::Red);
		pointsInGrid.push_back(point);
		adjacentNodes[i].push_back(i);
	}

	int nrOfEdges;
	int nrOfAgents;
	cout << "enter the number of edges in the graph" << "\n";
	cin >> nrOfEdges;

	for (int i = 0; i < nrOfEdges; i++) {
		int from, to;
		cout << "enter from and enter to" << "\n";
		cin >> from >> to;
		addEdge(linesInGrid, pointsInGrid, from, to);
	}
	cout << "enter the number of agents " << "\n";
	cin >> nrOfAgents;
	vector<int> startAgents;
	for (int i = 0; i < nrOfAgents; i++) {
		cout << "enter start" << "\n";
		cin >> start;
		pointsInGrid[start]->setFillColor(sf::Color::Green);
		cout << "enter goal" << "\n";
		cin >> goal;
		pointsInGrid[goal]->setFillColor(sf::Color::Red);
		startAgents.push_back(start);
		goals.push_back(goal);
	}
	int resultOfMapf = BFS(0,startAgents);
	cout << "the result is: " << resultOfMapf;
	/*
	if (resvec[0] != -1) {
		for (auto res : resvec) {
			pointsInGrid[res]->setFillColor(sf::Color::Yellow);
		}
		cout << "the distance from start to end is: " << resvec.size() - 1 << "\n";
	}
	else {
		cout << "no path found from start to end" << "\n";
	}*/

	//pointsInGrid[start]->setFillColor(sf::Color::Green);
	//pointsInGrid[goal]->setFillColor(sf::Color::Red);

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