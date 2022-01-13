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
class ClientHandler: public DefaultIO{
    int clientID;
    public:
    virtual void handle(int clientID)=0;
    virtual string read(){
        char readingSpace[1024];
        recv(clientID,readingSpace,1024,0);
        string readStr(readingSpace);
        return readStr;
    }
    virtual void write(string text) {
        send(clientID, text.c_str(), sizeof(text), 0);
    }

    virtual void write(float f){
        send(clientID, to_string(f).c_str(), sizeof(float),0);
    }

    virtual void read(float* f){
        recv(clientID,f,sizeof(f),0);
    }
};


// you can add helper classes here and implement on the cpp file


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler {
    int clientID;
public:
    virtual void handle(int clientID) {
        this->clientID = clientID;
        CLI clientInterface(this);
        int firstOpt = clientInterface.getMenu();
        char readingSpace[1024];
        //send(clientID, firstOpt, sizeof(firstOpt), 0);
        int r = recv(clientID, readingSpace, 1024,0);
        cout<<readingSpace<<endl;
        clientInterface.start(firstOpt);
    }
};

// implement on Server.cpp
class Server {
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