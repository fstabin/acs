#include "..\acs\include\sock.h"

#include <string>
#include <iostream>

int sock_sample() {
	using namespace acs::sock;
	using namespace std;

	SockModule host = SockModule();
	ServerSocket server;
	server.listen();
	ClientSocket client("192.168.0.5");
	ServerClientSocket sc = server.accept();
	string sCtos, sStoC;
	cout << "client send to server = ";
	cin >> sCtos;
	cout << "server send to client = ";
	cin >> sStoC;
	cout << "connecting..." << endl;
	cout << "client.send = " << client.send(sCtos.data(), sCtos.size()) << endl;
	cout << "sc.send = " <<  sc.send(sStoC.data(), sStoC.size()) << endl;

	client.shutdown();
	sc.shutdown();

	char strBuf[21] = {0};

	int read;
	cout << "server receive from client = ";
	while ((read = sc.recv(strBuf, 20)) > 0) {
		strBuf[read] = 0;
		cout << strBuf, read;
	}
	cout << endl;
	cout << "client receive from server  = ";
	while ((read = client.recv(strBuf, 20)) > 0) {
		strBuf[read] = 0;
		cout << strBuf;
	}
	cout << endl;

	return 0;
}