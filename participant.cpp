#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "2PC_Participant.h"

using namespace std;

struct account {
    double balance;
    string num;
};


/**
 * @param argv string array with elements representing 
 *  port number, accounts file, and log file respectively
*/
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
    cout << argc << " arguments" << endl;
    for (int i = 0; i < argc; i++)
        cout << "argument " << i << ": " << argv[i] << ", ";
    cout << endl;

    Participant participant(serve_port, argv[2], argv[3]);
    ostringstream note;
    note << "Transaction service on port " << serve_port << " (Ctrl-C to stop)" << endl;
    participant.log(note.str());
}