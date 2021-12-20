//
// Created by Maayan Shavit on 18/12/2021.
//

#ifndef ANOMALY_DETECTOR_HYBRIDANOMALYDETECTOR_H
#define ANOMALY_DETECTOR_HYBRIDANOMALYDETECTOR_H

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector{
    const float thresholdSimple;
    const float thresholdHybrid;
public:
    HybridAnomalyDetector();
    virtual ~HybridAnomalyDetector();

    void detectCorrelatedFeatures(const TimeSeries &ts, vector<correlatedindex> corrIndexes,
                                  vector<const vector<float> *> listOfVectors, const int numOfRows);

    bool checkAnomaly(correlatedFeatures &corF, Point point);

    void updateCorrMEC(Point **points, string feature1, string feature2, const int &size, float pearson);
};


#endif //ANOMALY_DETECTOR_HYBRIDANOMALYDETECTOR_H
