/* Senior Project 2023 Section 1?
*  Reacting Phase Simulation
*  Simulation processor, creates each "frame" of the simulation
*/
#pragma once

#import "AnimalModel.h"

class EnvironmentLayer
{
	char* name;
	double** grid;
	int xSize;
	int ySize;
	// Color value or reference to value
};

class Simulation
{
private:
	AnimalModel** animals; // Array of Animal Model pointers
	AnimalCell*** cells; // Array of Animal Cell arrays
	int* cellCount; // Array containing the size of each cell arrays
	int animalCount; // Count of animals

	EnvironmentLayer** layers;
	bool* layerMute;
	int layerCount;
	
public:
	Simulation() { };

	void AddAnimal(AnimalModel* model, int cells); // Adds animals and randomly places cells

	void HideAnimal(int animalIdx); // Hides a species from view

	void RemoveAnimal(int animalIdx);

	void AddLayer(EnvironmentLayer* layer); // Loads from a file

	void HideLayer(int layerIdx); // Hides a layer from view

	void MuteLayer(int layerIdx); // Prevents a layer from being calculated

	void RemoveLayer(int layerIdx);
};
