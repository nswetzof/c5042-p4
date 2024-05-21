#include "2PC_Participant.h"

Participant::Participant(const u_short port, const string acc_file_name, const string log_file_name)
    : TCPServer(port) {
    string line;
    string acc_num;
    double balance;
    int index;

    acc_file.open(acc_file_name);
    if (!acc_file.is_open()) {
        throw runtime_error("Failed to open " + acc_file_name);
    }

    // build accounts from file
    while(getline(acc_file, line)) {
        index = line.find(' ');
        balance = atof((line.substr(0, index + 1)).c_str());
        acc_num = line.substr(index + 1, line.length() - index);
        accounts[acc_num] = balance;
        cout << acc_num << " balance: " << balance << endl;
    }

    log_file.open(log_file_name);
}