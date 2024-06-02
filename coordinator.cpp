#include <future>
#include <fstream>
#include <iostream>
#include <vector>
#include "TCPClient.h"

using namespace std;

/**
 * Store the information necessary to carry out a transaction with a Participant
*/
struct Transaction {
    string host;
    u_short port;
    TCPClient client;
    string account;
    double amount;
};

void readTransactions(const string &filename, vector<Transaction> &transaction, ostream &log);

/**
 * Write the contents of a file to an output stream
 * @param output The output stream to write to
 * @param filename The name of the file to read from
*/
void displayFileContents(ostream &output, string filename);

vector<string> split(const string &text, const char delimeter = ' ');
string rtrim(const string &s);

int main(int argc, char *argv[]) {
    const string LOG_FILE_NAME = argv[1];
    const string TRANSACTION_FILE_NAME = argv[2];
    const string VOTE_REQUEST = "VOTE-REQUEST";
    const string COMMIT_RESPONSE = "VOTE-COMMIT";
    const string ABORT_RESPONSE = "VOTE-ABORT";
    const string GLOBAL_COMMIT = "GLOBAL-COMMIT";
    const string GLOBAL_ABORT = "GLOBAL-ABORT";
    const string ACK = "ACK";

    string response;
    ofstream logfile(LOG_FILE_NAME);

    if (!logfile.is_open())
        throw runtime_error("Failed to open " + LOG_FILE_NAME);

    // create transactions
    vector<Transaction> transactions;
    readTransactions(TRANSACTION_FILE_NAME, transactions, logfile);

    for (size_t i = 0; i < transactions.size(); i++) {
        logfile << "Connected to participant " << transactions.at(i).host << ":" 
            << transactions.at(i).port << endl;
    }

    // make requests to participants
    for (size_t i = 0; i < transactions.size(); i++) {
        logfile << "Sending message '" << VOTE_REQUEST << "' " << transactions.at(i).account
            << " " << transactions.at(i).amount << " to " 
            << transactions.at(i).host << ":" << transactions.at(i).port << endl;

        transactions.at(i).client.send_request(VOTE_REQUEST + " " 
            + to_string(transactions.at(i).amount) + " " 
            + transactions.at(i).account);
    }

    // get participant responses
    string message = GLOBAL_COMMIT;
    for (size_t i = 0; i < transactions.size(); i++) {
        response = transactions.at(i).client.get_response();
        logfile << "Got " << response << " from " << transactions.at(i).host 
            << ":" << transactions.at(i).port << endl;
        if (response != COMMIT_RESPONSE)
            message = GLOBAL_ABORT;
    }

    // send GLOBAL-COMMIT or GLOBAL-ABORT
    for (size_t i = 0; i < transactions.size(); i++) {
        logfile << "Sending message '" << message << "' to "
            << transactions.at(i).host << ":" << transactions.at(i).port << endl;
        transactions.at(i).client.send_request(message + ' ' + transactions.at(i).account);
    }

    // wait for ACKs
    bool acknowledged = true;
    for (size_t i = 0; i < transactions.size(); i++) {
        if (transactions.at(i).client.get_response() != ACK)
            acknowledged = false;
    }

    if (acknowledged)
        logfile << "Transaction committed." << endl;
    else {
        logfile << "Invalid response.  Responses must all be " + ACK << "." << endl;
        return EXIT_FAILURE;
    }

    // TODO: Extra Credit: start new thread on request?
    /*
        take file input
        each line specifies a transaction id, host, port, amount, and account number
        store list of unique transaction ids?
    */

    logfile.close();

    displayFileContents(cout, LOG_FILE_NAME);
}

void readTransactions(const string &filename, vector<Transaction> &transactions, ostream &log) {
    string line;
    string host;
    u_short port;
    string account;
    double amount;
    double total = 0;
    vector<string> transaction_info;

    ifstream file(filename);

    log << "Transactions:\n";
    while (getline(file, line)) {
        transaction_info = split(line);
        host = transaction_info.at(1);
        port = atoi(transaction_info.at(2).c_str());
        TCPClient client(host, port);
        account = rtrim(transaction_info.at(3));
        amount = atof(transaction_info[0].c_str());
        total += amount;

        transactions.push_back(Transaction {host, port, TCPClient(move(client)), account, amount});

        log << "\t$" << abs(amount) << ((amount < 0) ? " from " : " to ") << host << ":" << port
            << " account #" << account << '\n';
    }

    file.close();

    if (total != 0)
        throw runtime_error("Transactions do not add up to zero.");
}

void displayFileContents(ostream &output, string filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line))
        output << line << endl;

    file.close();
}

// TODO: Put these somewhere where I'm not duplicating code from participant
vector<string> split(const string &text, const char delimiter) {
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

string rtrim(const string &s) {
    int last = s.length() - 1;
    while (isspace(s[last]))
        last--;
    
    return s.substr(0, last + 1);
}