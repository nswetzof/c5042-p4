#include "2PC_Participant.h"

Participant::Participant(const u_short port, const string acc_file_name, const string log_file_name)
    : TCPServer(port), acc_file_name(acc_file_name), log_file_name(log_file_name) {
    string line;
    string acc_num;
    double balance;
    int index;

    ifstream acc_file(acc_file_name);
    if (!acc_file.is_open()) {
        throw runtime_error("Failed to open " + acc_file_name);
    }

    // build accounts from file
    while(getline(acc_file, line)) {
        index = line.find(' ');
        balance = atof((line.substr(0, index + 1)).c_str());
        acc_num = line.substr(index + 1, line.length() - index);
        accounts[acc_num] = balance;
    }

    acc_file.close();
}

Participant::~Participant() {}

bool Participant::process(const string &incoming_stream_piece) {
    // TODO: have to log
    int first_space = incoming_stream_piece.find(' ');
    vector<string> request = split(incoming_stream_piece);
    string type = request.at(0);
    string account;
    double amount;

    if (type == "VOTE-REQUEST") {
        amount = atof(request.at(1).c_str());
        account = request.at(2);

        if (accounts.find(account) != accounts.end()) {
            if (accounts[account].balance + amount >= 0 && !accounts[account].held) {
                accounts[account].held = true;
                respond("VOTE-COMMIT");
                return true;
            }
        }

        respond("VOTE-ABORT");
        return false;
    }

    if (type == "GLOBAL-COMMIT") {
        respond("ACK");
        // TODO: execute transaction and eliminate hold
        // TODO: need to determine protocol for this message type
        /*
        check if account exists
            check if hold has been placed
                execute transaction
                respond that transaction was executed
        else
            failure message
            remove hold
        */
        return true;
    }

    if (type == "GLOBAL-ABORT") {
        respond("ACK");
        // TODO: eliminate hold
        return true;
    }
    // TODO
    /* 
    if deposit
        if account exists
            VOTE-COMMIT
        else
            VOTE-ABORT
    if withdraw
        if account exists and sufficient funds
            VOTE-COMMIT
        else
            VOTE-ABORT
    */
}

vector<string> split(const string &text, const char delimiter) {
    vector<string> result;
    int prev = 0;
    int index = text.find(delimiter);
    while (index != string::npos) {
        result.push_back(text.substr(prev, index - prev));
        prev = index;
        index = text.find(delimiter, index);
    }
}