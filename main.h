#include "common.h"

class TrainingDataSet
{
public:
	std::vector<std::vector<float>> getRTTData(int w, std::vector<std::vector<float>> dataSet);
	std::vector<labels> getRTTTarget(int w, std::vector<std::vector<float>> dataSet);

};

class LogFileReader
{
public:
    //constructor
    LogFileReader(const std::string& fileName):
            fileName(fileName)
    {}

    //Function to fetch data from the log files
    std::vector<float> getTrainingSetData();

private:
    std::string fileName;
};

