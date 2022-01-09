

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

class DetectorProxy:public SimpleAnomalyDetector{

	SimpleAnomalyDetector* detector;
	vector<AnomalyReport> reports;
	vector<AnomalyReport> consilidatedReports;
	unsigned long numberOfLines;

public:

	DetectorProxy(SimpleAnomalyDetector* detector):SimpleAnomalyDetector() {
		this->detector = detector;
	}
	
	virtual void learnNormal(const TimeSeries& ts) {
		detector->learnNormal(ts);
	}

    virtual vector<AnomalyReport> detect(const TimeSeries& ts) {
		numberOfLines = ts.countRows();
		reports = detector->detect(ts);
		return reports;
	}

	vector<correlatedFeatures> getNormalModel(){
        return this->detector->getNormalModel();
    }
    
    // setters
    void setThreshold(float newThreshold) {
        this->detector->setThreshold(newThreshold);
    }

    // getters
    float getThreshold() {
        return this->detector->getThreshold();
    }

	unsigned long* getLines() {
		return &this->numberOfLines;
	}

	vector<AnomalyReport>* getReports() {
		return &this->reports;
	}

	~DetectorProxy() {
		delete this->detector;
	}
};


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

	
public:
	MacroCommand(DefaultIO* dio):Command(dio) {
		// empty, loads commands into vector
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
		dio->write("Upload complete.\n");
	}
};

class TrainCsv:public Upload {
public:
	// uses default file name and prompt
	TrainCsv(DefaultIO* dio):Upload(dio) {
		this->fileName = "anomalyTrain.csv";
		setDesc("Please upload your local train CSV file.");
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

public:
	CsvUpload(DefaultIO* dio): MacroCommand(dio) {
		setDesc("upload a time series csv file");
		commands.push_back(new TrainCsv(dio));
		commands.push_back(new TestCsv(dio));
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
			dio->write("Type a new threshold\n");
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
	string testFile;
	string trainFile;

public:
	Detect(DefaultIO* dio, SimpleAnomalyDetector* detector):Command(dio) {
		setDesc("detect anomalies");
		this->detector = detector;
		this->testFile = "anomalyTest.csv";
		this->trainFile = "anomalyTrain.csv";
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		this->detector->learnNormal(TimeSeries(trainFile));
		this->detector->detect(TimeSeries(testFile));
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
// class AnalyzeResults:public Command {
// 	vector<AnomalyReport>* reports;
// public:
// 	AnalyzeResults(DefaultIO* dio, vector<AnomalyReport>* reports):Command(dio) {
// 		setDesc("upload anomalies and analyze results");
// 		this->reports = reports;
// 	}
// 	//CsvUpload():Command(new StdIO()){}

// 	virtual void execute() {
		
// 	}
// };

class AnalyzeResults:public Command {
    vector<AnomalyReport> *reports;
    struct shrunkAnomaly {
        float start_step;
        float end_step;
        string description;
    };
    vector<shrunkAnomaly> inputAnomalies;
    vector<shrunkAnomaly> shrunkAnomalies;
	unsigned long* numberOfLines;

public:
    AnalyzeResults(DefaultIO *dio, vector<AnomalyReport> *reports, unsigned long* numberOfLines) : Command(dio) {
        setDesc("upload anomalies and analyze results");
        this->reports = reports;
		this->numberOfLines = numberOfLines;
    }
    //CsvUpload():Command(new StdIO()){}

    virtual void execute() {
        //getting user input
        getInput();
        //grouping anomalies from original dataset
        groupAnomalies();
        // comparing intersection instances between input-shrunk-anomalies and result-shrunk-anomalies
        unsigned long tp = 0;
        unsigned long fp = 0;
		unsigned long overAlltime = 0;
        float start, end;
        for (shrunkAnomaly anomalyIn: inputAnomalies) {
            start = anomalyIn.start_step;
            end = anomalyIn.end_step;
			overAlltime += start - end + 1;
            for (shrunkAnomaly anomalyRes: shrunkAnomalies) {
                if ((start <= anomalyRes.start_step && anomalyRes.start_step <= end) ||
                    (end >= anomalyRes.end_step && anomalyRes.end_step >= start)) {
                    tp++;
                } else {
                    fp++;
                }
            }
        }
        unsigned long P = inputAnomalies.size();
        unsigned long N = (*numberOfLines) - overAlltime;
        float tpRate = tp/P;
        float fpRate = fp/N;
        dio->write("True Positive Rate:" + to_string(tpRate) + "\n");
        dio->write("False Positive Rate: " + to_string(fpRate) + "\n");
    }
    void getInput(){
        shrunkAnomaly input;
        string in;
        dio->write("Please upload your local anomalies file.\n");
        string line = dio->read();
        //reading the anomalies from user line by line
        while (line.compare("done") != 0) {
            stringstream lineStream(line);
            getline(lineStream, in, ',');
            input.start_step = stof(in);
            getline(lineStream, in, ',');
            input.end_step = stof(in);
            input.description = "userInput";
            //pushing the input line to the final vector
            inputAnomalies.push_back(input);
            line = dio->read();
        }
        dio->write("Upload complete\n");
    }

    void groupAnomalies(){
        //designated comparator for first sort of the reports
        // sort(reports->begin(), reports->end(), [](const AnomalyReport &rep1, const AnomalyReport &rep2) {
        //     return rep1.description < rep2.description;
        // });
        int i = 0, j = 0;
        shrunkAnomaly grouping;
		int reportSize = reports->size() - 1;
        //continue while there are more rows
        while (i != reportSize) {
            j++;
            //check if j is pointing to a different anomaly description
            //meaning the code reached the end of a single description block
            if ((reports[0][i].description).compare(reports[0][j].description) != 0) {
                //sort all of the anomalies of this description according to their timesteps
                // sort(reports->begin()[i], reports->end()[j - 1],
                //      [](const AnomalyReport &rep1, const AnomalyReport &rep2) {
                //          return rep1.timeStep < rep2.timeStep;
                //      });
                //save the pointer to the beginning of the current block
                int first = i;
                //iterate over all the subsets of the block and group together rows with continuous timesteps
                for (int m = i; m <= j - 1; m++) {
                    //if the next timestep is not the consecutive for the current timestep
                    if (reports[0][m].timeStep != (reports[0][m + 1].timeStep - 1)) {
                        //create a new shrunk anomaly with start and end timestep
                        grouping.start_step = reports[0][first].timeStep;
                        grouping.end_step = reports[0][m].timeStep;
                        grouping.description = reports[0][m].description;
                        shrunkAnomalies.push_back(grouping);
                        //continue iterating the rest of the subsets
                        first = m + 1;
                    }
                }
            }
            //when finished the current description block, continue over to the next
            i = j;
        }
    }
};


// finish interaction with user
class Exit:public Command {
	bool* state;
	SimpleAnomalyDetector* detector;
public:
	Exit(DefaultIO* dio, bool* state, SimpleAnomalyDetector* detector):Command(dio) {
		setDesc("exit");
		this->state = state;
		this->detector = detector;
	}
	//CsvUpload():Command(new StdIO()){}

	virtual void execute() {
		*state = false;
		delete this->detector;
	}
};


#endif /* COMMANDS_H_ */
