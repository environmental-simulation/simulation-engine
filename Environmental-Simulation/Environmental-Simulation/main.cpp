// Main file to run Learning Phase and Reacting Phase programs

#include "EnvSim GUI/window.h"
#include "LearningPhase/openNN.h"
#include "LearningPhase/Brain.h"
#include <vector>

int main()
{
	// Run Learning Phase program
	//RunNN();
	/*Brain brain = Brain();

	std::vector<std::vector<std::vector<double>>> test_input(5, std::vector<std::vector<double>>(5, std::vector<double>(4)));

	for (int row = 0; row < 5; row++)
	{
		for (int col = 0; col < 5; col++)
		{
			for (int v = 0; v < 4; v++)
			{
				test_input[row][col][v] = 0.5;
			}
		}
	}

	brain.predict(test_input);

	brain.print_debug();*/
	
	// Run Reacting Phase program
	Window window = Window();

	return 0;
}
