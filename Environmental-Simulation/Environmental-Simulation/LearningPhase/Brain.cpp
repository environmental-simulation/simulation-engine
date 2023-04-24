#include <Brain.h>
#include <math.h>
#include <iostream>

Brain::Brain()
{
	net.read_net("./model/", "Network_File.nn");
	last_prediction = std::vector<double>(2);
}

std::vector<double> Brain::compute_directional_inputs(const std::vector<std::vector<std::vector<double>>>& raw_inputs)
{
	std::vector<double> directional_inputs(16);

	// 0 - Human observation, 1 - Foliage level, 2 - Prey observation, 3 - Proximity to other pack(s)

	// North values.
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 5; col++)
		{
			directional_inputs[0] += raw_inputs[row][col][0];
			directional_inputs[1] += raw_inputs[row][col][1];
			directional_inputs[2] += raw_inputs[row][col][2];
			directional_inputs[3] += raw_inputs[row][col][3];
		}
	}

	// East values.
	for (int row = 0; row < 5; row++)
	{
		for (int col = 2; col < 5; col++)
		{
			directional_inputs[4] += raw_inputs[row][col][0];
			directional_inputs[5] += raw_inputs[row][col][1];
			directional_inputs[6] += raw_inputs[row][col][2];
			directional_inputs[7] += raw_inputs[row][col][3];
		}
	}

	// South values.
	for (int row = 2; row < 5; row++)
	{
		for (int col = 0; col < 5; col++)
		{
			directional_inputs[8]  += raw_inputs[row][col][0];
			directional_inputs[9]  += raw_inputs[row][col][1];
			directional_inputs[10] += raw_inputs[row][col][2];
			directional_inputs[11] += raw_inputs[row][col][3];
		}
	}

	// West values.
	for (int row = 0; row < 5; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			directional_inputs[12] += raw_inputs[row][col][0];
			directional_inputs[13] += raw_inputs[row][col][1];
			directional_inputs[14] += raw_inputs[row][col][2];
			directional_inputs[15] += raw_inputs[row][col][3];
		}
	}

	double inv_mag = 0.0;

	for (const double& val : directional_inputs)
	{
		inv_mag += val * val;
	}

	inv_mag = sqrt(inv_mag);

	if (inv_mag > 0.0) inv_mag = 1.0 / inv_mag;
	else inv_mag = 1.0;

	for (double& val : directional_inputs)
	{
		val *= inv_mag;
	}

	return directional_inputs;
}

std::vector<double> Brain::predict(std::vector<std::vector<std::vector<double>>> raw_inputs)
{
	Eigen::MatrixXd in(16, 1);

	std::vector<double> directional_inputs = compute_directional_inputs(raw_inputs);

	for (int i = 0; i < raw_inputs.size(); i++)
	{
		in(i, 0) = directional_inputs[i];
	}

	Eigen::MatrixXd out = net.predict(in.col(0));

	std::vector<double> prediction = {out(0, 0), out(1, 0)};

	last_prediction = prediction;

	return prediction;
}

void Brain::print_debug()
{
	std::cout << "Last prediction: [" << last_prediction(0, 0) << ", " << last_prediction(1, 0) << "]" << std::endl;
}