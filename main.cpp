#include "main.h"

std::vector<labels> TrainingDataSet::getRTTTarget(int w, std::vector<std::vector<float>> dataSet)
{
	int instance_per_label = dataSet[0].size() / w;
	std::vector<labels> label;

	for(int i = B0_C0; i < LAST_LABEL; i++)
	{
		for(int j = 0; j < instance_per_label; j++)
			label.push_back(static_cast<labels>(i));
	}
	return label;
}

std::vector<std::vector<float>> TrainingDataSet::getRTTData(int w, std::vector<std::vector<float>> dataSet)
{
	std::vector<std::vector<float>> data;
	int extraValues = dataSet[0].size() % w;
	int rows = dataSet[0].size() - extraValues;

	for(std::vector<float> d: dataSet)
	{
		for(int i = 0; i < rows; i=i+w)
		{
			std::vector<float> rowData;
			for(int j = i; j < i+w; j++)
			{
				rowData.push_back(d[j]);
			}
			data.push_back(rowData);
		}
	}

	return data;
}


std::vector<float> LogFileReader::getTrainingSetData()
{
    std::ifstream file(this->fileName);
    std::vector<float> rttValues;
    std::string line;

    if(!file.is_open())
    {
        std::cerr << "Error: Unable to open file: " << this->fileName << std::endl;
        return {};
    }

    while(std::getline(file, line))
    {
        std::istringstream lineStream(line);
        uint32_t timestamp;
        float rttValue;

        lineStream >> timestamp >> rttValue;

        rttValues.push_back(rttValue);
    }

    file.close();
    return rttValues;
}

std::vector<float> getRTTInputData()
{
	std::string input;
    std::getline(std::cin, input);

    std::stringstream lineStream(input);
    std::vector<float> data;
    std::string rttData;

    while(std::getline(lineStream, rttData, ','))
        data.push_back(std::stof(rttData));
	
	return data;
}

int main()
{
	//get the RTT data from the STDIN
	std::vector<float> rttInputData = getRTTInputData();

	//file path for the training set
	std::string b0_c0_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_reliable_1hr/0b-0c.log"; 
	std::string b0_c1_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_reliable_1hr/0b-1c.log"; 
	std::string b1_c0_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_reliable_1hr/1b-0c.log"; 
	std::string b1_c1_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_reliable_1hr/1b-1c.log"; 

	LogFileReader b0_c0(b0_c0_logs);
	LogFileReader b0_c1(b0_c1_logs);
	LogFileReader b1_c0(b1_c0_logs);
	LogFileReader b1_c1(b1_c1_logs);

	std::vector<std::vector<float>> dataSet;

	dataSet.push_back(b0_c0.getTrainingSetData());
	dataSet.push_back(b0_c1.getTrainingSetData());
	dataSet.push_back(b1_c0.getTrainingSetData());
	dataSet.push_back(b1_c1.getTrainingSetData());

	if(dataSet[B0_C0].empty() || dataSet[B0_C1].empty() || dataSet[B1_C0].empty() || dataSet[B1_C1].empty())
		return 1;

	//Truncate all the time-series data to the size of the smallest one
	int dataSize = std::min(dataSet[B0_C0].size(), std::min(dataSet[B0_C1].size(), std::min(dataSet[B1_C0].size(), dataSet[B1_C1].size())));

#if 0
	//Range based for loops create a copy of each vector in dataSet
	//Hence data.erase() wont remove elements from dataSet
	for(std::vector<float> data: dataSet)
		data.erase(data.begin() + dataSize, data.end());
#endif

	//refernce operator is used in range based for loop to modify the actual data instead of creating a copy and modifying it
	for(std::vector<float>& data: dataSet)
		data.erase(data.begin() + dataSize, data.end());

#if 0
	//To check if the values are being read from the file and stored in the dataSet properly
	std::cout << "RTT data from " << b0_c0_logs << std::endl;
	for(float rttValue: dataSet[B0_C0])
		std::cout<< rttValue << std::endl;
	std::cout << "Min size: " << dataSize << std::endl;
#endif

	TrainingDataSet trainingDataSet;
	std::vector<std::vector<float>> x_all = trainingDataSet.getRTTData(rttInputData.size(), dataSet); 
	std::vector<labels> y_all = trainingDataSet.getRTTTarget(rttInputData.size(), dataSet);

#if 0
	//To print the x_all vector
	for(int i = 0; i < x_all.size(); i++)
	{
		if(0 == i)
			std::cout << "[";
		for(int j = 0; j < x_all[0].size(); j++)
		{
			if(j % 10 == 0)
				std::cout << "[";
			std::cout << x_all[i][j];

			if((j+1) % 10 ==0)
				std::cout << "]" << std::endl;
			else
				std::cout << "  ";
		}
		if(i == x_all.size() - 1)
			std::cout << "]" << std::endl;
	}
#endif

	return 0;
}
