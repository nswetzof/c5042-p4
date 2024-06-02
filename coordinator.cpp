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

    // string source_host = argv[3], dest_host = argv[6];
    // int source_port = atoi(argv[4]), dest_port = atoi(argv[7]);
    // string transfer = argv[2]; // TODO: handle numbers < 0
    // string source_account = argv[5], dest_account = argv[8];
    // string source_response, dest_response;
    // string message;
    ofstream logfile(LOG_FILE_NAME);

    if (!logfile.is_open())
        throw runtime_error("Failed to open " + LOG_FILE_NAME);

    // logfile << "Transaction: $" << transfer
    //     << "\n\tFrom:\t" << source_host << ":" << source_port << " account #" << source_account
    //     << "\n\tTo:\t" << dest_host << ":" << dest_port << " account #" << dest_account << endl;

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
    for (size_t i = 0; i < transactions.size(); i++) {
        string response = transactions.at(i).client.get_response();
        logfile << "Got " << response << " from " << transactions.at(i).host 
            << ":" << transactions.at(i).port << endl;
    }

    // TCPClient source_client(source_host, source_port);
    // TCPClient dest_client(dest_host, dest_port);
    // int transaction_id = 1; // TODO: modify or delete
    // string tid = to_string(transaction_id);

    // logfile << "Connected to participant " << source_host << ":" << source_port << endl;
    // logfile << "Connected to participant " << dest_host << ":" << dest_port << endl;

    // // make requests to participants
    // logfile << "Sending message '" << VOTE_REQUEST << "' " << source_account << " -" << transfer << " to " 
    //     << source_host << ":" << source_port << endl;
    // source_client.send_request(VOTE_REQUEST + " -" + transfer + " " + source_account);

    // logfile << "Sending message '" << VOTE_REQUEST << "' " << dest_account << " " << transfer << " to " 
    //     << dest_host << ":" << dest_port << endl;
    // dest_client.send_request(VOTE_REQUEST + " " + transfer + " " + dest_account);

    // // get participant responses
    // source_response = source_client.get_response();
    // logfile << "Got " << source_response << " from " << source_host << ":" << source_port << endl;
    // dest_response = dest_client.get_response();
    // logfile << "Got " << dest_response << " from " << dest_host << ":" << dest_port << endl;

    // if (source_response == COMMIT_RESPONSE
    //     && dest_response == COMMIT_RESPONSE)
    //     message = GLOBAL_COMMIT;
    // else
    //     message = GLOBAL_ABORT;

    // // send GLOBAL-COMMIT or GLOBAL-ABORT
    // logfile << "Sending message '" << message << "' to " 
    //     << source_host << ":" << source_port << endl;
    // source_client.send_request(message + ' ' + source_account);

    // logfile << "Sending message '" << message << "' to " 
    //     << dest_host << ":" << dest_port << endl;
    // dest_client.send_request(message + ' ' + dest_account);

    // // wait for ACKs
    // source_response = source_client.get_response();
    // dest_response = dest_client.get_response();

    // if (source_response == ACK && dest_response == ACK) {
    //     if (message == GLOBAL_COMMIT)
    //         logfile << "Transaction committed." << endl;
    //     else
    //         logfile << "Transaction aborted." << endl;
    // }
    // else {
    //     logfile << "Invalid response\n\tsource response: " << source_response
    //         << "\n\tdestination response: " << dest_response << endl;
    //     return EXIT_FAILURE;
    // }

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