/* Senior Project 2023 Section 1?
*  Reacting Phase Simulation
*  Simulation processor, creates each "frame" of the simulation
*/
#pragma once

#include "../AnimalCell.h"
#include "Environment.h"

struct EnvironmentLayer
{
	char* name;
	double** grid;
	int xSize;
	int ySize;
	// Color value or reference to value
};

struct Frame
{
public:
	bool season;							// True for Summer, False for Winter
	AnimalCell*** cells;
	int* cellCount;							// Array containing the size of each cell array
	int animalCount;

	// Constructor
	Frame()
	{
		season = false;
		cells = nullptr;
		cellCount = nullptr;
		animalCount = 0;
	}

	Frame(bool frameSeason, AnimalCell*** frameCells, int* frameCellCount, int frameAnimals)
	{
		season = frameSeason;
		cells = frameCells;
		cellCount = frameCellCount;
		animalCount = frameAnimals;
	}

	// Check if empty

	bool IsEmpty()
	{
		return (cells == nullptr) || (cellCount == nullptr);
	}

};

class Simulation
{
private:
	Grid* grid;								// Base grid simulation takes place on

	AnimalModel** models;					// Array of Animal Model pointers
	int animalCount;						// Count of animal species, also size of cellCount

	EnvironmentLayer** layers;				// Array of layers used
	bool* layerMute;						// Boolean array to show whether a layer has been "muted" or not
	int layerCount;							// Layer count

	AnimalCell*** animals;
	Frame* frames;							// The array of frames, even numbers are summer and odd are winter
	int* cellCount;							// Array consisting of how many animals per species
	int years;								// The amount of years in the simulation, years * 2 = frames length
	
public:
	Simulation(Grid* simGrid, int timeFrame);

	// Models and Model Index
	///////////////////////////////////////////////

	int GetIdx(AnimalModel* model);						// Gets the index of a specific animal model

	int GetAnimalCount() { return animalCount; }		// Gets the amount of animals

	AnimalModel* GetModel(int idx);						// Returns model at index if the array is at that point
	
	void AddAnimal(AnimalModel* model);					// Adds animals to the simulation

	void RemoveAnimal(int animalIdx);					// Removes an animal species from the simulation

	// Environmental Layers

	int GetLayerCount() { return layerCount; }			// Returns the layer count

	EnvironmentLayer* GetLayer(int idx);				// Returns the layer pointer if the index is valid

	bool GetMute(int idx);								// Returns mute status if index is valid
	
	void AddLayer(EnvironmentLayer* layer);				// Loads from a file

	void RemoveLayer(int layerIdx);						// Removes a specific layer
	
	bool* GetMute() { return layerMute; }				// Returns the array of muted layers

	void MuteLayer(int layerIdx);						// Prevents a layer from being calculated, if already muted undoes the mute

	// Frames and Cells
	///////////////////////////////////////////////

	AnimalCell* GetCell(int animalIdx, int cellIdx);	// Returns cell pointer of the specific species and cell

	void AddCells(int idx, int amount);					// Adds 'amount' cells of animal in animals['idx'] to the simulation at the START ONLY

	void SetFrame(int year, bool season);				// Saves the CURRENT set of cells as a frame to analyze later
	
	Frame* GetFrame(int year, bool season);				// Returns the frame for the specific time

	void LoadFrame(int year, bool season);				// Changes the frame to the frame saved at that time
};
