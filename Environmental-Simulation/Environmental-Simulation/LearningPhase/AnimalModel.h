/* Senior Project 2023 Section 1?
*  Animal Model that can learn from data
*/
#pragma once

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
