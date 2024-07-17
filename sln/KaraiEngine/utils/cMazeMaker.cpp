#include "cMazeMaker.h"

#include <iostream>
#include <fstream>

cMazeMaker::cMazeMaker()
{
	this->m_mazeSize[0] = 5;
	this->m_mazeSize[1] = 5;

    this->m_startAxis = 0;
    this->m_startSide = 0;
}

cMazeMaker::~cMazeMaker()
{
	for (unsigned int i = 0; i < vNodes.size(); i++)
	{
		delete vNodes[i];
	}
}

bool cMazeMaker::GenerateMaze(unsigned int width, unsigned int height)
{
	this->m_mazeSize[0] = width;
	this->m_mazeSize[1] = height;

	if ((width < 5) || (height < 5))
	{
		return false;
	}

	// The width and height must be greater than or equal to 5 or it won't work
	// The width and height must be odd or else we will have extra walls
	for (int a = 0; a < 2; a++)
	{
		if (this->m_mazeSize[a] < 5)
		{
			this->m_mazeSize[a] = 5;
		}
		else if (this->m_mazeSize[a] % 2 == 0)
		{
			this->m_mazeSize[a]--;
		}
	}

	this->InitializeMaze(width, height);
	this->m_RandomPoint(false);
	this->m_RandomPoint(true);

	bool first_move = true;
	bool success = true;


	while ((int)this->m_vDfsPath.size() > 1 - first_move)
	{
		if (!success)
		{
			this->m_vDfsPath.pop_back();

			if (!first_move && this->m_vDfsPath.size() > 2)
			{
				this->m_vDfsPath.pop_back();
			}
			else
			{
				break;
			}

			success = true;
		}

		while (success)
		{
			success = this->m_RandomMove(first_move);

			if (first_move)
			{
				first_move = false;
			}
		}
	}

	return true;
}

void cMazeMaker::InitializeMaze(unsigned int width, unsigned int height)
{
	this->m_mazeSize[0] = width;
	this->m_mazeSize[1] = height;

	for (int a = 0; a < this->m_mazeSize[1]; a++)
	{
		for (int b = 0; b < this->m_mazeSize[0]; b++)
		{
			bool is_border;

			if (a == 0 || a == this->m_mazeSize[1] - 1 ||
				b == 0 || b == this->m_mazeSize[0] - 1)
			{
				is_border = true;
			}
			else
			{
				is_border = false;
			}

			std::vector< bool > new_cell = { true, is_border };

			if ((unsigned int)a + 1 > this->vMaze.size())
			{
				std::vector< std::vector< bool > > new_row = { new_cell };

				this->vMaze.push_back(new_row);
			}
			else
			{
				this->vMaze[a].push_back(new_cell);
			}
		}
	}
}

bool cMazeMaker::LoadMazeFromFile(std::string fileName)
{
	std::ifstream mazeNameFile(fileName.c_str());
	if (!mazeNameFile.is_open())
	{
		return false;
	}

	std::string currentWord;

	while (mazeNameFile >> currentWord)
	{
		if (currentWord == "DUNGEON_BEGIN")
		{
			break;
		}
	}

	int mazeRow = 0;

	while (mazeNameFile >> currentWord)
	{
		if (currentWord == "DUNGEON_END")
		{
			break;
		}

		//std::vector< std::vector< std::vector< bool > > > vMaze;
		for (int i = 0; i != currentWord.size(); i++)
		{
			if (currentWord[i] == 'X')
			{
				vMaze[mazeRow][i][0] = true;
			}
			if (currentWord[i] == '.')
			{
				vMaze[mazeRow][i][0] = false;
			}
		}

		mazeRow++;
	}

	return true;
}

void cMazeMaker::PrintMazeToConsole(char walls, char ways)
{
	for (unsigned int a = 0; a < this->vMaze.size(); a++)
	{
		for (unsigned int b = 0; b < this->vMaze[a].size(); b++)
		{
			if (this->vMaze[a][b][0])
			{
				std::cout << walls;
				//std::cout << "X";
				//std::cout << '\u2500';
				//std::cout << (char)0xC5;
				//std::cout << (char)0xBF; // ┐ (191)
				//std::cout << (char)0xC0; // └ (192)
				//std::cout << (char)0xDA; // ┌ (218)
				//std::cout << (char)0xD9; // ┘ (217)
			}
			else
			{
				std::cout << ways;
				//std::cout << " ";
			}
		}

		std::cout << std::endl;
	}
	return;
}

void cMazeMaker::PrintMazeToFile(std::string filename, char walls, char ways)
{
	std::ofstream mazeFile(filename.c_str());

	for (unsigned int a = 0; a < this->vMaze.size(); a++)
	{
		for (unsigned int b = 0; b < this->vMaze[a].size(); b++)
		{
			if (this->vMaze[a][b][0])
			{
				mazeFile << walls;
			}
			else
			{
				mazeFile << ways;
			}
		}

		mazeFile << std::endl;
	}

	mazeFile.close();

	return;
}

void cMazeMaker::CopyToCharArray()
{
	for (unsigned int a = 0; a < this->vMaze.size(); a++)
	{
		for (unsigned int b = 0; b < this->vMaze[a].size(); b++)
		{
			if (this->vMaze[a][b][0])
			{
				theMaze[a][b] = 'X';
			}
			else
			{
				theMaze[a][b] = ' ';
			}
		}
	}
	return;
}

void cMazeMaker::CreateGraph(void)
{	
	for (unsigned int a = 0; a < this->vMaze.size(); a++)
	{
		for (unsigned int b = 0; b < this->vMaze[a].size(); b++)
		{
			if (this->vMaze[a][b][0])
			{
				// Do nothing!?				
			}
			else
			{
				sMazeNode* pNewNode = new sMazeNode();
				pNewNode->id = this->m_CalculateID(a, b);
				pNewNode->pos = glm::vec2(b, a);
				this->vNodes.push_back(pNewNode);
			}
		}
	}

	// Go through these nodes and using the original 2D array, 
	// see what nodes are connected to this one.
	for (unsigned int a = 0; a < this->vMaze.size(); a++)
	{
		for (unsigned int b = 0; b < this->vMaze[a].size(); b++)
		{
			if (!this->vMaze[a][b][0])
			{
				// Is a node
				unsigned int id = this->m_CalculateID(a, b);
				// Find associated node in vector
				sMazeNode* pTestNode = this->m_FindNodeByID(id);

				// Check left node
				if (b > 0) // Column is left most
				{
					// Node to the left
					if (!this->vMaze[a][b - 1][0])
					{
						// It's a node
						unsigned int leftNodeID = this->m_CalculateID(a, b - 1);
						pTestNode->vpAdjoiningNodes.push_back(this->m_FindNodeByID(leftNodeID));
					}
				}

				// Check right node
				if (b < (this->vMaze[a].size() - 1)) // Column is right most
				{
					// Node to the right
					if (!this->vMaze[a][b + 1][0])
					{
						// It's a node
						unsigned int rightNodeID = this->m_CalculateID(a, b + 1);
						pTestNode->vpAdjoiningNodes.push_back(this->m_FindNodeByID(rightNodeID));
					}
				}

				// Check above node
				if (a > 0)	// Row is top most
				{
					// Node to the above
					if (!this->vMaze[a - 1][b][0])
					{
						// It's a node
						unsigned int aboveNodeID = this->m_CalculateID(a - 1, b);
						pTestNode->vpAdjoiningNodes.push_back(this->m_FindNodeByID(aboveNodeID));
					}
				}

				// Check below node
				if (a < (this->vMaze.size() - 1)) // Row is bottom most
				{
					// Node to the above
					if (!this->vMaze[a + 1][b][0])
					{
						// It's a node
						unsigned int belowNodeID = this->m_CalculateID(a + 1, b);
						pTestNode->vpAdjoiningNodes.push_back(this->m_FindNodeByID(belowNodeID));
					}
				}
			}
		}
	}

	return;
}


sMazeNode* cMazeMaker::m_FindNodeByID(unsigned int ID)
{
	for (sMazeNode* pCurrentNode : vNodes)
	{
		if (pCurrentNode->id == ID)
		{
			return pCurrentNode;
		}
	}

    return nullptr;
}

unsigned int cMazeMaker::m_CalculateID(unsigned int row, unsigned int col)
{
    return row * MAZE_ROW_MULTIPLE + col;
}

void cMazeMaker::m_RandomPoint(bool part)
{
	int axis;
	int side;

	if (!part)
	{
		axis = rand() % 2;
		side = rand() % 2;

		this->m_startAxis = axis;
		this->m_startSide = side;
	}
	else
	{
		bool done = false;

		while (!done)
		{
			axis = rand() % 2;
			side = rand() % 2;

			if (axis != this->m_startAxis ||
				side != this->m_startSide)
			{
				done = true;
			}
		}
	}

	std::vector< int > location = { 0, 0 };

	if (!side)
	{
		location[!axis] = 0;
	}
	else
	{
		location[!axis] = this->m_mazeSize[!axis] - 1;
	}

	location[axis] = 2 * (rand() % ((this->m_mazeSize[axis] + 1) / 2 - 2)) + 1;

	if (!part)
	{
		this->m_vDfsPath.push_back(location);
	}

	this->vMaze[location[1]][location[0]][0] = false;
	this->vMaze[location[1]][location[0]][1] = true;

	return;
}

bool cMazeMaker::m_RandomMove(bool firstMove)
{
	int random_neighbor;
	std::vector< std::vector< int > > unvisited_neighbors;

	for (int direction = 0; direction < 4; direction++)
	{
		int possible_pmd[2] = { 0, 0 };

		if (direction == UP)
		{
			possible_pmd[1] = -1;
		}
		else if (direction == DOWN)
		{
			possible_pmd[1] = 1;
		}
		else if (direction == LEFT)
		{
			possible_pmd[0] = -1;
		}
		else
		{
			possible_pmd[0] = 1;
		}

		if (this->m_vDfsPath.back()[0] + possible_pmd[0] * 2 > 0 &&
			this->m_vDfsPath.back()[0] + possible_pmd[0] * 2 < this->m_mazeSize[0] - 1 &&
			this->m_vDfsPath.back()[1] + possible_pmd[1] * 2 > 0 &&
			this->m_vDfsPath.back()[1] + possible_pmd[1] * 2 < this->m_mazeSize[1] - 1)
		{
			if (! vMaze[this->m_vDfsPath.back()[1] + possible_pmd[1] * 2]
				[this->m_vDfsPath.back()[0] + possible_pmd[0] * 2][1])
			{
				std::vector< int > possible_move_delta = { possible_pmd[0], possible_pmd[1] };

				unvisited_neighbors.push_back(possible_move_delta);
			}
		}
	}

	if (unvisited_neighbors.size() > 0)
	{
		random_neighbor = rand() % unvisited_neighbors.size();

		for (int a = 0; a < !firstMove + 1; a++)
		{
			std::vector< int > new_location;

			new_location.push_back(this->m_vDfsPath.back()[0] + unvisited_neighbors[random_neighbor][0]);
			new_location.push_back(this->m_vDfsPath.back()[1] + unvisited_neighbors[random_neighbor][1]);

			this->m_vDfsPath.push_back(new_location);

			this->vMaze[this->m_vDfsPath.back()[1]][this->m_vDfsPath.back()[0]][0] = false;
			this->vMaze[this->m_vDfsPath.back()[1]][this->m_vDfsPath.back()[0]][1] = true;
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool cMazeMaker::m_ValidInteger(char* cstr)
{
	std::string str(cstr);

	for (char& c : str)
	{
		if (!isdigit(c))
		{
			return false;
		}
	}

	return true;
}

