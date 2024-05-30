#include <fstream>
#include <iostream>
#include "TCPClient.h"

using namespace std;

/**
 * Write the contents of a file to an output stream
 * @param output The output stream to write to
 * @param filename The name of the file to read from
*/
void displayFileContents(ostream &output, string filename);

// TODO: delete comment?
/**
 * @param argv string array with elements representing 
 *  executable file, log file, transaction amount, source hostname, source port, source account,
 *  destination hostname, destination port, and destination account
 * @param argc number of arguments
*/
int main(int argc, char *argv[]) {
    const string LOG_FILE_NAME = argv[1];
    const string VOTE_REQUEST = "VOTE-REQUEST";
    const string COMMIT_RESPONSE = "VOTE-COMMIT";
    const string ABORT_RESPONSE = "VOTE-ABORT";
    const string GLOBAL_COMMIT = "GLOBAL-COMMIT";
    const string GLOBAL_ABORT = "GLOBAL-ABORT";
    const string ACK = "ACK";

    string source_host = argv[3], dest_host = argv[6];
    int source_port = atoi(argv[4]), dest_port = atoi(argv[7]);
    string transfer = argv[2]; // TODO: handle numbers < 0
    string source_account = argv[5], dest_account = argv[8];
    string source_response, dest_response;
    string message;
    ofstream logfile(LOG_FILE_NAME);

    if (!logfile.is_open())
        throw runtime_error("Failed to open " + LOG_FILE_NAME);

    // create clients
    TCPClient source_client(source_host, source_port);
    TCPClient dest_client(dest_host, dest_port);
    int transaction_id = 1; // TODO: modify
    string tid = to_string(transaction_id);

    // make requests to participants
    logfile << "Sending message '" << VOTE_REQUEST << "' " << source_account << " -" << transfer << " to " 
        << source_host << ":" << source_port << endl;
    source_client.send_request(VOTE_REQUEST + " -" + transfer + " " + source_account);

    logfile << "Sending message '" << VOTE_REQUEST << "' " << dest_account << " " << transfer << " to " 
        << dest_host << ":" << dest_port << endl;
    dest_client.send_request(VOTE_REQUEST + " " + transfer + " " + dest_account);

    source_response = source_client.get_response();
    logfile << "Got " << source_response << " from " << source_host << ":" << source_port << endl;
    dest_response = dest_client.get_response();
    logfile << "Got " << dest_response << " from " << dest_host << ":" << dest_port << endl;

    if (source_response == COMMIT_RESPONSE
        && dest_response == COMMIT_RESPONSE)
        message = GLOBAL_COMMIT;
    else
        message = GLOBAL_ABORT;

    logfile << "Sending message '" << message << " to " 
        << source_host << ":" << source_port << endl;
    source_client.send_request(message + ' ' + source_account);

    logfile << "Sending message '" << message << " to " 
        << dest_host << ":" << dest_port << endl;
    dest_client.send_request(message + ' ' + dest_account);

    source_response = source_client.get_response();
    dest_response = dest_client.get_response();


    if (source_response == ACK && dest_response == ACK) {
        if (message == GLOBAL_COMMIT)
            logfile << "Transaction committed." << endl;
        else
            logfile << "Transaction aborted." << endl;
    }
    else
        return EXIT_FAILURE; // TODO: probably change this behavior
    
    /*
        if both VOTE-COMMIT
            respond GLOBAL-COMMIT
        if any VOTE-ABORT
            respond GLOBAL-ABORT (except to aborting participant)
        
        wait for ACKS
        if ACKS received
            log transaction committed
        else
            ask for another acknowledgement?
    */

    // exit program
    // Extra Credit: start new thread on request?

    logfile.close();

    displayFileContents(cout, LOG_FILE_NAME);
}

void displayFileContents(ostream &output, string filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line))
        output << line << endl;
}