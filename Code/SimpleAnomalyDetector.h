

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>
#include "minCircle.h"

struct correlatedFeatures{
    string feature1,feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    Circle enc_circle;
    float threshold;
};

struct correlatedindex{
    int corrAIndex, corrBIndex; // indexes of correlated features on vector list
    float corrlation;
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
    // find the maximum deviation of points from given regression line
    virtual float maxDev(Point** points, const Line &regLine, const int size) const {
        float maxDev = 0;
        // goes over points
        for (int i = 0; i < size; i++) {
            // calculate deviation
            float newDev = dev(*points[i], regLine);
            // if deviation is bigger then max found then update it
            if (maxDev < newDev) {
                maxDev = newDev;
            }
            // delete unrelevant point
            delete points[i];
        }
        // delete pointer
        delete[] points;
        return maxDev;
    }

public:
    SimpleAnomalyDetector();
    virtual ~SimpleAnomalyDetector();

    virtual void learnNormal(const TimeSeries& ts);
    virtual vector<AnomalyReport> detect(const TimeSeries& ts);

    virtual vector<correlatedFeatures> getNormalModel(){
        return cf;
    }
    
    // setters
    virtual void setThreshold(float newThreshold) {
        this->threshold = newThreshold;
    }

    // getters
    virtual float getThreshold() {
        return this->threshold;
    }

protected:
    vector<correlatedFeatures> cf;
    float threshold;
    Point** returnPoints(const vector<float>* corrA, const vector<float>* corrB);
    void deletePoints(Point** points, size_t size);
    void updateCorrFeatures(Point** points, string feature1, string feature2, const int& size, float pearson);
    vector<correlatedindex> findCorrelation(const TimeSeries& ts, std::vector<const vector<float>*>& vectors);

    virtual void detectCorrelatedFeatures(const TimeSeries &ts, vector<correlatedindex> corrIndexes,
                                vector<const vector<float> *> listOfVectors, const int numOfRows);

    virtual bool checkAnomaly(correlatedFeatures &corF, Point point);
};



#endif /* SIMPLEANOMALYDETECTOR_H_ */