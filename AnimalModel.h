/* Senior Project 2023 Section 1?
*  Animal Model that can learn from data
*/
#pragma once

// Imports
#import "ELP.h"

const int NODE_LAYER_COUNT = 3;
const int NODE_COUNT_PER_LAYER = 7;

enum ModelOutputs
{
	verticalMag,
	horizontalMag,
	split,
	merge,
	die,
	nothing,

	Outputs // = 6
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
	double values;
	double bias; // todo: remove if unnecessary, maybe add a backpropagation ratio tracker
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
	double*** weights; // Connecting weights between each node

public:
	AnimalModel(char* inName, int size, int inSize, int r);

	void CopyInputs(double* newIn);

	void Process();

	int GetRange();

	int GetInSize(); // Returns the Input Size
	
	int GetLayers(); // Returns the number of layers the model was trained on

	int GetNodeCountPerLayer();

	double* GetOutputs();
};

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
