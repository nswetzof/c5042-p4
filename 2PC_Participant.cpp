#include "2PC_Participant.h"

Participant::Participant(const u_short port, const string acc_file_name, const string log_file_name)
    : TCPServer(port), acc_file_name(acc_file_name), log_file_name(log_file_name), state(INIT) {
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
        acc_num = rtrim(line).substr(index + 1, line.length() - index - 1);

        accounts[acc_num].balance = balance;
        accounts[acc_num].held = 0;
    }
    
    acc_file.close();
}

Participant::~Participant() {}

void Participant::start_client(const std::string &their_host, u_short their_port) {
    log("Accepting coordinator connection. State: INIT");
}

bool Participant::process(const string &incoming_stream_piece) {
    vector<string> request = split(incoming_stream_piece);
    string type = request.at(0);
    string account;
    double amount;

    if (state == INIT) {
        if (type != "VOTE-REQUEST") {
            log("Invalid request " + type + ". State: INIT");
            return false;
        }

        amount = atof(request.at(1).c_str());
        account = request.at(2);
        log("Holding " + to_string(amount) + " from account " + account);

        if (accounts.find(account) != accounts.end()) {
            if (accounts[account].balance - accounts[account].held + amount >= 0) {
                accounts[account].held -= amount;
                log("Got " + type + ", replying VOTE-COMMIT.  State: READY");
                respond("VOTE-COMMIT");
                state = READY;
            }
            else {
                log("Got " + type + ", replying VOTE-ABORT.  State: ABORT");
                respond("VOTE-ABORT");
                state = ABORT;
            }

            return true;
        }
        else {
            log("Error: " + account + " not found");
            return false;
        }
    }

    else if (state == READY || state == ABORT) {
        
        if (type == "GLOBAL-COMMIT") {
            account = request.at(1);

            if (accounts.find(account) == accounts.end()) {
                log("Error: " + account + " not found");
                return false;
            }

            state = COMMIT;
            log("Got GLOBAL-COMMIT, replying ACK. State: COMMIT");
            respond("ACK");
            
            log("Committing " + to_string(-1 * accounts[account].held) + " from account " + account + '\n');
            accounts[account].balance -= accounts[account].held;
            accounts[account].held = 0;
            updateAccounts();
            state = INIT;
        }
        else if (type == "GLOBAL-ABORT") {
            account = request.at(1);
            
            if (accounts.find(account) == accounts.end()) {
                log("Error: " + account + " not found");
                return false;
            }

            state = ABORT;
            log("Got GLOBAL-ABORT, replying ACK. State: ABORT");
            respond("ACK");
            
            log("Releasing hold from account " + account + '\n');
            account = request.at(1);
            accounts[account].held = 0;

            state = INIT;
        }
        else {
            log("Invalid request " + type + ". State: READY");
            accounts[account].held = 0;
            return false;
        }

        return true;
    }

    return false;
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

string Participant::rtrim(const string &s) {
    int last = s.length() - 1;
    while (isspace(s[last]))
        last--;
    
    return s.substr(0, last + 1);
}

void Participant::updateAccounts() {
    ofstream file(acc_file_name);

    unordered_map<string, account>::const_iterator iter = accounts.begin();
    file.precision(2);

    while (iter != accounts.end()) {
        file << fixed << iter->second.balance << ' ' << iter->first << endl;
        iter++;
    }

    file.close();
}

void Participant::log(const string &note) {
    ofstream logfile(log_file_name, fstream::app);

    if (logfile.is_open())
        logfile << note << endl;
    
    logfile.close();
}