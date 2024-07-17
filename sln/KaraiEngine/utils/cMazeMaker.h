#pragma once
// This code is taken from Jaden Peterson's posting on codereview and slightly adapted:
// https://codereview.stackexchange.com/questions/135443/c-maze-generator

#include "../objects/object_3D/cObject3D.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct sMazeNode
{
	glm::vec2 pos = glm::vec2(0.0f);
	unsigned int id = 0;
	int weight = 0;

	cObject3D* pTreasure = nullptr;
	bool bHasTreasure = false;

	std::vector<sMazeNode*> vpAdjoiningNodes;
};

class cMazeMaker
{
public:
	cMazeMaker();
	~cMazeMaker();

	bool GenerateMaze(unsigned int width, unsigned int height);

	void InitializeMaze(unsigned int width, unsigned int height);

	void PrintMazeToFile(std::string fileName, char walls = 'X', char ways = ' ');

	void PrintMazeToConsole(char walls = 'X', char ways = ' ');

	bool LoadMazeFromFile(std::string fileName);

	void CopyToCharArray(void);

	void CreateGraph(void);

	std::vector< std::vector< std::vector< bool > > > vMaze;
	std::vector< sMazeNode* > vNodes;
	char theMaze[20][20] = { 0 };

private:
	int m_mazeSize[2];

	int m_startAxis;
	int m_startSide;

	std::vector< std::vector< int > > m_vDfsPath;

	static const unsigned int UP = 0;
	static const unsigned int DOWN = 1;
	static const unsigned int LEFT = 2;
	static const unsigned int RIGHT = 3;

	// Change this is the maze is wider or taller than this
	// (i.e. max size now is 10,000 x 10,000
	const unsigned int MAZE_ROW_MULTIPLE = 10'000;
	unsigned int m_CalculateID(unsigned int row, unsigned int col);
	void m_CalculateXZ(unsigned int id, unsigned int& x, unsigned int& z);
	sMazeNode* m_FindNodeByID(unsigned int id);

	bool m_RandomMove(bool first_move);
	void m_RandomPoint(bool part);
	bool m_ValidInteger(char* cstr);
};

