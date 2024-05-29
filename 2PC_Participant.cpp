#include "2PC_Participant.h"

#define DEBUG 1 // TODO: delete

Participant::Participant(const u_short port, const string acc_file_name, const string log_file_name)
    : TCPServer(port), acc_file_name(acc_file_name), log_file_name(log_file_name) {
    string line;
    string acc_num;
    double balance;
    int index;

    // TODO: make sure this is correct behavior
    // clear out log file
    ofstream log_file = ofstream(log_file_name);
    log_file.close();

    ifstream acc_file(acc_file_name);
    if (!acc_file.is_open()) {
        throw runtime_error("Failed to open " + acc_file_name);
    }

    // build accounts from file
    while(getline(acc_file, line)) {
        index = line.find(' ');
        balance = atof((line.substr(0, index + 1)).c_str());
        acc_num = line.substr(index + 1, line.length() - index - 1);

        accounts[acc_num].balance = balance;
        accounts[acc_num].held = false;

        // TODO: delete debugging line
        log("string length: " + to_string(acc_num.length()) + ", " + acc_num + ": " + to_string(accounts[acc_num].balance) + '\n');
    }
    
    acc_file.close();
}

Participant::~Participant() {}

void Participant::start_client(const std::string &their_host, u_short their_port) {
    log("Accepting coordinator connection. State: INIT\n");
}

bool Participant::process(const string &incoming_stream_piece) {
    // TODO: have to log
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
                log("Got " + type + ", replying VOTE-COMMIT.  State: READY");
                respond("VOTE-COMMIT");
                return true;
            }
        }

        log("Got " + type + ", replying VOTE-ABORT.  State: ABORT");
        respond("VOTE-ABORT");
        return true;
    }

    if (type == "GLOBAL-COMMIT") {
        respond("ACK"); // TODO: Should this be before or after transaction?

        // if (accounts.find(account) != accounts.end() && accounts[account].held) {
        //     accounts[account].balance += 
        // }

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

   return false; // FIXME: This will result in the server closing.  Make sure this should be here.
}

vector<string> Participant::split(const string &text, const char delimiter) {
    vector<string> result;
    int prev = 0;
    size_t index = text.find(delimiter);
    while (index != string::npos) {
        result.push_back(text.substr(prev, index - prev));
        prev = index + 1;
        index = text.find(delimiter, index + 1);
    }
    result.push_back(text.substr(prev, text.length()));

    return result;
}

void Participant::log(const string &note) {
    ofstream log_file(log_file_name, fstream::app);

    if (log_file.is_open())
        log_file << note;
    
    log_file.close();
}