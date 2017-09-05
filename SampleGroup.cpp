#include "SampleGroup.h"


SampleGroup::SampleGroup(void)
{
}

SampleGroup::SampleGroup(EnvDataset *envDataset, vector<EnvUnit *> sampleEnvUnits)
{
	this->EDS = envDataset;
	this->SampleEnvUnits = sampleEnvUnits;
	this->Threshold_EnvSimi = 0.95;
	this->Threshold_TargetVDist = 10;
	this->Threshold_Credibility = 0.3;
	this->Map_Prediction = new EnvLayer();
	this->Map_Uncertainty = new EnvLayer();
	this->Map_Credibility = new EnvLayer();
}

SampleGroup::~SampleGroup(void)
{
	if (this->EDS != NULL) { delete this->EDS; }
}

double SampleGroup::CalcSimi_Single(double e1, double e2, double range, DataTypeEnum dataType)
{
	double simi = -1;
	if (dataType == FACTOR)
	{
		if (e1 == e2)
		{
			simi = 1;
		}
		else
		{
			simi = 0;
		}
	}
	else if (dataType == SINGLEVALUE)
	{
		simi = 1 - abs(e1 - e2) / range;
	}
	return simi;
}

double SampleGroup::CalcSimi(EnvUnit *e1, EnvUnit *e2)
{
	double simi = -1;
	if (!e1->IsCal || !e2->IsCal)
	{
		return -1;	// 不参与计算的点
	}
	if (e1->EnvValues.size() == e2->EnvValues.size())
	{
		simi = 1;
		for (int i = 0; i < e1->EnvValues.size(); i++)
		{
			double range = this->EDS->Layers.at(i)->Data_Range;
			DataTypeEnum dataType = e1->DataTypes.at(i);
			double simi_temp = this->CalcSimi_Single(e1->EnvValues.at(i), e2->EnvValues.at(i), range, dataType);
			if (simi_temp < simi)
			{
				simi = simi_temp;
			}
		}
		return simi;
	}
	else
	{
		return -1;
	}
}

double SampleGroup::CalcTargetVDist(EnvUnit *e1, EnvUnit *e2)
{
	double dist = 0;
	dist = abs(e1->SoilVarible - e2->SoilVarible);
	return dist;
}

void SampleGroup::RefreshAll()
{
	this->RefreshSimiSamples();
	this->RefreshSampleCredibility();
}

void SampleGroup::RefreshSimiSamples()
{
	double threshold_envsimi = this->Threshold_EnvSimi;
	for (int i = 0; i < this->SampleEnvUnits.size(); i++)
	{
		EnvUnit *e1 = this->SampleEnvUnits[i];
		for (int j = 0; j < this->SampleEnvUnits.size(); j++)
		{
			EnvUnit *e2 = this->SampleEnvUnits[j];
			if (e1 != e2)
			{
				double simi_env = this->CalcSimi(e1, e2);
				if (simi_env > threshold_envsimi)
				{
					e1->SimiEnvUnits.push_back(e2);
				}
			}
		}
	}
}

void SampleGroup::RefreshSampleCredibility()
{
	// 更新支持和矛盾样点的数量
	double threshold_tardist = this->Threshold_TargetVDist;
	for (int i = 0; i < this->SampleEnvUnits.size(); i++)
	{
		EnvUnit *e1 = this->SampleEnvUnits[i];
		e1->Number_Support = 0;
		e1->Number_Contradict = 0;
		vector<EnvUnit *> simiSamples = e1->SimiEnvUnits;
		for (int j = 0; j < simiSamples.size(); j++)
		{
			EnvUnit *e2 = simiSamples[j];
			double dist = this->CalcTargetVDist(e1, e2);
			if (dist < threshold_tardist)
			{
				e1->Number_Support++;
			}
			else
			{
				e1->Number_Contradict++;
			}
		}
	}

	// 更新可信度值
	for (int i = 0; i < this->SampleEnvUnits.size(); i++)
	{
		EnvUnit *e1 = this->SampleEnvUnits[i];
		if (e1->SimiEnvUnits.size() <= 0)		// 可信度未知，此处设定为-1
		{
			e1->Credibility = -1;
		}
		else if (e1->Number_Support <= 0)		// 全部都是矛盾样点，设为0
		{
			e1->Credibility = 0;
		}
		else if (e1->Number_Contradict <= 0)	// 全部都是支持样点，设为1
		{
			double minValue = LLONG_MAX;
			vector<EnvUnit *> simiSamples = e1->SimiEnvUnits;
			for (int j = 0; j < simiSamples.size(); j++)
			{
				EnvUnit *e2 = simiSamples[j];
				double dist = this->CalcTargetVDist(e1, e2);
				double tempValue = 1 - dist / threshold_tardist;
				if (minValue > tempValue)
				{
					minValue = tempValue;
				}
			}
			minValue = MIN(minValue, 1.0);
			e1->Credibility = minValue;
		}
		else								// 既有支持样点也有矛盾样点
		{
			double sum_supportDist = 0;		// 支持样点的总距离
			double sum_dist = 0;			// 所有样点的总距离
			vector<EnvUnit *> simiSamples = e1->SimiEnvUnits;
			for (int j = 0; j < simiSamples.size(); j++)
			{
				EnvUnit *e2 = simiSamples[j];
				double dist = this->CalcTargetVDist(e1, e2);
				sum_dist += dist;
				if (dist < threshold_tardist)
				{
					sum_supportDist += dist;
				}
			}
			e1->Credibility = (1.0 - sum_supportDist / sum_dist) * (1.0 * e1->Number_Support / e1->SimiEnvUnits.size());
		}
	}
}

void SampleGroup::PredictMapBySamples()
{
	int xSize = this->EDS->Setting->Width;
	int ySize = this->EDS->Setting->Height;
	int count = xSize * ySize;
	this->Map_Prediction->XSize = xSize;
	this->Map_Prediction->YSize = ySize;
	this->Map_Uncertainty->XSize = xSize;
	this->Map_Uncertainty->YSize = ySize;
	this->Map_Credibility->XSize = xSize;
	this->Map_Credibility->YSize = ySize;
	vector<double> data_prediction;
	vector<double> data_uncertainty;
	vector<double> data_credibility;
	data_prediction.clear();
	data_uncertainty.clear();
	data_credibility.clear();
	data_prediction.reserve(count);
	data_uncertainty.reserve(count);
	data_credibility.reserve(count);

	//int count = this->EDS->Setting->Width * this->EDS->Setting->Height;
	int rownum = this->EDS->Setting->Height;
	int colnum = this->EDS->Setting->Width;
	int segementCount = 1000;
	int ii = 0;

	for (int row = 0; row < rownum; row++)
	{
		for (int col = 0; col < colnum; col++)
		{
			ii = row * colnum + col;
			if(ii % (count/segementCount) == 0)
			{
				cout<<'\r';
				cout<<"Completed "<<setw(5)<<(int((ii*1.0/count+0.5/segementCount)*segementCount))/(segementCount/100.0)<<"%";
			}

			EnvUnit *e = this->EDS->GetEnvUnitFromLayers(row, col);
		
			if (!e->IsCal)
			{
				data_prediction.push_back(this->EDS->Setting->NoDataValue);
				data_uncertainty.push_back(this->EDS->Setting->NoDataValue);
				data_credibility.push_back(this->EDS->Setting->NoDataValue);
				delete e;
				continue;
			}

			double sum1 = 0;	// 求和（土壤属性值 * 环境相似度值）
			double sum2 = 0;	// 求和（环境相似度值）
			double sum3 = 0;	// 求和（可信度）
			int count = 0;		// 计数，满足推测条件的样点数量

			for (int j = 0; j < this->SampleEnvUnits.size(); j++)
			{
				EnvUnit *se = this->SampleEnvUnits[j];
				if (se->Credibility > this->Threshold_Credibility)	// 挑出可信度高的样点
				{
					double envSimi = this->CalcSimi(e, se);
					if (envSimi > this->Threshold_EnvSimi)			// 挑出与待推测点环境相似度高的样点
					{
						sum1 += envSimi * se->SoilVarible;
						sum2 += envSimi;
						sum3 += se->Credibility;
						count++;
					}
				}
				else
				{
					continue;
				}
			}

			delete e;

			if (count > 0)
			{
				data_prediction.push_back( (float)(1.0 * sum1 / sum2) );
				data_uncertainty.push_back( (float)(1 - 1.0 * sum2 / count) );
				data_credibility.push_back( (float)(1.0 * sum3 / count) );
				/*e->PredictSoilVarible = 1.0 * sum1 / sum2;
				e->PredictUncertainty = 1 - 1.0 * sum2 / count;
				e->PredictCredibility = 1.0 * sum3 / count;*/
			}
			else	// 没有可推测点，设为-1
			{
				data_prediction.push_back( (float)(-1.0) );
				data_uncertainty.push_back( (float)(-1.0) );
				data_credibility.push_back( (float)(-1.0) );
				/*e->PredictSoilVarible = -1.0;
				e->PredictUncertainty = -1.0;
				e->PredictCredibility = -1.0;*/
			}

			/*data_prediction.push_back(e->PredictSoilVarible);
			data_uncertainty.push_back(e->PredictUncertainty);
			data_credibility.push_back(e->PredictCredibility);*/

		}
	}

	cout<<'\r';
	cout<<"Completed "<<setw(5)<<100.0<<"%";

	// 生成三个推测图层
	this->Map_Prediction->EnvData = data_prediction;
	this->Map_Uncertainty->EnvData = data_uncertainty;
	this->Map_Credibility->EnvData = data_credibility;
	this->Map_Prediction->NoDataValue = this->EDS->Setting->NoDataValue;
	this->Map_Uncertainty->NoDataValue = this->EDS->Setting->NoDataValue;
	this->Map_Credibility->NoDataValue = this->EDS->Setting->NoDataValue;
	this->Map_Prediction->CalcStat();
	this->Map_Uncertainty->CalcStat();
	this->Map_Credibility->CalcStat();
}

//void SampleGroup::PredictMapBySamples()
//{
//	vector<EnvUnit *> allEnvUnits = this->EDS->EnvUnits;
//	for (int i = 0; i < allEnvUnits.size(); i++)
//	{
//		EnvUnit *e = allEnvUnits[i];
//		if (!e->IsCal)
//		{
//			continue;
//		}
//		double sum1 = 0;	// 求和（土壤属性值 * 环境相似度值）
//		double sum2 = 0;	// 求和（环境相似度值）
//		double sum3 = 0;	// 求和（可信度）
//		int count = 0;		// 计数，满足推测条件的样点数量
//		for (int j = 0; j < this->SampleEnvUnits.size(); j++)
//		{
//			EnvUnit *se = this->SampleEnvUnits[j];
//			if (se->Credibility > this->Threshold_Credibility)	// 挑出可信度高的样点
//			{
//				double envSimi = this->CalcSimi(e, se);
//				if (envSimi > this->Threshold_EnvSimi)			// 挑出与待推测点环境相似度高的样点
//				{
//					sum1 += envSimi * se->SoilVarible;
//					sum2 += envSimi;
//					sum3 += se->Credibility;
//					count++;
//				}
//			}
//			else
//			{
//				continue;
//			}
//		}
//		if (count > 0)
//		{
//			e->PredictSoilVarible = 1.0 * sum1 / sum2;
//			e->PredictUncertainty = 1 - 1.0 * sum2 / count;
//			e->PredictCredibility = 1.0 * sum3 / count;
//		}
//		else	// 没有可推测点，设为-1
//		{
//			e->PredictSoilVarible = -1.0;
//			e->PredictUncertainty = -1.0;
//			e->PredictCredibility = -1.0;
//		}
//	}
//
//	// 生成三个推测图层
//	int xSize = this->EDS->Setting->Width;
//	int ySize = this->EDS->Setting->Height;
//	this->Map_Prediction->XSize = xSize;
//	this->Map_Prediction->YSize = ySize;
//	this->Map_Uncertainty->XSize = xSize;
//	this->Map_Uncertainty->YSize = ySize;
//	this->Map_Credibility->XSize = xSize;
//	this->Map_Credibility->YSize = ySize;
//	int count = this->EDS->EnvUnits.size();
//	vector<double> data_prediction;
//	vector<double> data_uncertainty;
//	vector<double> data_credibility;
//	for (int i = 0; i < count; i++)
//	{
//		EnvUnit *e = this->EDS->EnvUnits[i];
//		if (!e->IsCal)
//		{
//			data_prediction.push_back(this->EDS->Setting->NoDataValue);
//			data_uncertainty.push_back(this->EDS->Setting->NoDataValue);
//			data_credibility.push_back(this->EDS->Setting->NoDataValue);
//			continue;
//		}
//		data_prediction.push_back(e->PredictSoilVarible);
//		data_uncertainty.push_back(e->PredictUncertainty);
//		data_credibility.push_back(e->PredictCredibility);
//	}
//	this->Map_Prediction->EnvData = data_prediction;
//	this->Map_Uncertainty->EnvData = data_uncertainty;
//	this->Map_Credibility->EnvData = data_credibility;
//	this->Map_Prediction->NoDataValue = this->EDS->Setting->NoDataValue;
//	this->Map_Uncertainty->NoDataValue = this->EDS->Setting->NoDataValue;
//	this->Map_Credibility->NoDataValue = this->EDS->Setting->NoDataValue;
//	this->Map_Prediction->CalcStat();
//	this->Map_Uncertainty->CalcStat();
//	this->Map_Credibility->CalcStat();
//}
