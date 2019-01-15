#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <array>
#include <math.h>
#include <queue>
#include <set>
using namespace std;

int nrOfNodes;
vector<sf::CircleShape*> pointsInGrid;
vector<std::array<sf::Vertex, 2>> linesInGrid;
vector<int> *adjacentNodes;
map<vector<int>, bool> visitedState;
vector<int> goalNodes;
int nrOfAgents;
int AT_GOAL = -1;
sf::Font font;

set<int> startAgentPositions;
set<pair<int, int>> agentIntervalPositions;

void loadFont() {
	if (!font.loadFromFile("arial.ttf"))
	{
		cout << "there was an error loading the font";
	}
}
void addEdge(vector<std::array<sf::Vertex, 2>> &linesInGrid, vector<sf::CircleShape*> &pointsInGrid, int x, int y) {
	std::array<sf::Vertex, 2> line{
		sf::Vertex(sf::Vector2f(pointsInGrid[x]->getPosition()) + sf::Vector2f(5,5)),
		sf::Vertex(sf::Vector2f(pointsInGrid[y]->getPosition()) + sf::Vector2f(5,5))
	};
	linesInGrid.push_back(line);
	adjacentNodes[x].push_back(y);
	adjacentNodes[y].push_back(x);
}

vector<int>& getPath(int parent[], vector<int> &result, int current, int time) {
	result.push_back(current);
	if (parent[current] == -1) {
		startAgentPositions.erase(current);
		return result;
	}
	cout << "current is : " << current << "time is " << time << "\n";
	agentIntervalPositions.insert(make_pair(current, time));
	getPath(parent, result, parent[current], time - 1);
}

vector<int> BFS(int s, int g) {
	bool *visited = new bool[nrOfNodes];
	int *parent = new int[nrOfNodes];
	for (int i = 0; i < nrOfNodes; i++) {
		visited[i] = false;
	}
	queue<pair<int,int>> queueOfNodes;
	queueOfNodes.push(make_pair(s, 0));
	visited[s] = true;
	parent[s] = -1;


	while (!(queueOfNodes.empty())) {
		s = queueOfNodes.front().first;
		int time = queueOfNodes.front().second;
		queueOfNodes.pop();
		//We have reached the end
		if (s == g) {
			vector<int> resultVector;
			return getPath(parent, resultVector, g, time);
		}

		for (auto node : adjacentNodes[s]) {
			if (!(visited[node])) {
				visited[node] = true;
				//Check if the node has been traversed by another agent 
				if (startAgentPositions.find(node) == startAgentPositions.end()) {
					if (agentIntervalPositions.find(make_pair(node, time + 1)) == agentIntervalPositions.end()) {
						queueOfNodes.push( make_pair(node, time + 1));
						parent[node] = s;
					}
				}
			}
		}
	}
	// a path could not be found, so the first element of the new vector is -1
	vector<int> result;
	result.push_back(-1);
	return result;
}

int getResultOfSupOptimalMapf(vector<int> &startpositions, vector<int> &endpositions) {
	int result = 0;
	for (auto pos : startpositions) {
		startAgentPositions.insert(pos);
	}
	for (int i = 0; i < startpositions.size(); i++) {
		vector<int> agentpath = BFS(startpositions[i], endpositions[i]);
		cout << "agent path size is: " << agentpath.size() << "\n";

		for (auto a : agentpath) {
			cout << a << "\n";
		}
		if (agentpath[0] != -1) {
			// the longest of these paths is the result of the algorithm
			result = max(result, (int)agentpath.size());
		}
		else {
			return -1;
		}
	}
	return result - 1; 
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

int StateBasedBFS(int result,vector<int> state) {
	bool *visited = new bool[nrOfNodes];
	int *parent = new int[nrOfNodes];
	for (int i = 0; i < nrOfNodes; i++) {
		visited[i] = false;
	}
	queue<pair<int,vector<int>>> queueOfStates;
	queueOfStates.push(make_pair(0,state));
	//visited[start] = true;
	//parent[start] = -1;


	while (!(queueOfStates.empty())) {
		auto queuePair = queueOfStates.front();
		state = queuePair.second;
		queueOfStates.pop();
		visitedState[state] = true;
		vector<vector<int>> result;
		vector<int> sofar;
		//get all possible combinations of states. save result in result
		comb(0,result, sofar, state);
		for (auto statevector = result.begin(); statevector != result.end();) {
			//cout << "current state is : " << (*statevector).size() << "big and amount of states are: " << result.size();
			if (!(collitionInStateSpace(state, *statevector))) {
				//cout << "hello, no collition"; 
				if (checkIfAgentsAreAtGoal(*statevector, goalNodes)) {
					cout << "a solution is found " << "\n";
					return queuePair.first + 1;
				}
				else {
					if (visitedState.count(*statevector) != 1) {
						//cout << "this is looping " << "\n";
						queueOfStates.push(make_pair(queuePair.first + 1, *statevector));
					}				
				}
			}
			statevector++;
		}

	}
	//cout << "algorithm has not found solution " << "\n";
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
	vector<sf::Text*> textvector;
	loadFont();
	vector<int> startNodes;
	for (int i = 0; i < nrOfAgents; i++) {
		cout << "enter start" << "\n";
		cin >> start;
		auto startPosText = new sf::Text();
		startPosText->setString(to_string(i + 1));
		startPosText->setFillColor(sf::Color::Green);
		startPosText->setPosition(pointsInGrid[start]->getPosition().x - 5, pointsInGrid[start]->getPosition().y - 15);
		startPosText->setFont(font);
		textvector.push_back(startPosText);
		//pointsInGrid[start]->setFillColor(sf::Color::Green);
		cout << "enter goal" << "\n";
		cin >> goal;
		auto endPosText = new sf::Text();
		endPosText->setString(to_string(i + 1));
		endPosText->setFillColor(sf::Color::Red);
		endPosText->setPosition(pointsInGrid[goal]->getPosition().x - 5, pointsInGrid[goal]->getPosition().y - 15);
		endPosText->setFont(font);
		textvector.push_back(endPosText);
		//pointsInGrid[goal]->setFillColor(sf::Color::Red);
		startNodes.push_back(start);
		goalNodes.push_back(goal);
	}
	int resultOfMapfSolution = -1;
	//resultOfMapfSolution = StateBasedBFS(0,startNodes);
	resultOfMapfSolution = getResultOfSupOptimalMapf(startNodes, goalNodes);
	if (resultOfMapfSolution == -1) {
		cout << "the algorithm could not find a solution";
	}
	else {
		cout << "the result is: " << resultOfMapfSolution;
	}


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
		for (auto text : textvector) {
			window.draw(*text);
		}
		for (auto l : linesInGrid) {
			window.draw(l.data(), 2, sf::Lines);
		}
		window.display();
	}

	return 0;
}