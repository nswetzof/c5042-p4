#include <iostream>
#include "TCPClient.h"

using namespace std;

/**
 * @param argv string array with elements representing 
 *  log file, 
*/
int main(int argc, char *argv[]) {
    // create client

    // make requests to participants

    // handle responses (block until receiving both)
    /*
        if both VOTE-COMMIT
            respond GLOBAL-COMMIT
        if any VOTE-ABORT
            respond GLOBAL-ABORT
        
        wait for ACKS
        if ACKS received
            log transaction committed
        else
            ask for another acknowledgement?
    */
}