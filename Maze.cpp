#include "Maze.h"


CMaze::CMaze(LPDIRECT3DDEVICE9 m_pD3DDevice) // creates maze
{
	pathCalced = false;
	LogInfo("<li>Entering maze class ... stopping log.");
	StopLogging();
	 for (int i=0; i < 10; i++)
	{
		for(int j=0; j<10; j++)
		{
			cellArray[i][j].visited = false;
			cellArray[i][j].nWall = true;
			cellArray[i][j].sWall = true;
			cellArray[i][j].eWall = true;
			cellArray[i][j].wWall = true;

			floorTest[i][j] = NULL;
			nWall[i][j] = NULL;
			sWall[i][j] = NULL;
			eWall[i][j] = NULL;
			wWall[i][j] = NULL;

			nWall[i][j] = new CCuboid(m_pD3DDevice,5.0,5.0,5.0);
			nWall[i][j]->SetTexture("N.JPG");
			sWall[i][j] = new CCuboid(m_pD3DDevice,5.0,5.0,5.0);
			sWall[i][j]->SetTexture("S.JPG");
			eWall[i][j] = new CCuboid(m_pD3DDevice,5.0,5.0,5.0);
			eWall[i][j]->SetTexture("E.JPG");
			wWall[i][j] = new CCuboid(m_pD3DDevice,5.0,5.0,5.0);
			wWall[i][j]->SetTexture("W.JPG");

			floorTest[i][j] = new CCuboid(m_pD3DDevice,5.0,5.0,5.0);
			floorTest[i][j]->SetTexture("floor.JPG");
		}
	}
	m_pD3D = NULL;
	


	int cellTracker[100][2]; //keeps track of cells visited and in which order
	int cellsVisited =0;
	int totalCells =100-1; //now i would put 99 but thats confusing becuse ill forget to take the 1 off to make it not an endless loop :D

	srand((unsigned)time(0));  //requires ctime header
	int startX = (rand()%9)+1; //produces a random number and finds the modulus 10
	int startY = (rand()%9)+1;

	//sets current cell info
	int currentX = startX;
	int currentY = startY;
	cellTracker[0][0] = startX;
	cellTracker[0][1] = startY;
	cellArray[startX][startY].visited = true;
	int workingCell=0;
	int loopNum=0;

	m_pD3DDevice = m_pD3DDevice;

	LogInfo("creating cubes");
	

	char direction ='u'; // direction to go from current cell, possible returns are n/s/e/w/u - U for unset, will point out a fail.

	LogInfo("<li>loaded maze class, start do loop");

	/*	create a CellStack (LIFO) to hold a list of cell locations 
		set TotalCells = number of cells in grid 
		choose a cell at random and call it CurrentCell 
		set VisitedCells = 1 
  
			while VisitedCells < TotalCells 

			find all neighbors of CurrentCell with all walls intact  
			if one or more found 
				choose one at random 
				knock down the wall between it and CurrentCell 
				push CurrentCell location on the CellStack 
				make the new cell CurrentCell 
				add 1 to VisitedCells 
			else 
				pop the most recent cell entry off the CellStack 
				make it CurrentCell 
			endIf */

		 
	 while(cellsVisited<totalCells)
	{ 
		LogInfo("<li><li><li> start of while loop");

		direction='u';
		direction = ReturnNeighbors(currentX,currentY);
		
		if(direction=='u'){LogInfo("direction = u");}
		if(direction=='n'){LogInfo("direction = n");}
		if(direction=='s'){LogInfo("direction = s");}
		if(direction=='w'){LogInfo("direction = w");}
		if(direction=='e'){LogInfo("direction = e");}

		switch(direction)
		{
			
			case 'u':
				workingCell--;  //my so called back tracker, just steps back till we are out of a dead end.
				currentX=cellTracker[workingCell][0];
				currentY=cellTracker[workingCell][1];
				LogInfo("<li>In back tracker, working cell = %d, currentX = %d, CurrentY = %d",workingCell,currentX,currentY);
				break;

			case 'n':  //rest of the cases are movements n=north ect
				cellArray[currentX][currentY].nWall = false;
				cellArray[currentX+1][currentY].sWall = false;
				cellArray[currentX+1][currentY].visited = true;

				cellsVisited++;
				workingCell++;

				currentX++;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			case 's':
				cellArray[currentX][currentY].sWall = false;
				cellArray[currentX-1][currentY].nWall = false;
				cellArray[currentX-1][currentY].visited = true;

				cellsVisited++;
				workingCell++;

				currentX--;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			case 'e':
				cellArray[currentX][currentY].eWall = false;
				cellArray[currentX][currentY+1].wWall = false;
				cellArray[currentX][currentY+1].visited = true;

				cellsVisited++;
				workingCell++;

				currentY++;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			case 'w':
				cellArray[currentX][currentY].wWall = false;
				cellArray[currentX][currentY-1].eWall = false;
				cellArray[currentX][currentY-1].visited = true;

				cellsVisited++;
				workingCell++;

				currentY--;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			default: //default case incase a returned value is wrong
				LogInfo("ERROR IN SWITCH(DIRECTION)... Returned = %d",direction);
				exit(666);
				break;
		}
			loopNum++;
		LogInfo("loop number %d passed, visited cells = %d",loopNum,cellsVisited);
		
	}
	LogInfo("ZOMG IT FINISHED! I HESITATE TO SAY WORKED :D");
	ShowData();
	Connectify(); //removes dead ends on maze
	ShowData();
	
	LogInfo("<br><br>****outputting wall data****<br>");
	for(int j=0;j<10;j++)
	{
		for(int i=0;i<10;i++)
		{
			LogInfo("<br>Return north wall %d,%d, wall up=%d",j,i,ReturnWall(j,i,1));
			LogInfo("Return east wall %d,%d, wall up=%d",j,i,ReturnWall(j,i,2));
			LogInfo("Return south wall %d,%d, wall up=%d",j,i,ReturnWall(j,i,3));
			LogInfo("Return west wall %d,%d, wall up=%d<br>",j,i,ReturnWall(j,i,4));
		}
	}
	LogInfo("<br>****outputting wall data - finished!****<br><br>");

	PathFindMe(0,0,0,0);
	
}

	
CMaze::~CMaze(void)
{
}

void CMaze::Connectify()
{
	int walls=0;
	//removes dead ends.
	for (int i=0; i < 10; i++)
	{
		for(int j=0; j < 10; j++)
		{
			walls=0;

			if(cellArray[i][j].nWall){walls++;}
			if(cellArray[i][j].sWall){walls++;}
			if(cellArray[i][j].eWall){walls++;}
			if(cellArray[i][j].wWall){walls++;}

		
			if((walls>=3)&&(i!=0)&&(i!=9)&&(j!=0)&&(j!=9))
			{
				int randNum = rand()%3;
				int a=0;
				for(a=0; a<4; a++)
				{

				if((cellArray[i][j].nWall)&&(randNum==0))
				{
					cellArray[i][j].nWall = false;
					cellArray[i+1][j].sWall = false;
					break;
				} else {randNum++;}

				if((cellArray[i][j].sWall)&&(randNum==1))
				{
					cellArray[i][j].sWall = false;
					cellArray[i-1][j].nWall = false;
					break;
				} else {randNum++;}

				if((cellArray[i][j].eWall)&&(randNum==2))
				{
					cellArray[i][j].eWall = false;
					cellArray[i][j+1].wWall = false;
					break;
				} else {randNum++;}

				if((cellArray[i][j].wWall)&&(randNum==3))
				{
					cellArray[i][j].wWall = false;
					cellArray[i][j-1].eWall = false;
					break;
				} else {randNum++;}

				if(randNum>4){randNum=0;}
				}
			}
			else
			{
					if((i==0)&&(walls>=3))
					{
						if(cellArray[i][j].nWall)
						{
							cellArray[i][j].nWall = false;
							cellArray[i+1][j].sWall = false;
							continue;
						}

						if((cellArray[i][j].eWall)&&(j!=9)) //makes sure the outside walls cant be knocked down!
						{
							cellArray[i][j].eWall = false;
							cellArray[i][j+1].wWall = false;
							continue;
						}

						if((cellArray[i][j].wWall)&&(j!=0))
						{
							cellArray[i][j].wWall = false;
							cellArray[i][j-1].eWall = false;
							continue;
						}

					}

					if((i==9)&&(walls>=3))
					{
						if((cellArray[i][j].sWall))
						{
							cellArray[i][j].sWall = false;
							cellArray[i-1][j].nWall = false;
							continue;
						} 
						
						if((cellArray[i][j].eWall)&&(j!=9)) //makes sure the outside walls cant be knocked down!
						{
							cellArray[i][j].eWall = false;
							cellArray[i][j+1].wWall = false;
							continue;
						}

						if((cellArray[i][j].wWall)&&(j!=0))
						{
							cellArray[i][j].wWall = false;
							cellArray[i][j-1].eWall = false;
							continue;
						}
					
					
					}		


					if((j==0)&&(walls>=3))
					{
						if(cellArray[i][j].eWall)
						{
							cellArray[i][j].eWall = false;
							cellArray[i][j+1].wWall = false;
							continue;
						}

						if((cellArray[i][j].nWall)&&(i!=9)) //makes sure the outside walls cant be knocked down!
						{
							cellArray[i][j].nWall = false;
							cellArray[i+1][j].sWall = false;
							continue;
						}

						if((cellArray[i][j].sWall)&&(i!=0))
						{
							cellArray[i][j].sWall = false;
							cellArray[i+1][j].nWall = false;
							continue;
						}

					}

					if((j==9)&&(walls>=3))
					{
						if(cellArray[i][j].wWall)
						{
							cellArray[i][j].wWall = false;
							cellArray[i][j-1].eWall = false;
							continue;
						}

						if((cellArray[i][j].nWall)&&(i!=9)) //makes sure the outside walls cant be knocked down!
						{
							cellArray[i][j].nWall = false;
							cellArray[i+1][j].sWall = false;
							continue;
						}

						if((cellArray[i][j].sWall)&&(i!=0))
						{
							cellArray[i][j].sWall = false;
							cellArray[i+1][j].nWall = false;
							continue;
						}

					}
		
			}}
	}
}


void CMaze::Render(LPDIRECT3DDEVICE9 m_pD3DDevice)
{	
	D3DXMATRIX matFloor1, matWall1, matWall2; //scaling matrix
	D3DXMATRIX matMoveFloor1, matMoveWall1, matMoveWall2, matMoveWall3, matMoveWall4; //translation matrix
	
	//Size
	D3DXMatrixScaling(&matFloor1, 5.0f, 2.0f, 5.0f);
	D3DXMatrixScaling(&matWall1, 1.0f, 3.0f, 5.0f);
	D3DXMatrixScaling(&matWall2, 5.0f, 3.0f, 1.0f);

			//StartLogging();
			LogInfo("entering matrix for loop");
	for (int i=0; i < 10; i++)
	{
		for(int j=0; j < 10; j++)
		{

	//Movement 
	
	D3DXMatrixTranslation(&matMoveFloor1, 0.0f+(i*25), 0.0f, 0.0f+(j*25));

	D3DXMatrixTranslation(&matMoveWall1, 10.0f+(i*25), 2.5f, 0.0f+(j*5));
	D3DXMatrixTranslation(&matMoveWall2, 0.0f+(i*5), 2.5f, 10.0f+(j*25));
	D3DXMatrixTranslation(&matMoveWall3, -10.0f+(i*25), 2.5f, 0.0f+(j*5));
	D3DXMatrixTranslation(&matMoveWall4, 0.0f+(i*5), 2.5f, -10.0f+(j*25));


	D3DXMatrixMultiply(&matMoveFloor1, &matFloor1, &matMoveFloor1);

	D3DXMatrixMultiply(&matMoveWall1, &matMoveWall1, &matWall1);
	D3DXMatrixMultiply(&matMoveWall2, &matMoveWall2, &matWall2);
	D3DXMatrixMultiply(&matMoveWall3, &matMoveWall3, &matWall1);
	D3DXMatrixMultiply(&matMoveWall4, &matMoveWall4, &matWall2);


	m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &matMoveFloor1);
	floorTest[i][j]->Render();

	m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &matMoveWall1);
	if(cellArray[i][j].nWall){nWall[i][j]->Render();}
	LogInfo("<br><br><li>cellArray[%d][%d].nWall=%d",i,j,cellArray[i][j].nWall);
	
	m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &matMoveWall2);
	if(cellArray[i][j].eWall){eWall[i][j]->Render();}
	LogInfo("<li>cellArray[%d][%d].eWall=%d",i,j,cellArray[i][j].eWall);

	m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &matMoveWall3);
	if(cellArray[i][j].sWall){sWall[i][j]->Render();}
	LogInfo("<li>cellArray[%d][%d].sWall=%d",i,j,cellArray[i][j].sWall);

	m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &matMoveWall4);
	if(cellArray[i][j].wWall){wWall[i][j]->Render();}
	LogInfo("<li>cellArray[%d][%d].wWall=%d",i,j,cellArray[i][j].wWall);


		}
	}
	if(pathCalced)	{	
	UpdatePathTexture();
	}
}



char CMaze::ReturnNeighbors(int X, int Y)
{
	LogInfo("<li>entered Return neighbors");
	std::vector<char> directionArray(4, 'u');
	LogInfo("created vector");

	if((X+1<=9)&&(!cellArray[X+1][Y].visited))
	{		 directionArray[0]='n';	}

	if((X-1>=0)&&(!cellArray[X-1][Y].visited))
	{		directionArray[1]='s';	}

	if((Y+1<=9)&&(!cellArray[X][Y+1].visited))
	{		directionArray[2]='e';	}

	if((Y-1>=0)&&(!cellArray[X][Y-1].visited))
	{		directionArray[3]='w';		}

	LogInfo("finished if's");
	int randGen = rand()%4;
	int randSave = randGen; //used to check if the same number is reached
	LogInfo("generated random numbers");
	while(directionArray[randGen]=='u')
	{
		LogInfo("start while");
		randGen++;

		if(randGen >3)
		{	randGen=0;	}

		if(randSave==randGen)
		{	return 'u';	LogInfo("returned u, save==gen");	}

		LogInfo("in return neighbors while loop");
				
	}
	
	if(directionArray[randGen]=='n')
	{	LogInfo("direction is N");		}
	if(directionArray[randGen]=='e')
	{	LogInfo("direction is E");		}
	if(directionArray[randGen]=='s')
	{	LogInfo("direction is S");		}
	if(directionArray[randGen]=='w')
	{	LogInfo("direction is W");		}

	return directionArray[randGen];
}


bool CMaze::ReturnWall(int cellX, int cellY, int wall) //returns which walls are up as a structure
{
	if(wall==1){return cellArray[cellX][cellY].nWall;} //true = wall up
	if(wall==2){return cellArray[cellX][cellY].eWall;}
	if(wall==3){return cellArray[cellX][cellY].sWall;}
	if(wall==4){return cellArray[cellX][cellY].wWall;}
	
	//if code below exec, an error will have happened
	//Error();
	return 3;
}
void CMaze::ShowData(void) //prints maze to logfile - only required for debugging
{
	int y0,y1,y2,y3,y4,y5,y6,y7,y8,y9;



	LogInfo("<li>******Print cell info Started******<li>");
	for(int X=9;X>(0-1);X--)
	{
		y0=0;
		y1=0;
		y2=0;
		y3=0;
		y4=0;
		y5=0;
		y6=0;
		y7=0;
		y8=0;
		y9=0;

	if(cellArray[X][0].nWall){y0++;}
	if(cellArray[X][0].eWall){y0++;}
	if(cellArray[X][0].sWall){y0++;}
	if(cellArray[X][0].wWall){y0++;}
	
	if(cellArray[X][1].nWall){y1++;}
	if(cellArray[X][1].eWall){y1++;}
	if(cellArray[X][1].sWall){y1++;}
	if(cellArray[X][1].wWall){y1++;}

	if(cellArray[X][2].nWall){y2++;}
	if(cellArray[X][2].eWall){y2++;}
	if(cellArray[X][2].sWall){y2++;}
	if(cellArray[X][2].wWall){y2++;}

	if(cellArray[X][3].nWall){y3++;}
	if(cellArray[X][3].eWall){y3++;}
	if(cellArray[X][3].sWall){y3++;}
	if(cellArray[X][3].wWall){y3++;}

	if(cellArray[X][4].nWall){y4++;}
	if(cellArray[X][4].eWall){y4++;}
	if(cellArray[X][4].sWall){y4++;}
	if(cellArray[X][4].wWall){y4++;}

	if(cellArray[X][5].nWall){y5++;}
	if(cellArray[X][5].eWall){y5++;}
	if(cellArray[X][5].sWall){y5++;}
	if(cellArray[X][5].wWall){y5++;}

	if(cellArray[X][6].nWall){y6++;}
	if(cellArray[X][6].eWall){y6++;}
	if(cellArray[X][6].sWall){y6++;}
	if(cellArray[X][6].wWall){y6++;}

	if(cellArray[X][7].nWall){y7++;}
	if(cellArray[X][7].eWall){y7++;}
	if(cellArray[X][7].sWall){y7++;}
	if(cellArray[X][7].wWall){y7++;}

	if(cellArray[X][8].nWall){y8++;}
	if(cellArray[X][8].eWall){y8++;}
	if(cellArray[X][8].sWall){y8++;}
	if(cellArray[X][8].wWall){y8++;}

	if(cellArray[X][9].nWall){y9++;}
	if(cellArray[X][9].eWall){y9++;}
	if(cellArray[X][9].sWall){y9++;}
	if(cellArray[X][9].wWall){y9++;}
	
	LogInfo("%d%d%d%d%d%d%d%d%d%d",y0,y1,y2,y3,y4,y5,y6,y7,y8,y9);
	}
	LogInfo("<li>******Print cell info finished******");
}

void CMaze::ReMake(void)
{
	pathCalced = false;
	path.clear();

	for (int i=0; i < 10; i++)
	{
		for(int j=0; j<10; j++)
		{
			cellArray[i][j].visited = false;
			cellArray[i][j].nWall = true;
			cellArray[i][j].sWall = true;
			cellArray[i][j].eWall = true;
			cellArray[i][j].wWall = true;
			floorTest[i][j]->SetTexture("floor.jpg");

		}
	}

	


	int cellTracker[100][2]; //keeps track of cells visited and in which order
	cellsVisited =0;
	totalCells =100-1; //now i would put 99 but thats confusing becuse ill forget to take the 1 off to make it not an endless loop :D

	startX = (rand()%9)+1; //produces a random number and finds the modulus 10
	startY = (rand()%9)+1;

	//sets current cell info
	currentX = startX;
	currentY = startY;
	cellTracker[0][0] = startX;
	cellTracker[0][1] = startY;
	cellArray[startX][startY].visited = true;
	int workingCell=0;
	int loopNum=0;

	char direction ='u'; // direction to go from current cell, possible returns are n/s/e/w/u - U for unset, will point out a fail.

	LogInfo("<li>loaded maze class, start do loop");

	/*	create a CellStack (LIFO) to hold a list of cell locations 
		set TotalCells = number of cells in grid 
		choose a cell at random and call it CurrentCell 
		set VisitedCells = 1 
  
			while VisitedCells < TotalCells 

			find all neighbors of CurrentCell with all walls intact  
			if one or more found 
				choose one at random 
				knock down the wall between it and CurrentCell 
				push CurrentCell location on the CellStack 
				make the new cell CurrentCell 
				add 1 to VisitedCells 
			else 
				pop the most recent cell entry off the CellStack 
				make it CurrentCell 
			endIf */

		 
	 while(cellsVisited<totalCells)
	{ 
		LogInfo("<li><li><li> start of while loop");

		direction = ReturnNeighbors(currentX,currentY);
		
		if(direction=='u'){LogInfo("direction = u");}
		if(direction=='n'){LogInfo("direction = n");}
		if(direction=='s'){LogInfo("direction = s");}
		if(direction=='w'){LogInfo("direction = w");}
		if(direction=='e'){LogInfo("direction = e");}

		switch(direction)
		{
			
			case 'u':
				workingCell--;  //my so called back tracker, just steps back till we are out of a dead end.
				currentX=cellTracker[workingCell][0];
				currentY=cellTracker[workingCell][1];
				LogInfo("<li>In back tracker, working cell = %d, currentX = %d, CurrentY = %d",workingCell,currentX,currentY);
				break;

			case 'n':  //rest of the cases are movements n=north ect
				cellArray[currentX][currentY].nWall = false;
				cellArray[currentX+1][currentY].sWall = false;
				cellArray[currentX+1][currentY].visited = true;

				cellsVisited++;
				workingCell++;

				currentX++;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			case 's':
				cellArray[currentX][currentY].sWall = false;
				cellArray[currentX-1][currentY].nWall = false;
				cellArray[currentX-1][currentY].visited = true;

				cellsVisited++;
				workingCell++;

				currentX--;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			case 'e':
				cellArray[currentX][currentY].eWall = false;
				cellArray[currentX][currentY+1].wWall = false;
				cellArray[currentX][currentY+1].visited = true;

				cellsVisited++;
				workingCell++;

				currentY++;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			case 'w':
				cellArray[currentX][currentY].wWall = false;
				cellArray[currentX][currentY-1].eWall = false;
				cellArray[currentX][currentY-1].visited = true;

				cellsVisited++;
				workingCell++;

				currentY--;

				cellTracker[workingCell][0]=currentX;
				cellTracker[workingCell][1]=currentY;

				break;

			default: //default case incase a returned value is wrong
				LogInfo("ERROR IN SWITCH(DIRECTION)... Returned = %d",direction);
				exit(666);
				break;
		}
			loopNum++;
		LogInfo("loop number %d passed, visited cells = %d",loopNum,cellsVisited);
		
	}
	ShowData();
	Connectify(); //removes dead ends on maze
	ShowData();
	PathFindMe(0,0,0,0);

}

bool CMaze::PathFindMe(int startX, int startY, int finishX, int finishY)		{
	path.clear();

	//for the sake of conveniance i am going to use 0,0 and 9,9 as the start and finish.
	startX = 0;
	startY = 0;
	finishX = 9;
	finishY = 9;
	
	//reset visited flags
	for (int i=0; i < 10; i++)
	{
		for(int j=0; j<10; j++)
		{
			cellArray[i][j].visited = false;

		}
	}
	

	list<CELL*> openList;
	list<CELL*> closedList;

	//add the first node to the open list
	CELL * tmp = new CELL();
	CELL * currentCell = tmp;
	CELL * newCell = new CELL();
	currentCell->x = currentCell->x;
	currentCell->y = currentCell->y;
	currentCell->pX = currentCell->x;
	currentCell->pY = currentCell->y;
	currentCell->gScore = 10;
	currentCell->hScore = CalcHScore(currentCell->x, currentCell->y, finishX, finishY, currentCell->gScore);
	currentCell->fScore = currentCell->gScore + currentCell->hScore;
	openList.push_back(currentCell);
	cellArray[currentCell->x][currentCell->y].visited = true;




	while((currentCell->x != finishX) || (currentCell->y != finishY))		{
	//find all open cells next to current cell and add to open list
	string tmpString = DiscoverNewCells(currentCell->x, currentCell->y);

	

	if(!cellArray[currentCell->x][currentCell->y].nWall  && cellArray[currentCell->x + 1][currentCell->y].visited == false)
	{		 
		CELL * newCell = new CELL();
		newCell->x = currentCell->x + 1;
		newCell->y = currentCell->y;
		newCell->pX = currentCell->x;
		newCell->pY = currentCell->y;
		newCell->gScore = 10;
		newCell->hScore = CalcHScore(newCell->x, newCell->y, finishX, finishY, newCell->gScore);
		newCell->fScore = newCell->gScore + newCell->hScore;
		openList.push_back(newCell);
		cellArray[currentCell->x + 1][currentCell->y].visited = true;
	}

	if(!cellArray[currentCell->x][currentCell->y].sWall  && cellArray[currentCell->x - 1][currentCell->y].visited == false)
	{		 
		CELL * newCell = new CELL();
		newCell->x = currentCell->x - 1;
		newCell->y = currentCell->y;
		newCell->pX = currentCell->x;
		newCell->pY = currentCell->y;
		newCell->gScore = 10;
		newCell->hScore = CalcHScore(newCell->x, newCell->y, finishX, finishY, newCell->gScore);
		newCell->fScore = newCell->gScore + newCell->hScore;
		openList.push_back(newCell);
		cellArray[currentCell->x - 1][currentCell->y].visited = true;
	}

	if(!cellArray[currentCell->x][currentCell->y].eWall  && cellArray[currentCell->x][currentCell->y + 1].visited == false)
	{		 
		CELL * newCell = new CELL();
		newCell->x = currentCell->x;
		newCell->y = currentCell->y + 1;
		newCell->pX = currentCell->x;
		newCell->pY = currentCell->y;
		newCell->gScore = 10;
		newCell->hScore = CalcHScore(newCell->x, newCell->y, finishX, finishY, newCell->gScore);
		newCell->fScore = newCell->gScore + newCell->hScore;
		openList.push_back(newCell);
		cellArray[currentCell->x][currentCell->y + 1].visited = true;
	}

	if(!cellArray[currentCell->x][currentCell->y].wWall  && cellArray[currentCell->x][currentCell->y - 1].visited == false)
	{		 
		CELL * newCell = new CELL();
		newCell->x = currentCell->x;
		newCell->y = currentCell->y - 1;
		newCell->pX = currentCell->x;
		newCell->pY = currentCell->y;
		newCell->gScore = 10;
		newCell->hScore = CalcHScore(newCell->x, newCell->y, finishX, finishY, newCell->gScore);
		newCell->fScore = newCell->gScore + newCell->hScore;
		openList.push_back(newCell);
		cellArray[currentCell->x][currentCell->y - 1].visited = true;
	}

	
	//add current cell to the closed list and remove it from the open list.
	closedList.push_back(currentCell);

	for (list<CELL*>::iterator it = openList.begin(); it != openList.end(); it++)	{
		if(it._Ptr->_Myval == currentCell)	{
			openList.erase(it);
			break;
		}

	}

	//pick lowest F score from the open list and make it current
	int F = 10000;
	for (list<CELL*>::iterator it = openList.begin(); it != openList.end(); it++)	{
		if(it._Ptr->_Myval->fScore < F)		{
			F = it._Ptr->_Myval->fScore;
			currentCell = it._Ptr->_Myval;
		}
	}
	//repeate
	}

	//in reverse order add the cells to the path list, so...
	//while (current x != start x || current y != start y)
	while((currentCell->x != startX) || (currentCell->y != startY))		{

	//path.add(current.pX, current.pY)
		
	for (list<CELL*>::iterator it = closedList.begin(); it != closedList.end(); it++)	{
		if(it._Ptr->_Myval->x == currentCell->pX)		{
			if(it._Ptr->_Myval->y == currentCell->pY)		{
				PATH_INFO tmp;
				tmp.x = currentCell->x;
				tmp.y = currentCell->y;
				path.push_front(tmp);
				currentCell = it._Ptr->_Myval;
			}
		}
	}
	}
	//it wont add the last 1 during the loop, so maunally add it.
	PATH_INFO start;
	start.x = startX;
	start.y = startY;
	path.push_front(start);

	pathCalced = true;
	return true;
}


string CMaze::DiscoverNewCells(int x, int y)		{

	string temp;

	if((x+1<=9)&&(!cellArray[x+1][y].visited))
	{		 temp =+'n';	}

	if((x-1>=0)&&(!cellArray[x-1][y].visited))
	{		temp =+'s';	}

	if((y+1<=9)&&(!cellArray[x][y+1].visited))
	{		temp =+'e';	}

	if((y-1>=0)&&(!cellArray[x][y-1].visited))
	{		temp =+'w';		}

	return temp;

}
int CMaze::CalcHScore(int startX, int startY, int finishX, int finishY, int gScore)		{
	if((startX == finishX) || (startY == finishY))	{
		return 0;
	}
	return ((finishX - startX - 1) + (finishY - startY)) * gScore;
}

bool CMaze::RenderPath(LPDIRECT3DDEVICE9 m_pD3DDevice)		{

	//cellArray
	return true;
}

void CMaze::UpdatePathTexture()	{
	if(path.size() == 0)	{return;}
	if ((GetTickCount() % 10) == 0)	{
		floorTest[path.begin()->x][path.begin()->y]->SetTexture("path.jpg");
		path.pop_front();
	}

}