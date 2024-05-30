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
        acc_num = line.substr(index + 1, line.length() - index - 2); // FIXME: handle this better (newlines not consistent across systems)
        // acc_num.erase(std::find_if(acc_num.begin(), acc_num.end(), [](int c) {return isspace(c);}));

        accounts[acc_num].balance = balance;
        accounts[acc_num].held = 0;
        log(acc_num.length() + ", " + acc_num + ": " + to_string(accounts[acc_num].balance) + '\n');
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
            if (accounts[account].balance - accounts[account].held + amount >= 0) {
                accounts[account].held += amount;
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
        // TODO: execute transaction and eliminate hold
        account = request.at(1);
        if (accounts.find(account) != accounts.end()) {
            accounts[account].balance -= accounts[account].held;
            accounts[account].held = 0;

            // TODO: update accounts file
            updateAccounts();
        }
        else
            throw runtime_error("Could not commit.  " + account + " not found");
        
        respond("ACK"); // TODO: Should this be before or after transaction?
        return true;
    }

    if (type == "GLOBAL-ABORT") {
        respond("ACK");
        
        account = request.at(1);

        if (accounts.find(account) != accounts.end())
            accounts[account].held = 0;
        else
            throw runtime_error("No transaction to abort.  " + account + " not found");

        return true;
    }

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

void Participant::updateAccounts() {
    ofstream file(acc_file_name);
    // ifstream file(acc_file_name);
    // stringstream ss();

    unordered_map<string, account>::const_iterator iter = accounts.begin();

    while (iter != accounts.end())
        file << iter->first << ' ' << iter->second.balance << endl;

    file.close();
}

void Participant::log(const string &note) {
    ofstream log_file(log_file_name, fstream::app);

    if (log_file.is_open())
        log_file << note;
    
    log_file.close();
}