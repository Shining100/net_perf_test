#define WIN32_LEAN_AND_MEAN

#include <ctime>
#include <string>
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <lib/getopt/getopt.h>

#pragma comment (lib, "Ws2_32.lib")

struct Config
{
	std::string remoteIp;
	unsigned short remotePort;
	bool tcpNoDelay;
};

int parseArgs(int argc, char *argv[], Config &config);
int sendPackets(const Config &config, int packets);

int main(int argc, char *argv[])
{
	std::string input;

	Config config;
	int ret = parseArgs(argc, argv, config);
	if (0 != ret)
	{
		std::cout << "parseArgs failed with error: " << ret;
		goto err;
	}

	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (0 != ret)
	{
		std::cout << "WSAStartup failed with error: " << ret;
		goto err;
	}

	std::cout << "Please input send packets or q to exit: ";
	std::cin >> input;
	while ("q" != input)
	{
		int packets = atoi(input.c_str());
		ret = sendPackets(config, packets);
		if (0 != ret)
		{
			ret = WSAGetLastError();
			std::cout << "Error exit: " << ret;
			break;
		}
		std::cout << "Please input send packets or q to exit: ";
		std::cin >> input;
	}

	WSACleanup();
err:
	getchar();
	return ret;
}

int parseArgs(int argc, char *argv[], Config &config)
{
	config.remoteIp = "127.0.0.1";
	config.remotePort = 10000;
	config.tcpNoDelay = false;
	const option opts[] =
	{
		{"remote-ip", required_argument, NULL, 0},
		{"tcp-no-delay", no_argument, NULL, 0},
	};

	int opt = -1;
	int index = -1;
	do 
	{
		opt = getopt_long(argc, argv, "", opts, &index);
		switch (index)
		{
		case 0:
			config.remoteIp = optarg;
			break;
		case 1:
			config.tcpNoDelay = true;
			break;
		default:
			break;
		}
		index = -1;
	} while (-1 != opt);

	return 0;
}

int sendPackets(const Config &config, int packets)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == s) return -1;
	int ret;
	if (config.tcpNoDelay)
	{
		char val = 1;
		ret = setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &val, 1);
		if (0 != ret) goto err;
	}
	struct linger l;
	l.l_onoff = 1;
	l.l_linger = 3;
	ret = setsockopt(s, SOL_SOCKET, SO_LINGER, (const char*)&l, sizeof(linger));
	if (0 != ret) goto err;

	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_addr.s_addr = inet_addr(config.remoteIp.c_str());
	remoteAddr.sin_port = htons(config.remotePort);
	ret = connect(s, (sockaddr*)&remoteAddr, sizeof(sockaddr_in));
	if (0 != ret) goto err;

	clock_t begin = clock();
	char c = 0;
	for (int i = 0; i < packets; ++i)
	{
		ret = send(s, &c, 1, 0);
		if (SOCKET_ERROR == ret) goto err;
	}

	shutdown(s, SD_BOTH);
err:
	closesocket(s);
	if (1 == ret)
	{
		clock_t end = clock();
		std::cout << "send packets cost " << end - begin << " clock" << std::endl;
		ret = 0;
	}

	return ret;
}
