// 样点甄别方法
#ifndef _SAMPLEGROUP_H_
#define _SAMPLEGROUP_H_


#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include "DataTypeEnum.h"
#include "Location.h"
#include "EnvUnit.h"
#include "BasicSetting.h"
#include "Location.h"
#include "EnvDataset.h"
#include "Utility.h"

using namespace std;

class SampleGroup
{
public:
	EnvDataset *EDS;
	vector<EnvUnit *> SampleEnvUnits;

	double Threshold_EnvSimi;
	double Threshold_TargetVDist;
	double Threshold_Credibility;

	EnvLayer *Map_Prediction;
	EnvLayer *Map_Uncertainty;
	EnvLayer *Map_Credibility;

public:
	SampleGroup(void);
	~SampleGroup(void);
	SampleGroup(EnvDataset *envDataset, vector<EnvUnit *> sampleEnvUnits);

	double CalcSimi_Single(double e1, double e2, double range, DataTypeEnum dataType);
	double CalcSimi(EnvUnit *e1, EnvUnit *e2);											// 计算两个环境单元之间的综合相似度
	double CalcTargetVDist(EnvUnit *e1, EnvUnit *e2);									// 计算两个环境单元目标土壤属性之间的距离

	void RefreshAll();
	void RefreshSimiSamples();															// 计算每个样点与之相似的样点
	void RefreshSampleCredibility();													// 计算每个样点的可信度

	void PredictMapBySamples();															// 根据样点可信度进行推测制图（包括推测值、推测不确定性、推测可信度）
};

#endif