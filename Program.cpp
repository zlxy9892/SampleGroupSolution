// 作者：张磊
// 功能：样点甄别，计算样点可信度，进行推理制图

#include <iostream>
#include "Utility.h"
#include "SampleGroup.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout<<"Start!\n";
	cout<<"\nInitalizing...\n";
	
	// 处理输入参数

	// 示例
	//argc = 13;
	//argv[1] = "E:/data/samplegroup/test/testxc/geo.asc#E:/data/samplegroup/test/testxc/slope.asc#E:/data/samplegroup/test/testxc/planc.asc";		// 输入的环境因子图层
	//argv[2] = "category?boolean#category?gower#category?gower";												// 环境因子计算类型
	//argv[3] = "ID";																							// 样点ID在输入表格中的表头名称
	//argv[4] = "shaliA";																						// 样点目标属性变量在输入表格中的表头名称
	//argv[5] = "0.95";																							// 环境相似度阈值
	//argv[6] = "10";																							// 土壤属性宽度阈值
	//argv[7] = "0.3";																							// 样点可信度阈值
	//argv[8] = "E:/data/samplegroup/test/testxc/sample2.csv";																// 输入的样点数据（包含ID，x，y，土壤属性）
	//argv[9] = "E:/data/samplegroup/test/testxc/res/result.csv";																// 输出样点可信度结果文件
	//argv[10] = "E:/data/samplegroup/test/testxc/res/map_prediction.tif";																// 输出推测结果图层（推测值）
	//argv[11] = "E:/data/samplegroup/test/testxc/res/map_uncertainty.tif";																// 输出推测结果图层（不确定性）
	//argv[12] = "E:/data/samplegroup/test/testxc/res/map_credibility.tif";																// 输出推测结果图层（可信度）
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

	// 处理输入图层的类型和相似度计算方法
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

	// 读取其他相关参数
	double threshold_envSimi, threshold_targetVDist, threshold_credibility;
	sscanf(str_threshold_envSimi, "%lf", &threshold_envSimi);
	sscanf(str_threshold_targetVDist, "%lf", &threshold_targetVDist);
	sscanf(str_threshold_credibility, "%lf", &threshold_credibility);
	string targetVName = str_targetVName;
	string idName = str_idName;
	string inputSamplesCSV = str_samplesCSV;
	string resultCSV = str_resultCSV;

	// 初始化环境因子数据
	GDALAllRegister();
	cout<<"\nReading data ...\n";
	EnvDataset *eds = new EnvDataset();
	eds->ReadinLayers(envLayerFilenames, calMethods);
	cout<<"\nEnvironment Data Initalization OK!\n";

	// 读取样点信息
	vector<EnvUnit *> samples = Utility::ReadCSV(inputSamplesCSV, eds, targetVName, idName);
	cout<<"\nExisted Samples Initalization OK!\n";

	// 开始进行样点甄别的计算
	SampleGroup *sg = new SampleGroup(eds, samples);
	sg->Threshold_EnvSimi = threshold_envSimi;
	sg->Threshold_TargetVDist = threshold_targetVDist;
	sg->Threshold_Credibility = threshold_credibility;
	sg->RefreshAll();
	cout<<"\nCalculate Samples Credibility OK!\n";

	// 输出样点可信度文件
	Utility::WriteCSV_SampleCredibility(resultCSV, samples);
	cout<<"\nWrite out Sample Credibility CSV File OK!\n";

	// 开始进行推测制图
	cout<<"\nPredict Maps ...\n\n";
	sg->PredictMapBySamples();

	// 输出三个推测结果图层（包括推测值、推测不确定性、推测可信度）
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