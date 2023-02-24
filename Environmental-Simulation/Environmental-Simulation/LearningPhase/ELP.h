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

class Layer
{
public:
	char* name;			// Name of the layer
	int len;			// Length of name
	double** layer;		// 2D array of environmental values
	int xSize;			// Size of x-axis
	int ySize;			// Size of y-axis
	int xOffset;		// Possible horizontal offset or change in relation to other layers
	int yOffset;		// Possible vertical offset or change in relation to other layers
	bool muted;			// Boolean controlling if the layer is muted (affecting simulation) or not, unused in Learning Phase
};

class Grid
{
public:
	char* name;			// Name of the grid
	int len;			// Length of name
	Layer* layers;		// Array of layers in use
	int xSize;			// Horizontal size
	int ySize;			// Vertical size
};
