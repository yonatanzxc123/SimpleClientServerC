#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string> 
#include <string.h>

using namespace std;

#define TIME_PORT	27015
#define NUM_REQUESTS 100

bool Send(SOCKET connSocket, sockaddr_in server, char* sendBuff)
{
	int bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return false;
	}
	cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	return true;
}



bool Receive(SOCKET connSocket, char* recvBuff)
{
	int bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return false;
	}

	recvBuff[bytesRecv] = '\0'; // null-terminate
	cout << "Time Client: Received: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
	return true;
}



bool SendAndReceive(SOCKET connSocket, sockaddr_in server, char* sendBuff, char* recvBuff)
{
	// Calls the two helper functions
	return Send(connSocket, server, sendBuff) && Receive(connSocket, recvBuff);
}



void main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.

	bool flag = false;
	int choice = 0;	
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255] = {};
	char recvBuff[255] = {};

	while (flag == false)
	{
		
		cout << "\nChoose one of the following Action \n" << "Press the following:\n 1 for GetTime \n 2 for GetTimeWithoutDate\n 3 for GetTimeSinceEpoch\n 4 for GetClientToServerDelayEstimation\n 5 for MeasureRTT\n 6 for GetTimeWithoutDateOrSeconds\n 7 for GetYear\n 8 for GetMonthAndDay\n 9 for GetSecondsSinceBeginingOfMonth\n 10 for GetWeekOfYear\n 11 for GetDayLightSavings\n 12 for GetTimeWithoutDateInCity\n 13 for MeasureTimeLap\n 14 for EXIT\n";
		cin >> choice;

		
		// Check if the input is valid
		if (choice < 1 || choice > 14)
		{
			cout << "Invalid choice. Try again.\n";
			cin.clear();              
			cin.ignore(10000, '\n');  
			continue;              
		}
		
		switch (choice)
		{
		case 1:
		{
			// GetTime
			sendBuff[0] = '1';
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;

			break;
		}
		case 2:
		{
			// GetTimeWithoutDate
			sendBuff[0] = '2';
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 3:
		{
			// GetTimeSinceEpoch
			sendBuff[0] = '3';
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 4:
		{
			// GetClientToServerDelayEstimation


			DWORD clientSendTimes[NUM_REQUESTS] = {};
			DWORD serverTicks[NUM_REQUESTS] = {};
		
			double sumDiff = 0;
			double avg = 0;

			for (int i = 0; i < NUM_REQUESTS; i++)
			{

				sendBuff[0] = '4';
				Send(connSocket, server, sendBuff);

			}
			for (int i = 0; i < NUM_REQUESTS; i++)
			{
				Receive(connSocket, recvBuff);

				serverTicks[i] = strtoul(recvBuff, NULL, 10);
			}

			for (int i = 1; i < NUM_REQUESTS; i++)
			{
				sumDiff += (serverTicks[i] - serverTicks[i - 1]);
				
			}

			 avg = sumDiff/ (double)(NUM_REQUESTS -1);

			cout << "Estimated average client-to-server delay: " << avg << " ms (server tick count average)." << endl;

			break;
		}
		case 5:
		{
			// MeasureRTT

			DWORD clientSendTimes = 0;
			DWORD delays[NUM_REQUESTS] = {};
			double sumDiff = 0;
			double avg = 0;


			for (int i = 0; i < NUM_REQUESTS; i++)
			{
				clientSendTimes = GetTickCount();
				sendBuff[0] = '5';
				SendAndReceive(connSocket, server, sendBuff, recvBuff);

				delays[i] = GetTickCount() - clientSendTimes;
			//	cout << "Delay: " << delays[i] << " ms" << endl;
			}
			for (int i = 0; i < NUM_REQUESTS; i++)
			{

				sumDiff += delays[i];
			}
			avg = sumDiff / (double)(NUM_REQUESTS);
			cout << "Estimated average RTT: " << avg << " ms (measured on client side)." << endl;
			break;
		}
		case 6:
		{
			// GetTimeWithoutDateOrSeconds
			sendBuff[0] = '6';
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 7:
		{
			// GetYear
			sendBuff[0] = '7';
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 8:
		{
			// GetMonthAndDay
			sendBuff[0] = '8';
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 9:
		{
			// GetSecondsSinceBeginingOfMonth
			sendBuff[0] = '9';
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 10:
		{
			// GetWeekOfYear
			strcpy(sendBuff, "10");
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 11:
		{
			// GetDayLightSavings
			strcpy(sendBuff, "11");
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 12:
		{
			// GetTimeWithoutDateInCity
			cout << "Choose a city:\n";
			cout << "1 - Doha\n";
			cout << "2 - Prague\n";
			cout << "3 - New York\n";
			cout << "4 - Berlin\n";
			cout << "5 - Other (enter manually)\n";
			int choice;
			cin >> choice;

			string city;

			switch (choice)
			{
			case 1: city = "Doha";
				break;
			case 2: city = "Prague";
				break;
			case 3: city = "New York";
				break;
			case 4: city = "Berlin";
				break;
			case 5:
				cout << "Enter city name: ";
				cin.ignore();  
				getline(cin, city);
				break;
			default:
				cout << "Invalid option.\n";
				break;
			}

			if (city.empty())
				break;

			string message = "12" + city;
			strcpy(sendBuff, message.c_str());

			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;

			break;
		}
		case 13:
		{
			// MeasureTimeLap
			strcpy(sendBuff, "13");
			if (!SendAndReceive(connSocket, server, sendBuff, recvBuff))
				return;
			break;
		}
		case 14:
		{
			// EXIT
			flag = true;
			break;
		}
		}



		



	}
	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);

	system("pause");
}