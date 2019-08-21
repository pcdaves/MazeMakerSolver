#include <cstdlib> //required for the random number gen.
#include <ctime>  //required to get system time, this is used ot generate random number that arnt really random :D
#include "Base.h"
#include <iostream>
#include <vector>  
#include "Cuboid.h"
#include <string>
#include <list>
#include <time.h>
#pragma once

using namespace std;


class CMaze : public CBase
{
public:
	CMaze(LPDIRECT3DDEVICE9 m_pD3DDevice);
	virtual ~CMaze(void); 
	bool ReturnWall(int cellX, int cellY, int wall); //returns which walls are up as a structure
	void ShowData(void); //prints maze to logfile
	char ReturnNeighbors(int X, int Y);
	void Build3D();
	void Render(LPDIRECT3DDEVICE9 m_pD3DDevice);
	void Connectify();
	void ReMake(void);

	bool PathFindMe(int startX, int startY, int finishX, int finishY);
	bool RenderPath(LPDIRECT3DDEVICE9 m_pD3DDevice);
	int CalcHScore(int startX, int startY, int finishX, int finishY, int gScore);
	string DiscoverNewCells(int x, int y);
	void UpdatePathTexture();
	
		
private:
	//Define a custom structure to hold cell information
	struct CELL_INFO
	{
		bool nWall, sWall, eWall, wWall;		//true = wall up, false = wall down
		bool visited; //used in maze generation
		
	}; 

	struct PATH_INFO
	{
		int x;
		int y;		
	}; 

	//struct to hold our info
	struct CELL	{
		int hScore;
		int gScore;
		int fScore;
		int x;
		int y;

		//parent cords, required to find the path
		int pX;
		int pY;
	};

	CELL_INFO cellArray[10][10];  //create array for maze already set to all walls up

	int cellTracker[100][2]; //keeps track of cells visited and in which order
	int cellsVisited;
	int totalCells; 


	int startX;
	int startY;

	int currentX;
	int currentY;
	int returned;

	CCuboid* floorTest[10][10];
	CCuboid* nWall[10][10];
	CCuboid* sWall[10][10]; 
	CCuboid* eWall[10][10]; 
	CCuboid* wWall[10][10];

	LPDIRECT3D9 m_pD3D;
	LPDIRECT3DDEVICE9 m_pD3DDevice;

	list<PATH_INFO> path;

	bool pathCalced; //lets us know when a path has been worked out

};
