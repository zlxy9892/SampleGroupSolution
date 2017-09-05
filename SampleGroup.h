// ������𷽷�
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
	double CalcSimi(EnvUnit *e1, EnvUnit *e2);											// ��������������Ԫ֮����ۺ����ƶ�
	double CalcTargetVDist(EnvUnit *e1, EnvUnit *e2);									// ��������������ԪĿ����������֮��ľ���

	void RefreshAll();
	void RefreshSimiSamples();															// ����ÿ��������֮���Ƶ�����
	void RefreshSampleCredibility();													// ����ÿ������Ŀ��Ŷ�

	void PredictMapBySamples();															// ����������ŶȽ����Ʋ���ͼ�������Ʋ�ֵ���Ʋⲻȷ���ԡ��Ʋ���Ŷȣ�
};

#endif