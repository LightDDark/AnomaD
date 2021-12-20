//
// Created by Maayan Shavit on 18/12/2021.
//
#include "HybridAnomalyDetector.h"
HybridAnomalyDetector::HybridAnomalyDetector() : thresholdHybrid( 0.5 ),thresholdSimple ( 0.9 ) {
    // TODO Auto-generated constructor stub

}

HybridAnomalyDetector::~HybridAnomalyDetector() {
    // TODO Auto-generated destructor stub
}

void HybridAnomalyDetector::detectCorrelatedFeatures(const TimeSeries& ts, vector<correlatedindex> corrIndexes,
                                                     std::vector<const vector<float>*> listOfVectors, const int numOfRows){
    for (correlatedindex corrIn : corrIndexes) {
        // if we found correlation that's bigger than the treshold then update cf
        if (corrIn.corrBIndex != -1 && (corrIn.corrlation >= this->thresholdSimple)) {
            //correlatedFeatures corrrel;
            //gets te point in order to calculate reg_line
            Point** points = returnPoints(listOfVectors[corrIn.corrAIndex], listOfVectors[corrIn.corrBIndex]);
            // update cf
            updateCorrFeatures(points, ts.getColumnName(*listOfVectors[corrIn.corrAIndex]), ts.getColumnName(*listOfVectors[corrIn.corrBIndex]),
                               numOfRows, corrIn.corrlation);
        }
        else if (corrIn.corrBIndex != -1 && (corrIn.corrlation >= this->thresholdHybrid)){
            Point** points = returnPoints(listOfVectors[corrIn.corrAIndex], listOfVectors[corrIn.corrBIndex]);
            updateCorrMEC(points,ts.getColumnName(*listOfVectors[corrIn.corrAIndex]), ts.getColumnName(*listOfVectors[corrIn.corrBIndex]),
                          numOfRows, corrIn.corrlation);
        }
    }
}

void HybridAnomalyDetector::updateCorrMEC(Point** points, string feature1, string feature2, const int& size, float pearson) {
    correlatedFeatures correl;
    correl.feature1 = feature1;
    correl.feature2 = feature2;
    correl.corrlation = pearson;
    // gets reg line
    correl.enc_circle = findMinCircle(points, size);
    // set threshold as radius of enclosing circle
    correl.threshold = correl.enc_circle.radius * 1.1;
    // add to cf
    cf.push_back(correl);
}

bool HybridAnomalyDetector::checkAnomaly(correlatedFeatures &corF, Point point){
    return (isWithin(corF.enc_circle, point));
}

