

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

	// you may add additional methods here
};

// you may add here helper classes




// you may edit this class
class Command{
	string description;
protected:
	DefaultIO* dio;
	void setDesc(string desc) {
		description = desc + "\n";
	}
public:
	Command(DefaultIO* dio):dio(dio) {}
	virtual string getDesc() {return description;}
	virtual void execute()=0;
	virtual ~Command(){}
};

// implement here your command classes

// abstract class for macro commands
class MacroCommand:public Command {
protected:
	vector<Command*> commands;

	virtual void loadCommands()=0;
public:
	MacroCommand(DefaultIO* dio):Command(dio) {
		loadCommands();
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		for (Command* c : commands) {
			c->execute();
		}	
	}

	
	// does vector need deallocation?
};

// Abstract command class for uploading file to server
class Upload:public Command {
protected:
	string fileName;
public:
	Upload(DefaultIO* dio):Command(dio) {}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		dio ->write(getDesc());
		string line = dio->read();
		ofstream file(fileName);
		while (line.compare("done") != 0) {
			file << line << endl;
			line = dio->read();
		}
    	
    	file.close();
		dio->write("‫‪Upload complete\n");
	}
};

class TrainCsv:public Upload {
public:
	// uses default file name and prompt
	TrainCsv(DefaultIO* dio):Upload(dio) {
		this->fileName = "anomalyTrain.csv";
		setDesc("‫‪Please upload your local train CSV file.‬‬");
	}

	// in case of spesific fileName and prompt
	TrainCsv(DefaultIO* dio, string fileName, string prompt):Upload(dio){
		this->fileName = fileName;
		setDesc(prompt);
	}
	
};

class TestCsv:public Upload {
public:
	// uses default file name and prompt
	TestCsv(DefaultIO* dio):Upload(dio) {
		this->fileName = "anomalyTest.csv";
		setDesc("Please upload your local test CSV file.");
	}

	// in case of spesific fileName and prompt
	TestCsv(DefaultIO* dio, string fileName, string prompt):Upload(dio){
		this->fileName = fileName;
		setDesc(prompt);
	}
	
};

// macro class that reads from user and save csv file for time series
class CsvUpload:public MacroCommand{
protected:

	void loadCommands() {
		commands.push_back(new TrainCsv(dio));
		commands.push_back(new TestCsv(dio));
	}

public:
	CsvUpload(DefaultIO* dio): MacroCommand(dio) {
		setDesc("upload a time series csv file");
	}
	//CsvUpload():Command(new StdIO()){}
};

// displays and change algorithm setting
class AlgoSett:public Command {
	SimpleAnomalyDetector* detector;
public:
	AlgoSett(DefaultIO* dio, SimpleAnomalyDetector* detector):Command(dio) {
		setDesc("algorithm settings");
		this->detector=detector;
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		float userIn;
		bool state = true;
		do {
			dio->write("The current correlation threshold is ");
			dio->write(detector->getThreshold());
			dio->write("\n");
			dio->read(&userIn);
			if (userIn >= 0 && userIn <= 1) {
				state = false;
			} else {
				dio->write("please choose a value between 0 and 1.\n");
				string s = dio->read();
			}
		} while (state);
		
		this->detector->setThreshold(userIn);
	}

};

// runs anomaly detection on csv file taht were already given
class Detect:public Command {
	SimpleAnomalyDetector* detector;
	vector<AnomalyReport>* reports;
	string testFile;
	string trainFile;

public:
	Detect(DefaultIO* dio, SimpleAnomalyDetector* detector, vector<AnomalyReport>* reports):Command(dio) {
		setDesc("detect anomalies");
		this->detector = detector;
		this->reports = reports;
		this->testFile = "anomalyTest.csv";
		this->trainFile = "anomalyTrain.csv";
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		this->detector->learnNormal(TimeSeries(trainFile));
		*reports = this->detector->detect(TimeSeries(testFile));
		dio->write("anomaly detection complete.\n");
	}

};

// Displays anomalys detected with time step and desciption
class DisplayAnomad:public Command {
	vector<AnomalyReport>* reports;
public:
	DisplayAnomad(DefaultIO* dio, vector<AnomalyReport>* reports):Command(dio) {
		setDesc("display results");
		this->reports = reports;
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		for (AnomalyReport report : *reports) {
			this->dio->write(report.timeStep);
			this->dio->write("\t" + report.description + "\n");
		}
		dio->write("Done.\n");
	}

};

// reads known anomalies from user and analyze the results according to it
class AnalyzeResults:public Command {
	vector<AnomalyReport>* reports;
public:
	AnalyzeResults(DefaultIO* dio, vector<AnomalyReport>* reports):Command(dio) {
		setDesc("upload anomalies and analyze results");
		this->reports = reports;
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		
	}
};

// finish interaction with user
class Exit:public Command {
	bool* state;
public:
	Exit(DefaultIO* dio, bool* state):Command(dio) {
		setDesc("exit");
		this->state = state;
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		*state = false;
	}
};


#endif /* COMMANDS_H_ */
