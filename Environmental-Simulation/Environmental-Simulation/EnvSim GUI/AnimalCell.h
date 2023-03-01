#pragma once

#include "../LearningPhase/AnimalModel.h"

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
	AnimalCell* cells;		// List of current active cells of this species
	int cellsLive;			// The length of the above array, AKA how many cells exist currently

	double* GetClosestCells(); // Returns a double array of size 4 of closest cells, with no cells in that direction being 0

	double GetDirectionalInput(int layer, Direction dir); // Helper function that obtains all the inputs of one dierction in a specific layer, scales them, and returns the sum

	double* Observe(); // Observes nearby squares using coordinates and grid range to inputs

	void UpdateCellCount(); // Updates the cells array of each cell in the array

public:
	AnimalCell(AnimalModel* m, Grid* map, double xCoord, double yCoord, int cellCount);

	void Act(); // Function that inputs observed values, processes them from the model, and changes vector and next action

	AnimalCell* Split(); // Returns the new AnimalCell to be placed in any location by the Simulation

	void Merge(AnimalCell* partner); // may be programmed to either be a "Consumptive" merge where one cell "eats" the other, or a mutual merge where they must both agree to merge

	void Die();
};
