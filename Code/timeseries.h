

#ifndef TIMESERIES_H_
#define TIMESERIES_H_
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <list>
using namespace std;

class TimeSeries{
    map<string,vector<float>> dataTable;
    int lineCount = 0;
    int columnCount = 0;
    string timeCol;
public:
    explicit TimeSeries(const char* CSVfileName);
    int countColumns() const;
    unsigned long countRows(string colName);
    string getColumnName(vector<float>);
    float getTimeValue(string featureName, float time);
    std::list<string> getFeatures();
    };



#endif /* TIMESERIES_H_ */
