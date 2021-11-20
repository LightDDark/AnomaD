
#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::SimpleAnomalyDetector() {
	// TODO Auto-generated constructor stub

}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
	// TODO Auto-generated destructor stub
}


void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
	const int numOfCol = ts.countColumns();
	const string timeName = ts.getTimeName();
	const int numOfRows = ts.countRows();
	std::vector<const vector<float>*> listOfVectors = ts.getColumns();
	for (int i = 0; i < numOfCol; i++)
	{
		if (ts.getColumnName(*listOfVectors[i]) == timeName) {
			continue;
		}
		
		float finalPearson = 0;
		int c = -1;
		for (int j = i + 1; j < numOfCol; j++)
		{
			if(ts.getColumnName(*listOfVectors[j]) == timeName) {
				continue;
			}
			float currentPearson = std::abs(pearson(listOfVectors[i]->data(), listOfVectors[j]->data(), numOfRows));
			if (currentPearson > finalPearson) {
				finalPearson = currentPearson;
				c = j;
			}
		}
		if (c != -1 && finalPearson > this->thershold) {
			correlatedFeatures corrrel;
			Point** points = ts.returnPoints(listOfVectors[i], listOfVectors[c]);
			corrrel.feature1 = ts.getColumnName(*listOfVectors[i]);
			corrrel.feature2 = ts.getColumnName(*listOfVectors[c]);
			corrrel.corrlation = finalPearson;
			corrrel.lin_reg = linear_reg(points, numOfRows);
			corrrel.threshold = maxDev(points, corrrel.lin_reg, numOfRows);
			cf.push_back(corrrel);
		}
	}
	
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
	vector<AnomalyReport> report;
	unsigned long colSize = ts.countRows();
	for (correlatedFeatures &corF : cf) {
		string feature1 = corF.feature1;
		string feature2 = corF.feature2;
		Line lineReg = corF.lin_reg;
		float threshold = corF.threshold;
		Point** points = ts.returnPoints(ts.getValues(feature1), ts.getValues(feature2));
		for (unsigned long j = 0; j < colSize; j++) {
			if (dev(*points[j], lineReg) > threshold) {
				AnomalyReport rep(feature1 + "-" + feature2, j+1);
				report.push_back(rep);
			}
		}	
	}

	// TODO Auto-generated destructor stub
	return  report;
}

