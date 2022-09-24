#include <iostream>
#include <WS2tcpip.h>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	// Initializing Winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0)
	{
		cerr << "Cannot Initialize Winsock! Quitting" << endl;
		return;
	}
	else
	{
		cout << "WinSock Initialized" << endl;
	}

	// Creating a Socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET)
	{
		cerr << "Cannot create Socket! Quitting" << endl;
		return;
	}

	else
	{
		cout << "Socket created" << endl;
	}

	// Binding the IP Address and Port to a Socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Telling Winsock the Socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;
		
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the master file_set
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				string welcomeMessage = "--------------------Welcome to the Awesome Chat Server!--------------------\r\n";
				send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);
			}

			else
			{
				char buf [4096];
				ZeroMemory(buf, 4096);

				// Receive messsage
				int bytesIn = recv(sock, buf, 4096, 0);

				if (bytesIn <= 0)
				{
					closesocket(sock);
					FD_CLR(sock, &master);
				}

				else
				{
					// Send message to other clients, and definiately NOT the listening socket
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];

						if (outSock != listening && outSock != sock)
						{
							ostringstream ss;
							
							ss << "\nClient #" << sock << ": " << buf << "\r\n";

							string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}

				}
			}
		}
	}

	// Cleanup Winsock
	WSACleanup();
}


