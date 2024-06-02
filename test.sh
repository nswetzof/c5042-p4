make all
cmd.exe /c start wsl.exe --cd "~/projects/c5042/p2" ./participant 10101 accounts1.txt log1.txt
cmd.exe /c start wsl.exe --cd "~/projects/c5042/p2" ./participant 10102 accounts2.txt log2.txt
sleep 1
./coordinator log.txt 10.00 localhost 10101 8901-23.45 localhost 10102 0123-45.67
