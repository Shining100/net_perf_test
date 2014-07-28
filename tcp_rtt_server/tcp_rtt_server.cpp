#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int main(void)
{
	unsigned short port = 10000;

	int sl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sl)
	{
		std::cout << "socket failed with error: " << errno;
		return errno;
	}

	sockaddr_in local = {0};
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);
	int ret = bind(sl, (sockaddr*)&local, sizeof(sockaddr_in));
	if (-1 == ret)
	{
		std::cout << "bind failed with error: " << errno;
		return errno;
	}
	ret = listen(sl, SOMAXCONN);
	if (-1 == ret)
	{
		std::cout << "listen failed with error: " << errno;
		return errno;
	}

	for (;;)
	{
		int sa = accept(sl, NULL, NULL);
		if (-1 == sa)
		{
			std::cout << "accept failed with error: " << errno;
			return errno;
		}

		char c;
		for (;;)
		{
			ret = recv(sa, &c, 1, 0);
			if (-1 == ret)
			{
				std::cout << "recv failed with error: " << errno;
				return errno;
			}
			if (0 == ret) break;
//			int val = 1;
//			ret = setsockopt(sa, IPPROTO_TCP, TCP_QUICKACK,  &val, sizeof(int));
//			if (-1 == ret)
//			{
//				std::cout << "setsockopt failed with error: " << errno;
//				return errno;
//			}
		}
		close(sa);
	}

	close(sl);
	return 0;
}
