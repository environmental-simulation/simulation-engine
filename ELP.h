/* Senior Project 2023 Section 1?
*  Environmental Learning Phase format
*/
#pragma once

enum Direction
{
	north,
	east,
	south,
	west,

	cardinals // = 4
};

class Grid
{
	char* name;
	double** grid;
	int xSize;
	int ySize;
};
