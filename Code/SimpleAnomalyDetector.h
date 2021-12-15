

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
	string feature1,feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
	vector<correlatedFeatures> cf;
	const float thershold = 0.3;

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

	vector<correlatedFeatures> getNormalModel(){
		return cf;
	}

};



#endif /* SIMPLEANOMALYDETECTOR_H_ */
