#include "CLI.h"
#include <cassert>

CLI::CLI(DefaultIO* dio) {
    description = "‫‪Anomaly‬‬ ‫‪Detection‬‬ ‫‪Server‬‬";
    this->dio = dio;
    loadCommands();
    //Probably more
}

void CLI::start(){
    this->on = true;
    // option the user will pick
    int opt;
    // write an intro for the the server
    dio->write("‫‪Welcome‬‬ ‫‪to‬‬ ‫‪the‬‬ " + description + "\n");
    // prints menu and excute command on a loop
    while (on) {
        opt = menu();
        // excute user choice
        commands[opt]->execute();
    }
}

int CLI::menu() {
    // prints menu
    int i = 1;
    for (Command* c : commands) {
        dio->write(i + ". " + c->getDesc());
        i++;
    }
    // get user input for option
    float option;
    dio->read(&option);
    // check if valid
    assert(option > 0 && option <= 6 && floorf(option) == option);
    return option - 1;
}

void CLI::loadCommands() {
    // add commands to vector here.
    commands.push_back(new CsvUpload(dio));
    commands.push_back(new AlgoSett(dio, &this->detector));
    commands.push_back(new Detect(dio, &this->detector, &this->reports));
    commands.push_back(new DisplayAnomad(dio, &this->reports));
    commands.push_back(new AnalyzeResults(dio, &this->reports));
    commands.push_back(new Exit(dio, &this->on));
}

CLI::~CLI() {
}

