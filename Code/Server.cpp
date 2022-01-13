
#include "Server.h"

Server::Server(int port)throw (const char*) {
    servFD = socket(AF_INET,SOCK_STREAM,0);
    if (servFD < 0){
        throw "socket initialization failed";
    }
    server.sin_port = htons(port);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    int servSize = sizeof(server);
    int bindPort2IP = ::bind(servFD, (sockaddr*)&server,servSize);
    if (bindPort2IP < 0){
        throw "binding port to IP failed";
    }
    int hearingAid = listen(servFD,3);//change 3 to desired backlog
    if (hearingAid < 0){
        throw "can't listen";
    }

}

void Server::start(ClientHandler& ch)throw(const char*){
    t = new thread([&ch, this](){
        cout<<"waiting.."<<endl;
        int addLen = sizeof(client);
        int newClient = accept(servFD, (sockaddr*)&client, (socklen_t*)&addLen);
        if (newClient < 0){
            throw "couldn't accept client";
        }
        ch.handle(newClient);
        cout<<"done handling"<<endl;
        close(newClient);
        close(servFD);
    });
}

void Server::stop(){
	t->join(); // do not delete this!
}

Server::~Server() {
}

