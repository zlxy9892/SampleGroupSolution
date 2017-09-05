// ������Ԫ����������������ӵ���Ϣ

#ifndef _ENVUNIT_H_
#define _ENVUNIT_H_

#include <vector>
#include "DataTypeEnum.h"
#include "Location.h"

using namespace std;

class EnvUnit
{
public:
	bool IsCal;							// �Ƿ�������
	vector<double> EnvValues;			// �õ������л�������ֵ
	vector<DataTypeEnum> DataTypes;		// �õ������л�������ֵ������
	double SoilType;					// ��������ֵ
	double SoilVarible;					// ��������ֵ
	string SampleID;					// �������㣬��¼�����ID��
	Location *Loc;						// �õ��λ����Ϣ
	//double Uncertainty;				// �õ�Ĳ�ȷ����ֵ
	double MaxSimi;						// �õ������㼯�������ƶ�ֵ
	double Uncertainty_temp;
	double CellSize;						// �õ�ı߳�����ߣ�
	bool isCanPredict;					// �Ƿ�����Ʋ�

	double PredictSoilVarible;			// �Ʋ����������ֵ
	double PredictUncertainty;			// �Ʋⲻȷ����
	double PredictCredibility;			// �Ʋ���Ŷ�

	vector<EnvUnit *> SimiEnvUnits;		// ��õ����Ƶ�������
	double Credibility;					// ����Ŀ��Ŷ�
	int Number_Support;					// ֧����������
	int Number_Contradict;				// ì����������

public:
	EnvUnit(void);
	~EnvUnit(void);

	void AddEnvValue(double envValue);
};

#endif