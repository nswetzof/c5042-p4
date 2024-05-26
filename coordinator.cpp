#include <iostream>
#include "TCPClient.h"

using namespace std;

/**
 * @param argv string array with elements representing 
 *  log file, 
*/
int main(int argc, char *argv[]) {
    string source_host = argv[3], dest_host = argv[6];
    int source_port = atoi(argv[4]), dest_port = atoi(argv[7]);
    string transfer = argv[2]; // TODO: handle numbers < 0
    string source_account = argv[5], dest_account = argv[8];
    string source_response, dest_response;

    // create clients
    TCPClient source_client(source_host, source_port);
    TCPClient dest_client(dest_host, dest_port);

    // make requests to participants
    cout << "Sending message 'VOTE-REQUEST' " + source_account << " -" << transfer << " to " 
        << source_host << ":" << source_port << endl;
    source_client.send_request("VOTE-REQUEST -" + transfer + " " + source_account);

    cout << "Sending message 'VOTE-REQUEST' " + dest_account << " " << transfer << " to " 
        << dest_host << ":" << dest_port << endl;
    dest_client.send_request("VOTE-REQUEST " + transfer + " " + dest_account);

    source_response = source_client.get_response();
    dest_response = dest_client.get_response();

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
}