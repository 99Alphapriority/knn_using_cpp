#include "common.h"

class logFileReader
{
public:
    //constructor
    logFileReader(const std::string& fileName):
            fileName(fileName)
    {}

    //Function to fetch data from the log files
    std::vector<float> getRTTData();

private:
    std::string fileName;
};

