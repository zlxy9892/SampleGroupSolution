#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "EnvUnit.h"
#include "EnvDataset.h"

using namespace std;

//typedef float REAL;

class Utility
{
public:
	Utility(void);
	~Utility(void);

	static string ConvertToString(double value);
	static void ParseStr(string str, char c, vector<string>& tokens);	// 解析以c为分隔符的数据
	static vector<EnvUnit *> ReadCSV(string filename, EnvDataset *envDataset);
	static vector<EnvUnit *> ReadCSV(string filename, EnvDataset *envDataset, string targetV, string idName);	// 包含读取目标土壤属性信息和ID号
	static void WriteCSV(string filename, vector<EnvUnit *> envUnits);
	static void WriteCSV2(string filename, vector<EnvUnit *> envUnits, vector<int> predictedAreaList);

	static void WriteCSV_SampleCredibility(string filename, vector<EnvUnit *> samples);		// 输出样点可信度文件
};

#endif