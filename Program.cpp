// ���ߣ�����
// ���ܣ�������𣬼���������Ŷȣ�����������ͼ

#include <iostream>
#include "Utility.h"
#include "SampleGroup.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout<<"Start!\n";
	cout<<"\nInitalizing...\n";
	
	// �����������

	// ʾ��
	//argc = 13;
	//argv[1] = "E:/data/samplegroup/test/testxc/geo.asc#E:/data/samplegroup/test/testxc/slope.asc#E:/data/samplegroup/test/testxc/planc.asc";		// ����Ļ�������ͼ��
	//argv[2] = "category?boolean#category?gower#category?gower";												// �������Ӽ�������
	//argv[3] = "ID";																							// ����ID���������еı�ͷ����
	//argv[4] = "shaliA";																						// ����Ŀ�����Ա������������еı�ͷ����
	//argv[5] = "0.95";																							// �������ƶ���ֵ
	//argv[6] = "10";																							// �������Կ����ֵ
	//argv[7] = "0.3";																							// ������Ŷ���ֵ
	//argv[8] = "E:/data/samplegroup/test/testxc/sample2.csv";																// ������������ݣ�����ID��x��y���������ԣ�
	//argv[9] = "E:/data/samplegroup/test/testxc/res/result.csv";																// ���������ŶȽ���ļ�
	//argv[10] = "E:/data/samplegroup/test/testxc/res/map_prediction.tif";																// ����Ʋ���ͼ�㣨�Ʋ�ֵ��
	//argv[11] = "E:/data/samplegroup/test/testxc/res/map_uncertainty.tif";																// ����Ʋ���ͼ�㣨��ȷ���ԣ�
	//argv[12] = "E:/data/samplegroup/test/testxc/res/map_credibility.tif";																// ����Ʋ���ͼ�㣨���Ŷȣ�
	//string params = "E:/data/samplegroup/geo.asc#E:/data/samplegroup/slope.asc#E:/data/samplegroup/planc.asc category?boolean#category?gower#category?gower ID shali 0.95 10 0.3 E:/data/samplegroup/samples.csv E:/data/samplegroup/result.csv E:/data/samplegroup/map_prediction.tif E:/data/samplegroup/map_uncertainty.tif E:/data/samplegroup/map_credibility.tif";
	

	if(argc != 12+1)
	{
		cout << "Invalid inputs!\n" << endl;
		cout << "Example:\nSampleGroup\n<1. path to input layers>\n<2. type of calculation>\n<3. column name of sample ID>\n<4. column name of target variable>\n<5. threshold of environmental similarity>\n<6. threshold of target variable distance>\n<7. threshold of sample credibility>\n<8. path to input samples file>\n<9. path to output file of sample credibility>\n<10. path to output file prediction map>\n<11. path to output file uncertainty map>\n<12. path to output file credibility map>" << endl;
		return 999;
	}
	char *str_inputEnvLayerFilenames = argv[1];
	char *str_cate_calMethod = argv[2];
	char *str_idName = argv[3];
	char *str_targetVName = argv[4];
	char *str_threshold_envSimi = argv[5];
	char *str_threshold_targetVDist = argv[6];
	char *str_threshold_credibility = argv[7];
	char *str_samplesCSV = argv[8];
	char *str_resultCSV = argv[9];
	char *str_outputMap_prediction = argv[10];
	char *str_outputMap_uncertainty = argv[11];
	char *str_outputMap_credibility = argv[12];

	vector<string> envLayerFilenames;
	Utility::ParseStr(str_inputEnvLayerFilenames, '#', envLayerFilenames);

	// ��������ͼ������ͺ����ƶȼ��㷽��
	vector<string> cate_calMethodList;
	Utility::ParseStr(str_cate_calMethod, '#', cate_calMethodList);
	vector<string> categories, calMethods;
	for(int i = 0; i < cate_calMethodList.size(); i++)
	{
		vector<string> values;
		Utility::ParseStr(cate_calMethodList[i], '?', values);
		categories.push_back(values[0]);
		calMethods.push_back(values[1]);
	}

	// ��ȡ������ز���
	double threshold_envSimi, threshold_targetVDist, threshold_credibility;
	sscanf(str_threshold_envSimi, "%lf", &threshold_envSimi);
	sscanf(str_threshold_targetVDist, "%lf", &threshold_targetVDist);
	sscanf(str_threshold_credibility, "%lf", &threshold_credibility);
	string targetVName = str_targetVName;
	string idName = str_idName;
	string inputSamplesCSV = str_samplesCSV;
	string resultCSV = str_resultCSV;

	// ��ʼ��������������
	GDALAllRegister();
	cout<<"\nReading data ...\n";
	EnvDataset *eds = new EnvDataset();
	eds->ReadinLayers(envLayerFilenames, calMethods);
	cout<<"\nEnvironment Data Initalization OK!\n";

	// ��ȡ������Ϣ
	vector<EnvUnit *> samples = Utility::ReadCSV(inputSamplesCSV, eds, targetVName, idName);
	cout<<"\nExisted Samples Initalization OK!\n";

	// ��ʼ�����������ļ���
	SampleGroup *sg = new SampleGroup(eds, samples);
	sg->Threshold_EnvSimi = threshold_envSimi;
	sg->Threshold_TargetVDist = threshold_targetVDist;
	sg->Threshold_Credibility = threshold_credibility;
	sg->RefreshAll();
	cout<<"\nCalculate Samples Credibility OK!\n";

	// ���������Ŷ��ļ�
	Utility::WriteCSV_SampleCredibility(resultCSV, samples);
	cout<<"\nWrite out Sample Credibility CSV File OK!\n";

	// ��ʼ�����Ʋ���ͼ
	cout<<"\nPredict Maps ...\n\n";
	sg->PredictMapBySamples();

	// ��������Ʋ���ͼ�㣨�����Ʋ�ֵ���Ʋⲻȷ���ԡ��Ʋ���Ŷȣ�
	string fn_outputMap_prediction = str_outputMap_prediction;
	string fn_outputMap_uncertainty = str_outputMap_uncertainty;
	string fn_outputMap_credibility = str_outputMap_credibility;
	sg->Map_Prediction->Writeout(fn_outputMap_prediction, "GTiff", eds->Layers[0]->GdalEnvData);
	sg->Map_Uncertainty->Writeout(fn_outputMap_uncertainty, "GTiff", eds->Layers[0]->GdalEnvData);
	sg->Map_Credibility->Writeout(fn_outputMap_credibility, "GTiff", eds->Layers[0]->GdalEnvData);
	
	cout<<"\n\n --- DONE! ---\n";

	//system("pause");
	return 0;
}