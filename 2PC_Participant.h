/**
 * @file 2PC_Participant.h
 * @author Nathan Swetzof
 * Represents participant in a 2PC transaction.
*/

#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "TCPServer.h"

using namespace std;

/**
 * Participant class represents one participant in a 2PC transaction
*/
class Participant : public TCPServer {
public:
    /**
     * Constructor loads accounts listed in file
     * @param port port number for the Participant to run on
     * @param acc_file_name name of the file containing the accounts/balances
     * @param log_file_name name of the log file to write program output to
    */
    Participant(const u_short port, const string acc_file_name, const string log_file_name);
    ~Participant();

    /**
     * Write a string to the last line of the log file
     * @param note The string to write to the log file
    */
    void log(const string &note);

protected:
    void start_client(const std::string &their_host, u_short their_port) override;

    /**
     * Process transaction request
     * @param incoming_stream_piece Message received from the coordinator.
     * @return true if request is valid depending on the state of the Participant.
    */
    bool process(const std::string &incoming_stream_piece) override;
private:
    struct account {
        double balance;
        double held;
    };
    unordered_map<string, account> accounts;
    const string acc_file_name, log_file_name;
    enum State {INIT, READY, COMMIT, ABORT};
    State state;

    /**
     * Updates the accounts file associated with this instance with the current balances stored in
     *  the accounts field
    */
    void updateAccounts();

    /**
     * Splits a string into multiple strings defined by a delimiter
     * @return List of strings after splitting the input string
     * @param text String to split
     * @param delimiter Character which delimits the separation
    */
    static vector<string> split(const string &text, const char delimiter = ' ');

    /**
     * @return Input string with whitespace characters removed from the end
    */
    static string rtrim(const string &s);
};