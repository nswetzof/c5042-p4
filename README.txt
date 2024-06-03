This program implements a 2PC transaction using a coordinator and multiple participants to transfer funds between accounts.  The program takes arguments from the command line to initialize participants and to set up the coordinator to transfer the desired funds from each account.

To set up a participant, the port number, account file name, and log file name needs to be specified as follows:
./participant [port number] [account file] [log file]

To set up the coordinator, the log file name and the file name of a file listing the desired transactions needs to be specified as follows:
./coordinator [log file] [transaction file]

The transaction file must be formatted such that one transaction is on each line.  Each transaction consists of the amount, host name and port number of the participant holding the account, and the desired account number from which to initiate the transfer (negative values for withdrawals and positive values for deposits) - all separated by spaces as follows:
[amount] [host name] [port number] [account number]

For example, to withdraw $1.00 from account number 0123-45.67 on "localhost":10101, the line in the transaction file would read:
-1.0 localhost 10101 0123-45.67

Extra Credit:
a. Handling any number of participants
Multiple participants (> 2) are supported.  Every transaction in the transaction file will be executed provided each transaction is on a unique host:port pair and the sum of all transactions adds up to zero (no money is unaccounted for).