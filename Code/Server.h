/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "CLI.h"
#include <thread>

#ifndef SERVER_H_
#define SERVER_H_


using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
protected:
    int clientID;
public:
    virtual void handle(int clientID)=0;
};

// you can add helper classes here and implement on the cpp file


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler {
public:
    virtual void handle(int clientID) {
        SocketIO socketIo(clientID);
        CLI clientInterface(&socketIo);
        clientInterface.start();
    }
};

// implement on Server.cpp
class Server {
    bool isOn = true;
    thread* t; // the thread to run the start() method in
    struct sockaddr_in server;
    struct sockaddr_in client;
    //int addLen = sizeof(ipAddress);
    int servFD;
	// you may add data members

public:
	Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
    void stop();
};

#endif /* SERVER_H_ */
