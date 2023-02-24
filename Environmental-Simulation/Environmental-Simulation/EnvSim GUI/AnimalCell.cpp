
#include "AnimalCell.h"

// Animal Cell functions

double* AnimalCell::GetClosestCells()
{
	int* minCardinals = new int[cardinals];
	minCardinals[north] = grid->ySize;
	minCardinals[east] = grid->xSize;
	minCardinals[south] = grid->ySize;
	minCardinals[west] = grid->xSize;
	double* out = new double[cardinals];

	for (int i = 0; i < grid->speciesCount; i++)
	{
		// Make sure it's using the proper index of the species it is
		if (grid->cells[i, 0]->model == model)
		{
			for (int j = 0; j < grid->cellCount[i]; j++)
			{
				if (grid->cells[i, j] != this)
				{
					// Check if the cell is to the north in Cartesian plane coordinates
					if (grid->cells[i, j]->lat > lat)
					{
						// Check if the cell is closer than the north minimum
						if (grid->cells[i, j]->lat - lat < minCardinals[north])
							minCardinals[north] = grid->cells[i, j]->lat - lat;
					}
					// Check if the cell is to the south in Cartesian plane coordinates
					else if (grid->cells[i, j]->lat < lat)
					{
						// Check if the cell is closer than the south minimum
						if (lat - grid->cells[i, j]->lat < minCardinals[south])
							minCardinals[south] = lat - grid->cells[i, j]->lat;
					}
					else
					{
						minCardinals[north] = 0;
						minCardinals[south] = 0;
					}

					// Check if the cell is to the east in Cartesian plane coordinates
					if (grid->cells[i, j]->lon > lon)
					{
						// Check if the cell is closer than the east minimum
						if (grid->cells[i, j]->lon - lon < minCardinals[east])
							minCardinals[east] = grid->cells[i, j]->lon - lon;
					}
					// Check if the cell is to the west in Cartesian plane coordinates
					else if (grid->cells[i, j]->lon < lon)
					{
						// Check if the cell is closer than the west minimum
						if (lon - grid->cells[i, j]->lon < minCardinals[west])
							minCardinals[west] = lon - grid->cells[i, j]->lon;
					}
					else
					{
						minCardinals[east] = 0;
						minCardinals[west] = 0;
					}
				}
			}

			break;
		}
	}

	for (int i = 0; i < cardinals; i++)
	{
		// Check if i is pointing vertically or horizontally
		if (i == north || i == south)
			out[i] = ((double)minCardinals[i]) / grid->xSize;
		else
			out[i] = ((double)minCardinals[i]) / grid->ySize;
	}

	return out;
}

double AnimalCell::GetDirectionalInput(int layer, Direction dir)
{
	double scale = 1.0 / (model->GetRange() + 1);
	int range = model->GetRange();
	double** map = grid->layers[layer].layer;
	double sum = 0;
	int scanLon, scanLat;

	switch (dir)
	{
	case(north):
		for (scanLat = 0; scanLat <= range && lat + scanLat < grid->ySize; scanLat++)
		{
			scanLon = range - scanLat;
			while (scanLon <= range)
			{
				if (lon - scanLon >= 0)
					sum += map[lon - scanLon][lat + scanLat] * (scale * (scanLat + 1));

				sum += map[lon][lat + scanLat] * (scale * (scanLat + 1));

				if (lon + scanLon < grid->xSize)
					sum += map[lon + scanLon][lat + scanLat] * (scale * (scanLat + 1));

				scanLon++;
			}
		}
		break;
	case(east):
		for (scanLon = 0; scanLon <= range && lon + scanLon < grid->xSize; scanLon++)
		{
			scanLat = range - scanLon;
			while (scanLat <= range)
			{
				if (lat - scanLat >= 0)
					sum += map[lon + scanLon][lat - scanLat] * (scale * (scanLon + 1));

				sum += map[lon + scanLon][lat] * (scale * (scanLat + 1));

				if (lat + scanLat < grid->ySize)
					sum += map[lon + scanLon][lat + scanLat] * (scale * (scanLon + 1));

				scanLat++;
			}
		}
		break;
	case(south):
		for (scanLat = 0; scanLat <= range && lat - scanLat >= 0; scanLat++)
		{
			scanLon = range - scanLat;
			while (scanLon <= range)
			{
				if (lon - scanLon >= 0)
					sum += map[lon - scanLon][lat - scanLat] * (scale * (scanLat + 1));

				sum += map[lon][lat - scanLat] * (scale * (scanLat + 1));

				if (lon + scanLon < grid->xSize)
					sum += map[lon + scanLon][lat - scanLat] * (scale * (scanLat + 1));

				scanLon++;
			}
		}
		break;
	case(west):
		for (scanLon = 0; scanLon <= range && lon - scanLon < 0; scanLon++)
		{
			scanLat = range - scanLon;
			while (scanLat <= range)
			{
				if (lat - scanLat >= 0)
					sum += map[lon - scanLon][lat - scanLat] * (scale * (scanLon + 1));

				sum += map[lon - scanLon][lat] * (scale * (scanLat + 1));

				if (lat + scanLat < grid->ySize)
					sum += map[lon - scanLon][lat + scanLat] * (scale * (scanLon + 1));

				scanLat++;
			}
		}
		break;
	}

	return sum;
}

double* AnimalCell::Observe()
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

	return newInputs;
}

// Public functions

AnimalCell::AnimalCell(AnimalModel* m, Grid* map, double xCoord, double yCoord)
{
	model = m;
	grid = map;
	
	lon = xCoord;
	lat = yCoord;
	horVector = 0;
	verVector = 0;
	next = act_nothing;
}

void AnimalCell::Act()
{
	// First input is always the one detailing the closest cell
	// Get Closest Cells -> input

	// Later inputs are environmental inputs
	// Observe -> input

	// model->process
}
