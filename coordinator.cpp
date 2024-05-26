#include <fstream>
#include <iostream>
#include "TCPClient.h"

using namespace std;

void displayFileContents(ostream &output, string filename);

/**
 * @param argv string array with elements representing 
 *  log file, 
*/
int main(int argc, char *argv[]) {
    const string LOG_FILE_NAME = "log.txt";
    string source_host = argv[3], dest_host = argv[6];
    int source_port = atoi(argv[4]), dest_port = atoi(argv[7]);
    string transfer = argv[2]; // TODO: handle numbers < 0
    string source_account = argv[5], dest_account = argv[8];
    string source_response, dest_response;
    fstream logfile(LOG_FILE_NAME, fstream::in | fstream::out);

    if (!logfile.is_open())
        throw runtime_error("Failed to open " + LOG_FILE_NAME);

    // create clients
    TCPClient source_client(source_host, source_port);
    TCPClient dest_client(dest_host, dest_port);

    // make requests to participants
    logfile << "Sending message 'VOTE-REQUEST' " + source_account << " -" << transfer << " to " 
        << source_host << ":" << source_port << endl;
    source_client.send_request("VOTE-REQUEST -" + transfer + " " + source_account);

    logfile << "Sending message 'VOTE-REQUEST' " + dest_account << " " << transfer << " to " 
        << dest_host << ":" << dest_port << endl;
    dest_client.send_request("VOTE-REQUEST " + transfer + " " + dest_account);

    source_response = source_client.get_response();
    logfile << "Got " << source_response << " from " << source_host << ":" << source_port << endl;
    dest_response = dest_client.get_response();
    logfile << "Got " << dest_response << " from " << dest_host << ":" << dest_port << endl;

    // handle responses (block until receiving both)
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
    string line;

    logfile.close();

    displayFileContents(cout, LOG_FILE_NAME);
}

void displayFileContents(ostream &output, string filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line))
        output << line << endl;
}