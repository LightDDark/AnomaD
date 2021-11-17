
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
	std::vector<vector<float>*> listOfVectors = ts.getColumns();
	for (int i = 0; i < numOfCol; i++)
	{
		if (ts.getColumnName(listOfVectors[i]) == timeName) {
			continue;
		}
		
		float currentPearson = 0;
		int c = -1;
		for (int j = i + 1; j < numOfCol; j++)
		{
			if(ts.getColumnName(listOfVectors[j]) == timeName) {
				continue;
			}
			float absPearson = std::abs(pearson(listOfVectors[i]->data(), listOfVectors[j]->data(), numOfRows));
			if (absPearson > currentPearson) {
				currentPearson = absPearson;
				c = j;
			}
		}
		if (c != -1 && currentPearson > thershold) {
			correlatedFeatures corrrel;
			corrrel.feature1 = ts.getColumnName(listOfVectors[i]);
			corrrel.feature2 = ts.getColumnName(listOfVectors[c]);
			corrrel.corrlation = currentPearson;
			corrrel.lin_reg = linear_reg(ts.returnPoints(ts.getColumnName(listOfVectors[i]), ts.getColumnName(listOfVectors[c])), numOfRows);
			corrrel.threshold = this->thershold;
			cf.push_back(corrrel);
		}
	}
	
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
	vector<AnomalyReport> report;
	int corSize = cf.size();
	unsigned long colSize = ts.countRows();
	for (int i = 0; i < corSize; i++) {
		string feature1 = cf[i].feature1;
		string feature2 = cf[i].feature2;
		Line lineReg = cf[i].lin_reg;
		float threshold = cf[i].threshold;
		Point** points = ts.returnPoints(feature1, feature2);
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

