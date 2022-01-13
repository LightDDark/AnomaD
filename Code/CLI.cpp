#include "CLI.h"
#include <cassert>

CLI::CLI(DefaultIO* dio) {
    description = "Anomaly Detection Server.";
    this->dio = dio;
    loadCommands();
    //Probably more
}

void CLI::start(){
    this->on = true;
    int opt;
    // option the user will pick
    // prints menu and excute command on a loop
    while (on) {
        opt = menu();

        commands[opt]->execute();
        // write an intro for the the server
        // excute user choice
    }
}
int CLI::menu() {
    // prints menu
    int i = 1;
    dio->write("Welcome to the " + description + "\n");
    dio->write("Please choose an option:\n");
    for (Command* c : commands) {
        dio->write(std::to_string(i) + "." + c->getDesc());
        i++;
    }
    // get user input for option
    float option;
    dio->read(&option);
    // check if valid
    assert(option > 0 && option <= 6 && floorf(option) == option);
    return option - 1;
}

int CLI::getMenu(){
    return this->menu();
}

void CLI::loadCommands() {
    DetectorProxy* detector = new DetectorProxy(new HybridAnomalyDetector());
    // add commands to vector here.
    commands.push_back(new CsvUpload(dio));
    commands.push_back(new AlgoSett(dio, detector));
    commands.push_back(new Detect(dio, detector));
    commands.push_back(new DisplayAnomad(dio, detector->getReports()));
    commands.push_back(new AnalyzeResults(dio, detector->getReports(), detector->getLines()));
    commands.push_back(new Exit(dio, &this->on, detector));
}

CLI::~CLI() {
}
