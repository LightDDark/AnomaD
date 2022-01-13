

#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
	DefaultIO* dio;
	vector<Command*> commands;
	string description;
	bool on;

	// you can add data members

	// add relevant commands to vector
	void loadCommands();
	// prints menu and get user input
	int menu();
public:
	CLI(DefaultIO* dio);
	void start();
    int getMenu();
	virtual ~CLI();
};

#endif /* CLI_H_ */
