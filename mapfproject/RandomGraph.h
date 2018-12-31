#pragma once
#include <vector>

int AT_GOAL = -1; 

using namespace std;
int getRandomInteger(int from, int to) {
	srand(time(NULL));
	int result = rand() % to + from;
	return result;
}

vector<pair<int,int>>& getEdges(int from, int to, int nrofedges) {
	vector<pair<int,int>> resultVec;
	for (int i = 0; i < nrofedges; i++) {
		int fromNode = getRandomInteger(from, to);
		int toNode = getRandomInteger(from, to);
		resultVec.push_back(make_pair(fromNode, toNode));
	}
	return resultVec;
}

bool checkIfAgentsAreAtGoal(vector<int> &agentPosisitions, vector<int> goalPositions) {
	bool allAgentsAtGoal = true;
	for (int i = 0; i < goalPositions.size(); i++) {
		if (agentPosisitions[i] == goalPositions[i]) {
			agentPosisitions[i] = AT_GOAL;
		}
		if(agentPosisitions[i] != AT_GOAL){
			allAgentsAtGoal = false;
		}
	}
	return allAgentsAtGoal;
}