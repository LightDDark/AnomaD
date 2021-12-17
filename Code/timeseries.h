

#ifndef TIMESERIES_H_
#define TIMESERIES_H_
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <list>
#include "anomaly_detection_util.h"

using namespace std;

class TimeSeries{
    map<string,vector<float>> dataTable;
    int lineCount = 0;
    int columnCount = 0;
    string timeCol;
public:
    explicit TimeSeries(const char* CSVfileName);
    int countColumns() const;
    unsigned long countRows()const;
    string getColumnName(vector<float>)const;
    float getTimeValue(string featureName, float time)const;
    std::list<string> getFeatures()const;
    const vector<float>* getValues(string columnName) const;
    std::vector<const vector<float>*> getColumns() const;
    string getTimeName() const;
    vector<string> extractFeatures(fstream csvStream) const;
    string removeEOL(string last) const;
};



#endif /* TIMESERIES_H_ */
