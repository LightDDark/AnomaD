
#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::SimpleAnomalyDetector() {
    threshold = 0.9;

}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
    // TODO Auto-generated destructor stub
}

/*
*Given a timeseries, learns the normal correlatives and tresholds.
*/
void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
    const int numOfCol = ts.countColumns();
    const string timeName = ts.getTimeName();
    const int numOfRows = ts.countRows();
    // get pointers to vectors of values
    std::vector<const vector<float>*> listOfVectors = ts.getColumns();
    // go over the vector and check corellation
    vector<correlatedindex> corrIndexes = findCorrelation(ts, listOfVectors);
    this->detectCorrelatedFeatures(ts, corrIndexes, listOfVectors, numOfRows);
}

void SimpleAnomalyDetector::detectCorrelatedFeatures(const TimeSeries& ts, vector<correlatedindex> corrIndexes,
                                                   std::vector<const vector<float>*> listOfVectors, const int numOfRows){
    for (correlatedindex corrIn : corrIndexes) {
        // if we found correlation that's bigger than the treshold then update cf
        if (corrIn.corrBIndex != -1 && (corrIn.corrlation >= this->threshold)) {
            //correlatedFeatures corrrel;
            //gets te point in order to calculate reg_line
            Point** points = returnPoints(listOfVectors[corrIn.corrAIndex], listOfVectors[corrIn.corrBIndex]);
            // update cf
            updateCorrFeatures(points, ts.getColumnName(*listOfVectors[corrIn.corrAIndex]), ts.getColumnName(*listOfVectors[corrIn.corrBIndex]),
                               numOfRows, corrIn.corrlation);
        }
    }
}

/*
* Detects anomalies in given timeseries in relation to what we learned from normal
*/
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
    vector<AnomalyReport> report;
    unsigned long colSize = ts.countRows();
    //this->rowCounter = colSize;
    // goes on every correlation we found in normal mode
    for (correlatedFeatures &corF : cf) {
        string feature1 = corF.feature1;
        string feature2 = corF.feature2;
        //Line lineReg = corF.lin_reg;
        float threshold = corF.threshold;
        // get the current points of the features
        Point** points = returnPoints(ts.getValues(feature1), ts.getValues(feature2));
        // for every point
        for (unsigned long j = 0; j < colSize; j++) {
            // check deviation from the linear regression we found in normal and compare to threshold
            if (checkAnomaly(corF, *points[j])) {
                // if bigger than threshold add to the report
                AnomalyReport rep(feature1 + "-" + feature2, j+1);
                report.push_back(rep);
            }
            delete points[j];
        }
        delete[] points;
    }

    // return report
    return  report;
}
bool SimpleAnomalyDetector::checkAnomaly(correlatedFeatures &corF, Point point){
    return (dev(point, corF.lin_reg) > corF.threshold);
}

//returning ptr to array of ptrs to Point objects from two correlated features
Point** SimpleAnomalyDetector::returnPoints(const vector<float>* corrA, const vector<float>* corrB) {
    unsigned long size = (*corrA).size();
    Point **pArray = new Point*[size];
    //for each ptr of Point, insert new point using the values from the feature data vector
    for (int i = 0; i < size; i++) {
        auto *p = new Point((*corrA)[i], (*corrB)[i]);
        //insert the point to the new array
        pArray[i] = p;
    }
    return pArray;
}

// deletes created points
void SimpleAnomalyDetector::deletePoints(Point** points, size_t size) {
    for (size_t i = 0; i < size; i++) {
        delete points[i];
    }
    delete[] points;
}

void SimpleAnomalyDetector::updateCorrFeatures(Point** points, string feature1, string feature2, const int& size, float pearson) {
    correlatedFeatures corrrel;
    corrrel.feature1 = feature1;
    corrrel.feature2 = feature2;
    corrrel.corrlation = pearson;
    // gets reg line
    corrrel.lin_reg = linear_reg(points, size);
    // set threshold as max deviation from line
    corrrel.threshold = maxDev(points, corrrel.lin_reg, size) * 1.1;
    // add to cf
    cf.push_back(corrrel);
}

vector<correlatedindex> SimpleAnomalyDetector::findCorrelation(const TimeSeries& ts, std::vector<const vector<float>*>& vectors) {
    const int numOfCol = ts.countColumns();
    const string timeName = ts.getTimeName();
    const int numOfRows = ts.countRows();
    vector<correlatedindex> corrIndexes;
    // go over the vector and check corellation
    for (int i = 0; i < numOfCol; i++)
    {
        // if current vector is Time column then skip
        if (timeName.compare("") != 0 && ts.getColumnName(*vectors[i]).compare(timeName) == 0) {
            continue;
        }

        float finalPearson = 0;
        // intitialize a var to check if correlation found
        int c = -1;
        // check corellation of current vector with others after it
        for (int j = i + 1; j < numOfCol; j++)
        {
            // if current vector is Time column then skip
            if(timeName.compare("") != 0 && ts.getColumnName(*vectors[i]).compare(timeName) == 0) {
                continue;
            }
            const float* first = vectors[i]->data();
            // gets the pearson and compare it with the maximum checked
            float currentPearson = std::abs(pearson(vectors[i]->data(), vectors[j]->data(), numOfRows));
            if (currentPearson > finalPearson) {
                finalPearson = currentPearson;
                c = j;
            }
        }
        corrIndexes.push_back(correlatedindex{i, c, finalPearson});
    }
    return corrIndexes;
}