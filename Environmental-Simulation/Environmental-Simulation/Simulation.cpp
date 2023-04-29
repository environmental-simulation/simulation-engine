#include "Simulation.h"
#include <random>

Simulation::Simulation(Grid* simGrid, int timeFrame)
{
	grid = simGrid;

	models = nullptr;
	animalCount = 0;

	layers = nullptr;
	layerMute = nullptr;
	layerCount = 0;

	animals = nullptr;
	frames = new Frame[timeFrame];
	cellCount = nullptr;
	years = timeFrame;
}

int Simulation::GetIdx(AnimalModel* model)
{
	for (int i = 0; i < animalCount; i++)
	{
		if (models != nullptr && models[i] == model)
			return i;
	}

	return -1; // Error default
}

AnimalModel* Simulation::GetModel(int idx)
{
	if (animalCount <= idx)
		return nullptr;

	return models[idx];
}

void Simulation::AddAnimal(AnimalModel* model)
{
	AnimalModel** newModels = new AnimalModel*[animalCount + 1];
	int* newCount = new int[animalCount + 1];
	AnimalCell*** newCells = new AnimalCell**[animalCount + 1];

	for (int i = 0; i < animalCount; i++)
	{
		newModels[i] = models[i];
		newCount[i] = cellCount[i];
		newCells[i] = animals[i];
	}

	newModels[animalCount] = model;
	newCount[animalCount] = 0;
	newCells[animalCount] = nullptr;

	if (models != nullptr)
	{
		delete models;
		delete animals;
		delete cellCount;
	}
	models = newModels;
	cellCount = newCount;
	animals = newCells;
	animalCount++;
}

void Simulation::RemoveAnimal(int animalIdx)
{
	if (animalCount <= animalIdx)
		return;

	AnimalModel** newModels = new AnimalModel*[animalCount - 1];
	AnimalCell*** newAnimals = new AnimalCell**[animalCount - 1];
	int* newCells = new int[animalCount - 1];

	for (int i = 0, j = 0; i < animalCount; i++, j++)
	{
		if (j == animalIdx)
			i++;

		newModels[j] = models[i];
		newAnimals[j] = animals[i];
		newCells[j] = cellCount[i];
	}

	if (cellCount[animalIdx] > 0)
	{
		delete models[animalIdx];
		for (int i = 0; i < cellCount[animalIdx]; i++)
		{
			delete animals[animalIdx][i];
		}
		delete animals[animalIdx];
		
	}
	delete models;
	delete animals;
	delete cellCount;
	models = newModels;
	animals = newAnimals;
	cellCount = newCells;
	animalCount--;
}

// Environment Layers

EnvironmentLayer* Simulation::GetLayer(int idx)
{
	if (layerCount <= idx)
		return nullptr;

	return layers[idx];
}

bool Simulation::GetMute(int idx)
{
	if (layerCount <= idx)
		return true;

	return layerMute[idx];
}

void Simulation::AddLayer(EnvironmentLayer* layer)
{
	EnvironmentLayer** newLayers = new EnvironmentLayer*[layerCount + 1];
	bool* newMute = new bool[layerCount + 1];

	for (int i = 0; i < layerCount; i++)
	{
		newLayers[i] = layers[i];
		newMute[i] = layerMute[i];
	}

	newLayers[layerCount] = layer;
	newMute[layerCount] = true;

	if (layers != nullptr)
	{
		delete layers;
		delete layerMute;
	}
	layers = newLayers;
	layerMute = newMute;
	layerCount++;
}

void Simulation::RemoveLayer(int layerIdx)
{
	if (layers == nullptr)
		return;

	EnvironmentLayer** newLayers = new EnvironmentLayer*[layerCount - 1];
	bool* newMute = new bool[layerCount - 1];

	for (int i = 0, j = 0; i < layerCount; i++, j++)
	{
		if (j == layerIdx)
			i++;

		newLayers[j] = layers[i];
		newMute[j] = layerMute[i];
	}
}

void Simulation::MuteLayer(int idx)
{
	if (layerCount <= idx)
		return;

	layerMute[idx] = !layerMute[idx];
}

// Frames and Cells

AnimalCell* Simulation::GetCell(int animalIdx, int cellIdx)
{
	if (animalCount <= animalIdx || cellCount[animalIdx] <= cellIdx)
		return nullptr;

	return animals[animalIdx][cellIdx];
}

void Simulation::AddCells(int idx, int amount, int* xCoords, int* yCoords)
{
	if (animalCount <= idx)
		return;

	AnimalCell** newCells = new AnimalCell*[cellCount[idx] + amount];

	for (int i = 0; i < cellCount[idx]; i++)
	{
		newCells[i] = animals[idx][i];
	}

	for (int i = 0; i < amount; i++)
	{
		newCells[cellCount[idx] + i] = new AnimalCell(
			models[idx],
			grid,
			xCoords[i],
			yCoords[i],
			animals[idx],
			cellCount[idx] + i);
		
		cellCount[idx]++;
	}

	if (cellCount[idx] > 0)
		delete animals[idx];
	animals[idx] = newCells;
}

void Simulation::SetFrame(int year, bool season)
{
	AnimalCell*** frameCells = new AnimalCell**[animalCount];
	int* cellNum = new int[animalCount];

	for (int i = 0; i < animalCount; i++)
	{
		cellNum[i] = cellCount[i];
		frameCells[i] = new AnimalCell*[cellNum[i]];

		for (int j = 0; j < cellNum[i]; j++)
		{
			frameCells[i][j] = new AnimalCell(animals[i][j]);
		}
	}

	if (!frames[year * 2 + season].IsEmpty())
	{
		delete frames[year * 2 + season].cells;
		delete frames[year * 2 + season].cellCount;
	}

	frames[year * 2 + season] = Frame(
		season = season,
		frameCells,
		cellNum,
		animalCount);
}

Frame* Simulation::GetFrame(int year, bool season)
{
	return &frames[year * 2 + season];
}

void Simulation::LoadFrame(int year, bool season)
{
	for (int i = 0; i < animalCount; i++)
	{
		cellCount[i] = frames[year * 2 + season].cellCount[i];

		for (int j = 0; j < cellCount[i]; j++)
		{
			animals[i][j] = frames[year * 2 + season].cells[i][j];
		}
	}
}
