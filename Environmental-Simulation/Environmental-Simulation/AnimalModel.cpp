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

// Animal Cell functions

double AnimalCell::GetDirectionalInput(int layer, Direction dir)
{/*
	double scale = 1.0 / (model->range + 1);
	Layer* map = grid->layers[layer];
	double sum = 0;
	int scanLon, scanLat;
	
	switch (dir)
	{
	case(north):
		for (scanLat = 0; scanLat <= model->range && lat + scanLat < grid->ySize; scanLat++)
		{
			scanLon = range - scanLat;
			while (scanLon <= model->range)
			{
				if (lon - scanLon >= 0)
					sum += map[lon - scanLon][lat + scanLat] * (scale * (scanLat + 1));

				sum += map[lon][lat + scanLat] * (scale * (scanlat + 1));

				if (lon + scanLon < grid->xSize)
					sum += map[lon + scanLon][lat + scanLat] * (scale * (scanLat + 1));
				
				scanLon++;
			}
		}
		break;
	case(east):
		for (scanLon = 0; scanLon <= model->range && lon + scanLon < grid->xSize; scanLon++)
		{
			scanLat = range - scanLon;
			while (scanLat <= model->range)
			{
				if (lat - scanLat >= 0)
					sum += map[lon + scanLon][lat - scanLat] * (scale * (scanLon + 1));

				sum += map[lon + scanLon][lat] * (scale * (scanlat + 1));

				if (lat + scanLat < grid->ySize)
					sum += map[lon + scanlon][lat + scanLat] * (scale * (scanLon + 1));

				scanLat++;
			}
		}
		break;
	case(south):
		for (scanLat = 0; scanLat <= model->range && lat - scanLat >= 0; scanLat++)
		{
			scanLon = range - scanLat;
			while (scanLon <= model->range)
			{
				if (lon - scanLon >= 0)
					sum += map[lon - scanLon][lat - scanLat] * (scale * (scanLat + 1));

				sum += map[lon][lat - scanLat] * (scale * (scanlat + 1));

				if (lon + scanLon < grid->xSize)
					sum += map[lon + scanLon][lat - scanLat] * (scale * (scanLat + 1));

				scanLon++;
			}
		}
		break;
	case(west):
		for (scanLon = 0; scanLon <= model->range && lon - scanLon < 0; scanLon++)
		{
			scanLat = range - scanLon;
			while (scanLat <= model->range)
			{
				if (lat - scanLat >= 0)
					sum += map[lon - scanLon][lat - scanLat] * (scale * (scanLon + 1));

				sum += map[lon - scanLon][lat] * (scale * (scanlat + 1));

				if (lat + scanLat < grid->ySize)
					sum += map[lon - scanlon][lat + scanLat] * (scale * (scanLon + 1));

				scanLat++;
			}
		}
		break;
	}*/
	return 0;
}

void AnimalCell::Observe()
{
	double dSum;
	double* newInputs = new double[model->GetInSize()];

	for (int i = 0; i < model->GetLayers(); i++)
	{
		for (int j = 0; j < cardinals; j++)
		{
			dSum = GetDirectionalInput(i, (Direction)j);
		}
	}
}
