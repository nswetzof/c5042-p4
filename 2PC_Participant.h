#pragma once

#include <fstream>
#include <string>
#include <vector>
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
     * @param incoming_stream_piece Message received from the coordinator.  Valid message types are as follows:
     *  VOTE-REQUEST [amount] [account] - request a transaction from account specified of amount specified (positive or negative)
     *      If the account exists and there are sufficient funds to execute the transaction, a hold will be placed
     *      and a VOTE-COMMIT response will be sent to the coordinator
     *  GLOBAL-COMMIT - execute the transaction
     *  GLOBAL-ABORT - abort the transaction
     * @return true if request is satisfied, false otherwise
    */
    bool process(const std::string &incoming_stream_piece) override;
    // void respond(const std::string &response) override; // TODO: Delete?  Don't see why this needs to be overridden
private:
    struct account {
        double balance;
        bool held;
    };
    unordered_map<string, account> accounts;
    const string acc_file_name, log_file_name;
    // ifstream acc_file;
    // ofstream log_file;

    /**
     * Splits a string into multiple strings defined by a delimiter
     * @return List of strings after splitting the input string
     * @param text String to split
     * @param delimiter Character which delimits the separation
    */
    vector<string> split(const string &text, const char delimiter = ' ');
    bool processDeposit(string acc_num, double balance);
    bool processWithdrawal(string acc_num, double balance);
};