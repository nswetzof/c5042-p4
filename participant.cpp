#include <fstream>
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
    Participant bank1(10101, "accounts.txt", "log.txt");
}