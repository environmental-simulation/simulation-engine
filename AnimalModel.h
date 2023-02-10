/* Senior Project 2023 Section 1?
*  Animal Model that can learn from data
*/
#pragma once

// Imports
#import "ELP.h"

enum ModelOutputs
{
	northMag,
	southMag,
	westMag,
	eastMag,
	split,
	merge,
	die,

	Outputs // = 7
};

enum CellAction
{
	act_split,
	act_merge,
	act_die,
	act_nothing,
};

struct node
{
	double weight;
	double bias;
};

class AnimalModel
{
private:
	char* name;
	int nameSize;
	double* inputs;
	int inputSize;
	double* outputs; // Size = Outputs
	int range; // Range of "sight" for model
	node** hNodes; // Hidden nodes, two dimensional array of nodes

public:
	AnimalModel(int inSize, int r);

	void CopyInputs(double newIn);

	void Process();

	int GetRange();

	int GetInSize();

	double* GetOutputs();
};

class AnimalCell
{
private:
	AnimalModel* model;
	double lon;
	double lat;
	double horVector; // Horizontal vector, + for East and - for West
	double verVector; // Vertical vector, + for North and - for South
	int id; // Optional value for Learning Phase if using a specific pack or other ID
	CellAction next; // next action

public:
	AnimalCell(AnimalModel m, double xCoord, double yCoord);

	void Observe(Grid* grid); // Observes nearby squares using coordinates and grid range to inputs

	AnimalCell* Split();

	void Merge(AnimalCell* partner); // may be programmed to either be a "Consumptive" merge where one cell "eats" the other, or a mutual merge where they must both agree to merge

	void Die();
};
