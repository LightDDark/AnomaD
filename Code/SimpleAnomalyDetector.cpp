
#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::SimpleAnomalyDetector() {
	// TODO Auto-generated constructor stub

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
	for (int i = 0; i < numOfCol; i++)
	{
		// if current vector is Time column then skip
		if (timeName.compare("") != 0 && ts.getColumnName(*listOfVectors[i]).compare(timeName) == 0) {
			continue;
		}
		
		float finalPearson = 0;
		// intitialize a var to check if correlation found
		int c = -1;
		// check corellation of current vector with others after it
		for (int j = i + 1; j < numOfCol; j++)
		{
			// if current vector is Time column then skip
			if(timeName.compare("") != 0 && ts.getColumnName(*listOfVectors[i]).compare(timeName) == 0) {
				continue;
			}
			const float* first = listOfVectors[i]->data();
			// gets the pearson and compare it with the maximum checked
			float currentPearson = std::abs(pearson(listOfVectors[i]->data(), listOfVectors[j]->data(), numOfRows));
			if (currentPearson > finalPearson) {
				finalPearson = currentPearson;
				c = j;
			}
		}
		// if we found correlation that's bigge than the treshold then update cf
		if (c != -1 && finalPearson > this->thershold) {
			correlatedFeatures corrrel;
			//gets te point in order to calculate reg_line
			Point** points = ts.returnPoints(listOfVectors[i], listOfVectors[c]); //remember to delete points
			corrrel.feature1 = ts.getColumnName(*listOfVectors[i]);
			corrrel.feature2 = ts.getColumnName(*listOfVectors[c]);
			corrrel.corrlation = finalPearson;
			// gets reg line
			corrrel.lin_reg = linear_reg(points, numOfRows);
			// set threshold as max deviation from line
			corrrel.threshold = maxDev(points, corrrel.lin_reg, numOfRows) * 1.1;
			// add to cf
			cf.push_back(corrrel);
		}
	}
	
}

/*
* Detects anomalies in given timeseries in relation to what we learned from normal
*/
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
	vector<AnomalyReport> report;
	unsigned long colSize = ts.countRows();
	// goes on every correlation we found in normal mode
	for (correlatedFeatures &corF : cf) {
		string feature1 = corF.feature1;
		string feature2 = corF.feature2;
		Line lineReg = corF.lin_reg;
		float threshold = corF.threshold;
		// get the current points of the features
		Point** points = ts.returnPoints(ts.getValues(feature1), ts.getValues(feature2));
		// for every point
		for (unsigned long j = 0; j < colSize; j++) {
			// check deviation from the linear regression we found in normal and compare to threshold
			if (dev(*points[j], lineReg) > threshold) {
				// if bigger than threshold add to the report
				AnomalyReport rep(feature1 + "-" + feature2, j+1);
				report.push_back(rep);
			}
		}	
	}

	// return report
	return  report;
}

