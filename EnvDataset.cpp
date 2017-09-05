#include "EnvDataset.h"


EnvDataset::EnvDataset(void)
{
	this->Setting = new BasicSetting();
}

EnvDataset::~EnvDataset(void)
{
	delete this->Setting;
}

void EnvDataset::ReadinLayers(vector<string> envLayerFilenames, vector<string> calMethods)
{
	for(int i = 0; i < envLayerFilenames.size(); i++)
	{
		if(calMethods[i] == "boolean")
		{
			this->AddLayer(new EnvLayer(i, "Bedrock", envLayerFilenames[i], FACTOR));
		}
		else if(calMethods[i] == "gower")
		{
			this->AddLayer(new EnvLayer(i, "OtherLayer", envLayerFilenames[i], SINGLEVALUE));
		}
		else
		{
			this->AddLayer(new EnvLayer(i, "OtherLayer", envLayerFilenames[i], SINGLEVALUE));
		}
	}
	this->RefreshAll();
}

void EnvDataset::AddLayer(EnvLayer *layer)
{
	this->Layers.push_back(layer);
}

void EnvDataset::RefreshAll()
{
	this->RefreshSetting();
	//this->RefreshEnvUnits();
}

//void EnvDataset::RefreshEnvUnits()
//{
//	int envUnitCount = this->Setting->Width*this->Setting->Height;
//	this->EnvUnits.clear();
//	this->EnvUnits.reserve(envUnitCount);
//	if(this->Layers.size() <= 0)
//	{
//		return;
//	}
//	for(int i = 0; i < envUnitCount; i++)
//	{
//		EnvUnit *e = new EnvUnit();
//		int row = i / this->Setting->Width;
//		int col = i % this->Setting->Width;
//		double x = this->Setting->Xmin + this->Setting->CellSize * col;
//		double y = this->Setting->Ymax - this->Setting->CellSize * row;
//		Location *loc = new Location(row, col, x, y);
//		e->Loc = loc;
//		e->CellSize = this->Setting->CellSize;
//		e->IsCal = true;
//		e->EnvValues.reserve(this->Layers.size());
//		for(int j = 0; j < this->Layers.size(); j++)
//		{
//			EnvLayer *layer = this->Layers.at(j);
//			double envValue = layer->EnvData[i];
//			e->EnvValues.push_back(envValue);
//			e->DataTypes.push_back(layer->DataType);
//			if(envValue == layer->GdalEnvData->GetRasterBand(1)->GetNoDataValue())
//			{
//				e->IsCal = false;
//			}
//		}
//		this->EnvUnits.push_back(e);
//	}
//}

void EnvDataset::RefreshSetting()
{
	if (this->Layers.size() <= 0)
	{
		return;
	}
	GDALDataset *firstLayerData = this->Layers.at(0)->GdalEnvData;
	double geoTransform[6];
	firstLayerData->GetGeoTransform(geoTransform);
	this->Setting->CellSize = geoTransform[1];
	this->Setting->NoDataValue = firstLayerData->GetRasterBand(1)->GetNoDataValue();
	this->Setting->Xmin = geoTransform[0];
	this->Setting->Ymin = geoTransform[3] - this->Setting->CellSize * firstLayerData->GetRasterYSize();
	this->Setting->Xmax = this->Setting->Xmin + this->Setting->CellSize * firstLayerData->GetRasterXSize();
	this->Setting->Ymax = geoTransform[3];
	this->Setting->Width = firstLayerData->GetRasterXSize();
	this->Setting->Height = firstLayerData->GetRasterYSize();
	// ��������ͼ����������Ƿ����һͼ��һ��...
}

//EnvUnit* EnvDataset::GetEnvUnit(int row, int col)
//{
//	if (row < 0 || row >= this->Setting->Height || col < 0 || col >= this->Setting->Width)
//	{
//		return NULL;
//	}
//	else
//	{
//		return this->EnvUnits.at(row * this->Setting->Width + col);
//	}
//}
//
//EnvUnit* EnvDataset::GetEnvUnit(double x, double y)
//{
//	int irow, icol;
//	if (x < this->Setting->Xmin || x > this->Setting->Xmax || y < this->Setting->Ymin || y > this->Setting->Ymax)
//	{
//		return NULL;
//	}
//	irow = (int) ((this->Setting->Ymax - y) / this->Setting->CellSize);
//	icol = (int) ((x - this->Setting->Xmin) / this->Setting->CellSize);
//	return this->EnvUnits.at(irow * this->Setting->Width + icol);
//}



EnvUnit* EnvDataset::GetEnvUnitFromLayers( int row, int col )
{
	if (row < 0 || row >= this->Setting->Height || col < 0 || col >= this->Setting->Width)
	{
		return NULL;
	}
	
	EnvUnit *e = new EnvUnit();
	double x = this->Setting->Xmin + this->Setting->CellSize * col;
	double y = this->Setting->Ymax - this->Setting->CellSize * row;
	Location *loc = new Location(row, col, x, y);
	e->Loc = loc;
	e->CellSize = this->Setting->CellSize;
	e->IsCal = true;
	e->EnvValues.reserve(this->Layers.size());
	for(int i = 0; i < this->Layers.size(); i++)
	{
		EnvLayer *layer = this->Layers.at(i);
		double envValue = layer->EnvData[row * this->Setting->Width + col];
		e->EnvValues.push_back(envValue);
		e->DataTypes.push_back(layer->DataType);
		if(envValue == layer->GdalEnvData->GetRasterBand(1)->GetNoDataValue())
		{
			e->IsCal = false;
		}
	}

	return e;
}

EnvUnit* EnvDataset::GetEnvUnitFromLayers( double x, double y )
{
	int irow, icol;
	if (x < this->Setting->Xmin || x > this->Setting->Xmax || y < this->Setting->Ymin || y > this->Setting->Ymax)
	{
		return NULL;
	}
	irow = (int) ((this->Setting->Ymax - y) / this->Setting->CellSize);
	icol = (int) ((x - this->Setting->Xmin) / this->Setting->CellSize);

	EnvUnit *e = new EnvUnit();
	Location *loc = new Location(irow, icol, x, y);
	e->Loc = loc;
	e->CellSize = this->Setting->CellSize;
	e->IsCal = true;
	e->EnvValues.reserve(this->Layers.size());
	for(int i = 0; i < this->Layers.size(); i++)
	{
		EnvLayer *layer = this->Layers.at(i);
		double envValue = layer->EnvData[irow * this->Setting->Width + icol];
		e->EnvValues.push_back(envValue);
		e->DataTypes.push_back(layer->DataType);
		if(envValue == layer->GdalEnvData->GetRasterBand(1)->GetNoDataValue())
		{
			e->IsCal = false;
		}
	}

	return e;
}

