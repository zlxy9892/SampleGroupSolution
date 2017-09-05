// 环境数据集
#ifndef _ENVDATASET_H_
#define _ENVDATASET_H_

#include <vector>
#include "EnvLayer.h"
#include "EnvUnit.h"
#include "BasicSetting.h"
#include "Location.h"
#include <iostream>

using namespace std;

class EnvDataset
{
public:
	vector<EnvLayer *> Layers;
	//vector<EnvUnit *> EnvUnits;
	BasicSetting *Setting;

public:
	EnvDataset(void);
	~EnvDataset(void);

	void AddLayer(EnvLayer *layer);
	void ReadinLayers(vector<string> envLayerFilenames, vector<string> calMethods);
	//void RefreshEnvUnits();
	void RefreshSetting();
	void RefreshAll();
	//EnvUnit* GetEnvUnit(int row, int col);
	//EnvUnit* GetEnvUnit(double x, double y);
	EnvUnit* GetEnvUnitFromLayers(int row, int col);
	EnvUnit* GetEnvUnitFromLayers(double x, double y);

};

#endif