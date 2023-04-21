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
	Frame(bool frameSeason, AnimalCell*** frameCells, int* frameCellCount, int frameAnimals)
	{
		season = frameSeason;
		cells = frameCells;
		cellCount = frameCellCount;
		animalCount = frameAnimals;
	}
};

class Simulation
{
private:
	AnimalModel** animals;					// Array of Animal Model pointers
	int animalCount;						// Count of animal species, also size of cellCount

	EnvironmentLayer** layers;				// Array of layers used
	bool* layerMute;						// Boolean array to show whether a layer has been "muted" or not
	int layerCount;							// Layer count

	Frame* frames;							// The array of frames, even numbers are summer and odd are winter
	int years;								// The amount of years in the simulation, years * 2 = frames length
	
public:
	Simulation() { };

	// Models and Model Index
	///////////////////////////////////////////////

	int GetIdx(AnimalModel* model);						// Gets the index of a specific animal model

	int GetAnimalCount();								// Gets the amount of animals

	void AddAnimal(AnimalModel* model);					// Adds animals to the simulation

	void RemoveAnimal(int animalIdx);					// Removes an animal species from the simulation

	// Environmental Layers

	int GetLayerCount();								// Returns the layer count

	void AddLayer(EnvironmentLayer* layer);				// Loads from a file

	void RemoveLayer(int layerIdx);						// Removes a specific layer
	
	bool* GetMute();									// Returns the array of muted layers

	void MuteLayer(int layerIdx);						// Prevents a layer from being calculated, if already muted undoes the mute

	// Frames and Cells
	///////////////////////////////////////////////

	void AddCells(int idx, int amount);					// Adds 'amount' cells of animal in animals['idx'] to the simulation at the START ONLY

	void SetFrame(int year, bool season, Frame* frame);
};
