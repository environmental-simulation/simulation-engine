/* Senior Project 2023 Section 1?
*  Simulation Display
*  Class that holds the data for the simulation after it has been simulated, GUI loads variables from this data
*/
#pragma once

#import "Simulation.h"

enum Season
{
	Spring,
	Summer,
	Autumn,
	Winter,

	seasons // = 4
};

struct Frame
{
	int year;
	Season season;
	AnimalCells* cells;
	int cellCount;
};

class Display
{
private:
	int* years;
	Frame* frames; // 4 per year
	int timeLen;

public:

};
