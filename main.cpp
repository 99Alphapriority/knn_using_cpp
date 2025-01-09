#include "main.h"

float DTWDistance(std::vector<float> train, std::vector<float> test, int w)
{
	std::map<std::pair<int, int>, float> DTW;

	w = std::max(w, std::abs(static_cast<int>(train.size()) - static_cast<int>(test.size())));

	for(int i = -1; i < train.size(); i++)
	{
		for(int j = -1; j < test.size(); j++)
		{
			DTW[{i,j}] = std::numeric_limits<float>::infinity();
		}
	}

	DTW[{-1,-1}] = 0.0;

	for(int i = 0; i < train.size(); i++)
	{
		for(int j = std::max(0, i-w); j < std::min(static_cast<int>(test.size()), i + w); j++)
		{
			float dist = std::pow(train[i]-test[j],2);
			DTW[{i,j}] = dist + std::min(DTW[{i-1, j}], std::min(DTW[{i, j-1}], DTW[{i-1, j-1}]));
		}
	}

	return std::sqrt(DTW[{train.size()-1, test.size()-1}]);
}

labels knn(TrainingDataSet trainingDataSet, std::vector<float> test, int w, int n_neighbours=1)
{
	std::vector<float> tmpDistance;
	std::vector<std::vector<float>> tmpVectors;
	std::vector<labels> tmpLabels;

	for(std::vector<float> j: trainingDataSet.trainingSet)
	{
		labels label = static_cast<labels>(j.back());
		j.pop_back();

		float distance = DTWDistance(j, test, w);
		
		auto idx = std::lower_bound(tmpDistance.begin(), tmpDistance.end(), distance) - tmpDistance.begin();

		j.push_back(static_cast<float>(label));
		tmpDistance.insert(tmpDistance.begin() + idx, distance);
		tmpVectors.insert(tmpVectors.begin() + idx, j);
		tmpLabels.insert(tmpLabels.begin() + idx, label);
	}

	return tmpLabels[0];
}

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

	trainingDataSet.trainingSet = x_all;

	for(int i = 0; i < y_all.size(); i++)
		trainingDataSet.trainingSet[i].push_back(y_all[i]);

#if 0
	//To print the x_all vector
	for(int i = 0; i < trainingDataSet.trainingSet.size(); i++)
	{
		if(0 == i)
			std::cout << "[";
		for(int j = 0; j < trainingDataSet.trainingSet[0].size(); j++)
		{
			if(j % 11 == 0)
				std::cout << "[";
			std::cout << trainingDataSet.trainingSet[i][j];

			if((j+1) % 11 ==0)
				std::cout << "]" << std::endl;
			else
				std::cout << "  ";
		}
		if(i == trainingDataSet.trainingSet.size() - 1)
			std::cout << "]" << std::endl;
	}
#endif

	labels predict = knn(trainingDataSet, rttInputData, rttInputData.size());
	std::cout << "predict: " << predict << std::endl;

	return 0;
}
