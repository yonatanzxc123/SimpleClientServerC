#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <time.h>

#define TIME_PORT	27015

void main()
{
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	bool sendBuffPrepared = false;


	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		sendBuffPrepared = false;

		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		int command = atoi(recvBuff);

		cout << "Time Server: Received command: " << command << endl;

		if (command < 1 || command > 13)
		{
			strcpy(sendBuff, "Invalid request. Command must be an int between 1 and 13.");
			sendBuffPrepared = true;
		}
		else if (command == 1)
		{
			//GetTime

			time_t timer;
			time(&timer);
			// Parse the current time to printable string.
			strcpy(sendBuff, ctime(&timer));
			sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
			sendBuffPrepared = true;

		}
		else if (command == 2)
		{
			// GetTimeWithoutDate

			time_t timer;
			time(&timer);
			tm* timeInfo = localtime(&timer);
			strftime(sendBuff, sizeof(sendBuff), "%H:%M:%S", timeInfo);
			sendBuffPrepared = true;


		}
		else if (command == 3)
		{
			//GetTimeSinceEpoch

			time_t timer;
			time(&timer);
			sprintf(sendBuff, "%ld", timer);
			sendBuffPrepared = true;

		}
		else if (command == 4)
		{
			// GetClientToServerDelayEstimation

			DWORD tick = GetTickCount(); //returns milliseconds since system start
			sprintf(sendBuff, "%lu", tick); // convert to string and sending it back
			sendBuffPrepared = true;
		}
		else if (command == 5)
		{
			// GetServerToClientDelayEstimation

			DWORD tick = GetTickCount(); //returns milliseconds since system start
			sprintf(sendBuff, "%lu", tick); // convert to string and sending it back
			sendBuffPrepared = true;

		}
		else if (command == 6)
		{
			// GetTimeWithoutDateOrSeconds

			time_t timer;
			time(&timer);
			tm* timeInfo = localtime(&timer);
			strftime(sendBuff, sizeof(sendBuff), "%H:%M", timeInfo);
			sendBuffPrepared = true;

		}
		else if (command == 7)
		{
			// GetYear

			time_t timer;
			time(&timer);
			tm* timeInfo = localtime(&timer);
			strftime(sendBuff, sizeof(sendBuff), "%Y", timeInfo);  // %Y = 4-digit year
			sendBuffPrepared = true;

		}
		else if (command == 8)
		{
			// GetMonthAndDay

			time_t timer;
			time(&timer);
			tm* timeInfo = localtime(&timer);
			strftime(sendBuff, sizeof(sendBuff), "%m-%d", timeInfo); // %m = 2-digit month, %d = 2-digit day
			sendBuffPrepared = true;


		}
		else if (command == 9)
		{
			// GetSecondsSinceBeginingOfMonth

			time_t curr;
			time(&curr);
			tm* currentTime = localtime(&curr);

			// Init data for start of the month
			tm beginningOfMonth = *currentTime;
			beginningOfMonth.tm_mday = 1;
			beginningOfMonth.tm_hour = 0;
			beginningOfMonth.tm_min = 0;
			beginningOfMonth.tm_sec = 0;

			time_t startOfMonth = mktime(&beginningOfMonth);

			// Calculate diff in seconds
			double secondsSinceMonthStart = difftime(curr, startOfMonth);
			sprintf(sendBuff, "%.0f", secondsSinceMonthStart);  // %.0f to avoid deci
			sendBuffPrepared = true;

		}
		else if (command == 10)
		{
			//GetWeekOfYear

			time_t timer;
			time(&timer);
			tm* timeInfo = localtime(&timer);

			// %U = week number, when we choose sunday  (iff needed we can use %W for monday)
			strftime(sendBuff, sizeof(sendBuff), "%U", timeInfo);
			sendBuffPrepared = true;

		}

		else if (command == 11)
		{
			// GetDayLightSavings

			time_t timer;
			time(&timer);
			tm* timeInfo = localtime(&timer);

			if (timeInfo->tm_isdst > 0)
				strcpy(sendBuff, "1");
			else if (timeInfo->tm_isdst == 0)
				strcpy(sendBuff, "0");
			else
				strcpy(sendBuff, "Houston we have an error");

			sendBuffPrepared = true;

		}

		else if (command == 12)
		{
			// GetTimeWithoutDateInCity

			string city = string(recvBuff + 2);  // skip '12'

			// Convert to lowercase for matching
			string cityLower = city;
			transform(cityLower.begin(), cityLower.end(), cityLower.begin(), ::tolower);

			time_t rawtime;
			time(&rawtime);
			tm* utcTime = gmtime(&rawtime);

			int offset = 0;
			bool knownCity = true;
			if (cityLower == "doha")
				offset = 3;
			else if (cityLower == "prague")
				offset = 1;
			else if (cityLower == "new york")
				offset = -5;
			else if (cityLower == "berlin")
				offset = 1;
			else
				knownCity = false;

			if (knownCity)
			{
				int hour = (utcTime->tm_hour + offset + 24) % 24;
				sprintf(sendBuff, "%02d:%02d:%02d", hour, utcTime->tm_min, utcTime->tm_sec);
			}
			else
			{
				sprintf(sendBuff, "City '%s' not supported. Current UTC time: %02d:%02d:%02d",
					city.c_str(), utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);
			}

			sendBuffPrepared = true;


		}

		else if (command == 13)
		{
			// MeasureTimeLap

			static bool isMeasuring = false;
			static time_t startTime = 0;
			static time_t expirationTime = 0;


			time_t currentTime;
			time(&currentTime);

			if (!isMeasuring || currentTime > expirationTime)
			{
				// Start new measurement
				isMeasuring = true;
				startTime = currentTime;
				expirationTime = currentTime + 180; // 3 minutes from now

				strcpy(sendBuff, "Measurement of time lap started.");
				sendBuffPrepared = true;
			}
			else
			{
				// Measurement in progress and still valid
				double elapsed = difftime(currentTime, startTime);

				sprintf(sendBuff, "Time Lap lasted %.0f seconds.", elapsed);
				sendBuffPrepared = true;

				// Reset measurement
				isMeasuring = false;
				startTime = 0;
				expirationTime = 0;
			}
		}


		if (sendBuffPrepared)
		{

			// Sends the answer to the client, using the client address gathered
			// by recvfrom. 
			bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
			if (SOCKET_ERROR == bytesSent)
			{
				cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
				closesocket(m_socket);
				WSACleanup();
				return;
			}

			cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
		}
	}
	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}