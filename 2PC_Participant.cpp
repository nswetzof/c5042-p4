#include "2PC_Participant.h"

#define DEBUG 1 // TODO: delete

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
        accounts[acc_num].balance = balance;
        accounts[acc_num].held = false;
    }

    acc_file.close();

    serve();

// #if DEBUG == 1
// #include <iterator>
//     unordered_map<string, account>::const_iterator iter;
//     for (iter = accounts.begin(); iter != accounts.end(); iter++)
//         cout << iter->first << endl;
// #endif
}

Participant::~Participant() {}

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
                respond("VOTE-COMMIT");
                return true;
            }
        }

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
        prev = index;
        index = text.find(delimiter, index);
    }

    return result;
}

void Participant::log(const string &note) {
    ofstream log_file(log_file_name);

    if (log_file.is_open())
        log_file.write(note.c_str(), note.length());
}