/**
 * @file participant.cpp
 * @author Nathan Swetzof
 * Implements behavior for a participant in a 2PC transaction, using the 
 *  Participant class.
*/

#include <fstream>
#include <sstream>
#include "2PC_Participant.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cerr << "Error: not enough arguments" << endl;
        return EXIT_FAILURE;
    }
    int serve_port = atoi(argv[1]);
    if (serve_port < 1 || serve_port >= 1 << 16) {
        cerr << "Invalid serve port " << serve_port << endl;
        return EXIT_FAILURE;
    }
    
    Participant participant(serve_port, argv[2], argv[3]);
    ostringstream note;

    note << "Transaction service on port " << serve_port << " (Ctrl-C to stop)";
    participant.log(note.str());
    cout << note.str() << endl;

    // while loop allows for additional connections after first client terminates
    while (true)
        participant.serve();

    // print contents of log file to console
    ifstream logfile(argv[3]);
    string line;
    while (getline(logfile, line))
        cout << line << endl;

    return EXIT_SUCCESS;
}