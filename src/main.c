#include "ft_malcolm.h"

int g_stop = 0;

void handler(int sig)
{
	printf("\nSignal reçu:%d\n", sig);
	g_stop = 1;
}

char *if_idxtoname(int index)
{

	struct ifaddrs *ifaddr, *ifbase;
	char *name;

	if (getifaddrs(&ifaddr) == -1)
	{
		printf("getifaddrs : %s\n", strerror(errno));
		return NULL;
	}

	ifbase = ifaddr;

	while (ifaddr)
	{
		if (if_nametoindex(ifaddr->ifa_name) == (unsigned int)index)
		{
			name = ft_strdup(ifaddr->ifa_name);
			freeifaddrs(ifbase);
			return (name);
		}
		ifaddr = ifaddr->ifa_next;
	}
	freeifaddrs(ifbase);
	return NULL;
}

int main(int argc, char **argv)
{
	signal(SIGINT, handler);

	unsigned char buffer[1518];
	struct sockaddr_ll addr;
	socklen_t addr_len = sizeof(addr);

	if (argc != 5)
		return 1;

	if (getuid() != 0)
	{
    	fprintf(stderr, "Vous devez être root pour exécuter ce programme\n");
    return 1;
	}
	
	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0)
		printf("socket : %s\n", strerror(errno));
	while (!g_stop)
	{
		int b = recvfrom(sock, buffer, 1518, 0, (struct sockaddr *)&addr, &addr_len);
		if (b > 0)
		{
			if (buffer[12] == 8 && buffer[13] == 6)
			{
				printf("Found available interface: %s", if_idxtoname(addr.sll_ifindex));
				printf("An ARP request has been broadcast.\n");
				printf("	mac address of request: %02X:%02X:%02X:%02X:%02X:%02X\n", buffer[6], buffer[7],buffer[8],buffer[9],buffer[10],buffer[11]);
				printf("	IP address of request: %d:%d:%d:%d\n", buffer[28], buffer[29], buffer[30],  buffer[31]);
			}
			return 0;
		}
	}

	(void)argv;
}
