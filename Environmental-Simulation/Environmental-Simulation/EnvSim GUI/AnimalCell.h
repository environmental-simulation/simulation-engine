#pragma once

#include "../LearningPhase/AnimalModel.h"
#include "../Environment.h"


class AnimalCell
{
private:
	AnimalModel* model;
	Grid* grid;

	int lon;				// Cell Longitude (x-axis)
	int lat;				// Cell Latitude (y-axis)
	double horVector;		// Horizontal vector, + for East and - for West
	double verVector;		// Vertical vector, + for North and - for South
	CellAction next;		// next action
	AnimalCell** cells;		// List of current active cells of this species
	int cellsLive;			// The length of the above array, AKA how many cells exist currently

	double* GetClosestCells(); // Returns the pointer to the closest cell to THIS cell

	double GetDirectionalInput(int layer, Direction dir); // Helper function that obtains all the inputs of one dierction in a specific layer, scales them, and returns the sum

	double* Observe(); // Observes nearby squares using coordinates and grid range to inputs

	void UpdateCellCount(); // Updates the cells array of each cell in the array with THIS cell's cell array

public:
	AnimalCell();	// Default constructor

	AnimalCell(AnimalModel* m, Grid* map, int xCoord, int yCoord, AnimalCell** cellsAlive, int cellCount);

	void Act(); // Function that inputs observed values, processes them from the model, and changes vector and next action

	AnimalCell* Split(); // Returns the new AnimalCell to be placed in any location by the Simulation

	void Merge(); // Programmed to be a "Consumptive" merge of the closest other cell

	void Die();
};
