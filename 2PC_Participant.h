#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include "TCPServer.h"

using namespace std;

class Participant : TCPServer {
public:
    /**
     * Constructor stores accounts listed in file
    */
    Participant(const u_short port, const string acc_file_name, const string log_file_name);
    ~Participant();
protected:
    /**
     * Process transaction request
    */
    bool process(const std::string &incoming_stream_piece) override;
    void respond(const std::string &response) override;
private:
    unordered_map<string, double> accounts;
    ifstream acc_file;
    ofstream log_file;

    
};