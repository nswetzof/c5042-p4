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

    // create clients
    TCPClient source_client(source_host, source_port);
    TCPClient dest_client(dest_host, dest_port);

    // make requests to participants
    source_client.send_request() // TODO: continue

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