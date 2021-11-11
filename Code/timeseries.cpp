#include "timeseries.h"
/*
 * this class sets up the data segment for the detector.
 * the class designs the input data, inlcuding methods used
 * for accessing the data on different conditions.
 */

//constructor for the map
TimeSeries::TimeSeries(const char *CSVfileName) {
    string line;
    string columnName;
    float value;
    vector<string> titles;
    fstream csvFile(CSVfileName);
    //checking if file opening was unsuccessful
    if (!csvFile.is_open()) {
        cout << "cant open" << endl;
    }
    //working on the csv file
    if (csvFile.good()) {
        //get the first line
        getline(csvFile, line);
        //set up a new string stream for the title row
        stringstream headerStream(line);
        //as long as the stream has remaining characters, cut them
        // by comma and insert the string to columnName
        while (getline(headerStream, columnName, ',')) {
            //increase the column count
            this->columnCount++;
            //checking if the current one is the time column
            if (columnName.compare("time") == 0 ||  columnName.compare("seconds") == 0 || columnName.compare("Hz") == 0) {
                this->timeCol = columnName;
            }
            //insert the name to the first value of the map pair
            titles.push_back(columnName);
            //this->dataTable.insert({columnName, vector<float>{}});
        }
        //for each line of the remaining data, insert the value
        while (getline(csvFile, line)) {
            this->lineCount++;
            stringstream lineStream(line);
            //for every pair item in the datatable, insert the float value to the vector
            for (int i = 0; i < this->columnCount; i++) {
                dataTable.insert({titles[i],vector<float>{}});
                lineStream >> value;
                dataTable.at(titles[i]).push_back(value);
                //skip commas
                if (lineStream.peek() == ',') {
                    lineStream.ignore();
                }
            }
        }
        csvFile.close();
        //printing line for debug, will be removed in the final file
        cout << *(dataTable.at("grade").data()) << endl;
    }
}
//returns number of columns
    int TimeSeries::countColumns() const {
        return this->columnCount;
    }
//returns number of lines
    unsigned long TimeSeries::countRows() const {
        return this->lineCount;
    }
//returns feature name for a given value
    string TimeSeries::getColumnName(vector<float> values) const {
        string name = "not found";
        //comparing the vector type with all of the vectors in the map
        for (auto &i: this->dataTable) {
            if (i.second == values) {
                name = i.first;
            }
        }
        return name;
    }
    //returns the value of a specific feature in a given time
    float TimeSeries::getTimeValue(string featureName, float time) const {
    int rowCounter = 0;
    //finding the row of the time
    for (auto  & i : this->dataTable.at(this->timeCol)) {
        if (i == time) {
            break;
        }
        rowCounter++;
    }
    //create a new vector for the requested feature
    vector<float> valueVector = this->dataTable.at(featureName);
    //return the value at the requested time using the row number
    return valueVector[rowCounter];
}
//get list of features
    std::list<string> TimeSeries::getFeatures() const {
        list<string> features;
        for (auto &i: this->dataTable) {
            features.push_back(i.first);
        }
        return features;
    }