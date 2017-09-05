#include "Utility.h"

Utility::Utility(void)
{
}

Utility::~Utility(void)
{
}

string Utility::ConvertToString(double value)
{
	ostringstream os;
	os.unsetf(ios::scientific);
	os.precision(10);
	if (os << value)
		return os.str();  
	return "invalid conversion";
}

void Utility::ParseStr(string str, char c, vector<string>& tokens) {
	unsigned int posL = 0;
	unsigned int posR = 0;
	while(posR < str.length()-1) {
		posR = str.find_first_of(c, posL);
		string sub = str.substr(posL, posR-posL);
		tokens.push_back(sub);
		posL = posR + 1;
	}
}

vector<EnvUnit *> Utility::ReadCSV(string filename, EnvDataset *envDataset)
{
	vector<EnvUnit*> envUnits;
	ifstream file(filename); // declare file stream:

	// �����һ�У���ȡX,Y�������ļ��е�����λ��
	string line;
	getline(file, line);
	vector<string> names;
	int pos_X = 0;
	int pos_Y = 1;
	Utility::ParseStr(line, ',', names);
	for(int i = 0; i < names.size(); i++)
	{
		if(names[i] == "X" || names[i] == "x")
		{
			pos_X = i;
			break;
		}
	}
	for(int i = 0; i < names.size(); i++)
	{
		if(names[i] == "Y" || names[i] == "y")
		{
			pos_Y = i;
			break;
		}
	}

	// ����x��yֵ��ȡ������Ϣ
	while (getline(file, line))
	{
		vector<string> values;
		Utility::ParseStr(line, ',', values);
		const char *xstr = values[pos_X].c_str();
		const char *ystr = values[pos_Y].c_str();
		double x = atof(xstr);
		double y = atof(ystr);
		EnvUnit *e = envDataset->GetEnvUnitFromLayers(x, y);
		if(e != NULL)
		{
			envUnits.push_back(e);
		}
	}
	file.close();
	return envUnits;
}

vector<EnvUnit *> Utility::ReadCSV(string filename, EnvDataset *envDataset, string targetVName, string idName)
{
	vector<EnvUnit*> envUnits;
	ifstream file(filename); // declare file stream:

	// �����һ�У���ȡX,Y���ԡ���������ֵ���ơ�����ID�����ļ��е�����λ��
	string line;
	getline(file, line);
	vector<string> names;
	int pos_X = 0;
	int pos_Y = 1;
	int pos_targetVName = 2;
	int pos_idName = 0;
	Utility::ParseStr(line, ',', names);
	for(int i = 0; i < names.size(); i++)
	{
		if(names[i] == "X" || names[i] == "x")
		{
			pos_X = i;
			break;
		}
	}
	for(int i = 0; i < names.size(); i++)
	{
		if(names[i] == "Y" || names[i] == "y")
		{
			pos_Y = i;
			break;
		}
	}
	for(int i = 0; i < names.size(); i++)
	{
		if(names[i] == targetVName)
		{
			pos_targetVName = i;
			break;
		}
	}
	for(int i = 0; i < names.size(); i++)
	{
		if(names[i] == idName)
		{
			pos_idName = i;
			break;
		}
	}

	// ����x��yֵ��ȡ������Ϣ
	while (getline(file, line))
	{
		vector<string> values;
		Utility::ParseStr(line, ',', values);
		const char *xstr = values[pos_X].c_str();
		const char *ystr = values[pos_Y].c_str();
		const char *targetVstr = values[pos_targetVName].c_str();
		string id = values[pos_idName];
		double x = atof(xstr);
		double y = atof(ystr);
		double targetV = atof(targetVstr);
		EnvUnit *e = envDataset->GetEnvUnitFromLayers(x, y);
		if(e != NULL)
		{
			e->SoilVarible = targetV;
			e->SampleID = id;
			envUnits.push_back(e);
		}
	}
	file.close();
	return envUnits;
}

void Utility::WriteCSV(string filename, vector<EnvUnit *> envUnit)
{
	if(envUnit.size() <= 0)
	{
		return;
	}
	ofstream file(filename);
	string firstLine = "X,Y\n";
	file<<firstLine;
	
	double cellSize = envUnit[0]->CellSize;
	for(int i = 0; i < envUnit.size(); i++)
	{
		double x = envUnit[i]->Loc->X + cellSize / 2;
		double y = envUnit[i]->Loc->Y - cellSize / 2;
		string xstr = Utility::ConvertToString(x);
		string ystr = Utility::ConvertToString(y);
		string line = xstr + "," +ystr + "\n";
		file<<line;
	}
	file.flush();
	file.close();
}

void Utility::WriteCSV2(string filename, vector<EnvUnit *> envUnit, vector<int> predictedAreaList)
{
	if(envUnit.size() <= 0)
	{
		return;
	}
	ofstream file(filename);
	string firstLine = "X,Y,PredictedArea\n";
	file<<firstLine;
	
	double cellSize = envUnit[0]->CellSize;
	for(int i = 0; i < envUnit.size(); i++)
	{
		double x = envUnit[i]->Loc->X + cellSize / 2;
		double y = envUnit[i]->Loc->Y - cellSize / 2;
		int predictedArea = predictedAreaList[i];
		string xstr = Utility::ConvertToString(x);
		string ystr = Utility::ConvertToString(y);
		string predictedAreaStr = Utility::ConvertToString(predictedArea);
		string line = xstr + "," +ystr + "," + predictedAreaStr + "\n";
		file<<line;
	}
	file.flush();
	file.close();
}

void Utility::WriteCSV_SampleCredibility(string filename, vector<EnvUnit *> samples)
{
	if(samples.size() <= 0)
	{
		return;
	}
	ofstream file(filename);
	string firstLine = "ID,X,Y,Credibility,Number_Support,Number_Contradict\n";
	file<<firstLine;
	
	double cellSize = samples[0]->CellSize;
	for(int i = 0; i < samples.size(); i++)
	{
		EnvUnit *e = samples[i];
		double x = e->Loc->X + cellSize / 2;
		double y = e->Loc->Y - cellSize / 2;
		string str_id = e->SampleID;
		string str_x = Utility::ConvertToString(x);
		string str_y = Utility::ConvertToString(y);
		string str_credibility = Utility::ConvertToString(e->Credibility);
		string str_number_support = Utility::ConvertToString(e->Number_Support);
		string str_number_contradict = Utility::ConvertToString(e->Number_Contradict);
		string line = str_id + "," + str_x + "," + str_y + "," + str_credibility + "," + str_number_support + "," + str_number_contradict + "\n";
		file<<line;
	}
	file.flush();
	file.close();
}

