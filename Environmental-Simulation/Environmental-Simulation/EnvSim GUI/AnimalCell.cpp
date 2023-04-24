
#include <cmath>
#include <limits.h>
#include <float.h>
#include "../Environment.h"
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

	for (int j = 0; j < cellsLive; j++)
	{
		if (cells[j] != this)
		{
			// Check if the cell is to the north in Cartesian plane coordinates
			if (cells[j]->lat > lat)
			{
				// Check if the cell is closer than the north minimum
				if (cells[j]->lat - lat < minCardinals[north])
					minCardinals[north] = cells[j]->lat - lat;
			}
			// Check if the cell is to the south in Cartesian plane coordinates
			else if (cells[j]->lat < lat)
			{
				// Check if the cell is closer than the south minimum
				if (lat - cells[j]->lat < minCardinals[south])
					minCardinals[south] = lat - cells[j]->lat;
			}
			else
			{
				minCardinals[north] = 0;
				minCardinals[south] = 0;
			}

			// Check if the cell is to the east in Cartesian plane coordinates
			if (cells[j]->lon > lon)
			{
				// Check if the cell is closer than the east minimum
				if (cells[j]->lon - lon < minCardinals[east])
					minCardinals[east] = cells[j]->lon - lon;
			}
			// Check if the cell is to the west in Cartesian plane coordinates
			else if (cells[j]->lon < lon)
			{
				// Check if the cell is closer than the west minimum
				if (lon - cells[j]->lon < minCardinals[west])
					minCardinals[west] = lon - cells[j]->lon;
			}
			else
			{
				minCardinals[east] = 0;
				minCardinals[west] = 0;
			}
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
	double* newInputs = new double[model->GetInSize() - cardinals];

	for (int i = 0; i < model->GetLayers(); i++)
	{
		for (int j = 0; j < cardinals; j++)
		{
			dSum = GetDirectionalInput(i, (Direction)j);
		}
	}

	return newInputs;
}

void AnimalCell::UpdateCellCount()
{
	for (int i = 0; i < cellsLive; i++)
	{
		if (cells[i] != this)
			delete cells[i]->cells;
		cells[i]->cells = cells;
		cells[i]->cellsLive = cellsLive;
	}
}

// Public functions

AnimalCell::AnimalCell()
{
	model = nullptr;
	grid = nullptr;

	lon = -1;
	lat = -1;
	horVector = 0;
	verVector = 0;
	next = act_nothing;

	cellsLive = 0;
	cells = nullptr;
}

AnimalCell::AnimalCell(AnimalModel* m, Grid* map, int xCoord, int yCoord, AnimalCell** cellsAlive, int cellCount)
{
	model = m;
	grid = map;
	
	lon = xCoord;
	lat = yCoord;
	horVector = 0;
	verVector = 0;
	next = act_nothing;

	cellsLive = cellCount + 1;
	if (cellsAlive == nullptr)
		cells = new AnimalCell*[1];
	else
		cells = new AnimalCell*[cellsLive];
}

void AnimalCell::Act()
{
	double* inputs = new double[model->GetInSize()];
	double* temp;

	// First input set is always the one detailing the closest cell
	temp = GetClosestCells();
	for (int i = 0; i < cardinals; i++)
	{
		inputs[i] = temp[i];
	}

	// Later inputs are environmental inputs
	temp = Observe();
	for (int i = cardinals; i < model->GetInSize(); i++)
	{
		inputs[i] = temp[i - cardinals];
	}

	model->Process();
}

AnimalCell* AnimalCell::Split()
{
	AnimalCell* newCell = new AnimalCell();
	newCell->model = model;
	newCell->grid = grid;
	newCell->lon = lon;
	newCell->lat = lat;
	newCell->lon = -horVector;
	newCell->lat = -verVector;
	next = act_nothing;

	cellsLive++;
	AnimalCell** newCellArr = new AnimalCell*[cellsLive];
	for (int i = 0; i < cellsLive; i++)
	{
		newCellArr[i] = cells[i];
	}

	newCellArr[cellsLive - 1] = newCell;

	delete cells;
	cells = newCellArr;
	UpdateCellCount();

	return newCell;
}

void AnimalCell::Merge()
{
	// Quick error check for only one cell of this species alive:
	if (cellsLive == 1)
		return;

	AnimalCell* closest = new AnimalCell(model, grid, INT_MAX, INT_MAX, cells, cellsLive);
	double* out = new double[cardinals];
	double horDist, verDist, dist;
	double minDist = DBL_MAX;

	for (int j = 0; j < cellsLive; j++)
	{
		if (cells[j] != this)
		{
			if (cells[j]->lon > lon)
				horDist = cells[j]->lon - lon;
			else
				horDist = lon - cells[j]->lon;

			if (cells[j]->lat > lat)
				verDist = cells[j]->lat - lat;
			else
				verDist = lat - cells[j]->lat;

			dist = sqrt(horDist * horDist + verDist * verDist);
			if (dist < minDist)
			{
				closest = cells[j];
				minDist = dist;
			}
		}
	}

	// Closest cell found, that cell will now die off
	closest->Die();
}

void AnimalCell::Die()
{
	// Extinction event
	if (cellsLive = 1)
	{
		delete cells;
		delete this;
		return;
	}

	cellsLive--;
	AnimalCell** newCellArr = new AnimalCell*[cellsLive];
	for (int i = 0, j = 0; i < cellsLive; i++, j++)
	{
		if (cells[i] == this)
			j++;

		newCellArr[i] = cells[j];
	}

	delete cells;
	cells = newCellArr;
	UpdateCellCount();
	delete this;
}
