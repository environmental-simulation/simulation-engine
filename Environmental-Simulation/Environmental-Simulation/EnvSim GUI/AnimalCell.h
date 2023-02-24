#pragma once

#include "AnimalModel.h"

class AnimalCell
{
private:
	AnimalModel* model;
	Grid* grid;

	double lon;				// Cell Longitude (x-axis)
	double lat;				// Cell Latitude (y-axis)
	double horVector;		// Horizontal vector, + for East and - for West
	double verVector;		// Vertical vector, + for North and - for South
	int id;					// Optional value for Learning Phase if using a specific pack or other ID
	CellAction next;		// next action

	double GetDirectionalInput(int layer, Direction dir); // Helper function that obtains all the inputs of one dierction in a specific layer, scales them, and returns the sum

public:
	AnimalCell(AnimalModel m, double xCoord, double yCoord);

	void Observe(); // Observes nearby squares using coordinates and grid range to inputs

	void Act(); // Function that inputs observed values and changes vector and next action

	AnimalCell* Split();

	void Merge(AnimalCell* partner); // may be programmed to either be a "Consumptive" merge where one cell "eats" the other, or a mutual merge where they must both agree to merge

	void Die();
};
