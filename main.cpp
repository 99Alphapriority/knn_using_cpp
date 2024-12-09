#include "main.h"

std::vector<float> logFileReader::getRTTData()
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

int main()
{
	//file path
	std::string b0_c0_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_1hr/0b-0c.log"; 
	std::string b0_c1_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_1hr/0b-1c.log"; 
	std::string b1_c0_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_1hr/1b-0c.log"; 
	std::string b1_c1_logs = "../../MINT/year_2/project/Experiments/dumbbell_topology/ml_rtt_probing/logs/iperf_1hr/1b-1c.log"; 

	logFileReader b0_c0(b0_c0_logs);
	logFileReader b0_c1(b0_c1_logs);
	logFileReader b1_c0(b1_c0_logs);
	logFileReader b1_c1(b1_c1_logs);

	std::vector<std::vector<float>> dataSet;

	dataSet.push_back(b0_c0.getRTTData());
	dataSet.push_back(b0_c1.getRTTData());
	dataSet.push_back(b1_c0.getRTTData());
	dataSet.push_back(b1_c1.getRTTData());

	if(dataSet[B0_C0].empty() || dataSet[B0_C1].empty() || dataSet[B1_C0].empty() || dataSet[B1_C1].empty())
		return 1;

#ifdef DEBUG
	std::cout << "RTT data from " << b0_c0_logs << std::endl;
	for(float rttValue: dataSet[B0_C0])
		std::cout<< rttValue << std::endl;
#endif

	return 0;
}
