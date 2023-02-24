/* Senior Project 2023 Section 1
*  Animal model that can learn from data and react in an alternate environment
*/
#include "AnimalModel.h"

// Animal Model functions

AnimalModel::AnimalModel(char* inName, int size, int inSize, int r)
{
	name = inName;
	nameSize = size;
	inputs = new double[inSize];
	inputSize = inSize;
	outputs = new double[Outputs];
	range = r;
	hNodes = new node*[NODE_LAYER_COUNT];
	for (int i = 0; i < NODE_LAYER_COUNT; i++)
	{
		hNodes[i] = new node[NODE_COUNT_PER_LAYER];
		hNodes[i]->values = 0;
		// todo:Set bias or othersuch
	}
	weights = new double**[NODE_LAYER_COUNT + 2];
	weights[0] = new double*[inputSize];
	for (int i = 0; i < inputSize; i++)
	{
		weights[0][i] = new double[NODE_COUNT_PER_LAYER];
		// todo:random value added dependent on seed
	}
	for (int i = 0; i < NODE_LAYER_COUNT; i++)
	{
		weights[i + 1] = new double* [NODE_COUNT_PER_LAYER];

		for (int j = 0; j < NODE_COUNT_PER_LAYER; j++)
		{
			weights[i + 1][j] = new double[NODE_COUNT_PER_LAYER];
			// todo:random value added dependent on seed
		}
	}
	weights[NODE_LAYER_COUNT + 1] = new double*[NODE_COUNT_PER_LAYER];
	for (int i = 0; i < NODE_COUNT_PER_LAYER; i++)
	{
		weights[NODE_LAYER_COUNT + 1][i] = new double[Outputs];
		// todo:random value added dependent on seed
	}
}

void AnimalModel::CopyInputs(double* newIn)
{
	for (int i = 0; i < Outputs; i++)
	{
		outputs[i] = newIn[i];
	}
}

void AnimalModel::Process()
{
	//for (int i = 0; i < NODE_COUNT_PER_LAYER; i++)
	//{
	//	for (int j = 0; j < inputSize; j++)
	//	{
	//		hNodes[0][i] = weights[0][i][j] * inputs[j];
	//	}
	//}

	//for (int i = 1; i < NODE_LAYER_COUNT; i++)
	//{
	//	for (int j = 0; j < NODE_COUNT_PER_LAYER; j++)
	//	{
	//		for (int k = 0; k < NODE_COUNT_PER_LAYER; k++)
	//		{
	//			hNodes[i][j] = weights[i][j][k] * hNodes[i - 1][k];
	//		}
	//	}
	//}

	//for (int i = 0; i < Outputs; i++)
	//{
	//	for (int j = 0; j < NODE_COUNT_PER_LAYER; j++)
	//	{
	//		ModelOutputs[i] = weights[NODE_COUNT_PER_LAYER + 1][i][j] * hNodes[NODE_LAYER_COUNT][j];
	//	}
	//}
}

int AnimalModel::GetRange() { return range; }

int AnimalModel::GetInSize() { return inputSize; }

int AnimalModel::GetLayers() { return NODE_LAYER_COUNT; }

int AnimalModel::GetNodeCountPerLayer() { return NODE_COUNT_PER_LAYER; }

double* AnimalModel::GetOutputs() { return outputs; };

