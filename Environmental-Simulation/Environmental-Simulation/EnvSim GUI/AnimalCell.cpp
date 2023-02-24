
#include "AnimalCell.h"

// Animal Cell functions

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
