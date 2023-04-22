#include <MiniDNN.h>

#include <string>
#include <vector>
#include <iostream>

using namespace MiniDNN;

class Brain
{
private:
	Network net;

public:
	Brain();

	// Converts the 100 points of data from the grid into 16 directional points that can be vectorized into the network.
	std::vector<double> compute_directional_inputs(const std::vector<std::vector<std::vector<double>>>& raw_inputs);

	// 'raw_inputs' should be a 3D vector of size 5x5x4, with values in range [0, 1].
	std::vector<double> predict(std::vector<std::vector<std::vector<double>>> raw_inputs);
};