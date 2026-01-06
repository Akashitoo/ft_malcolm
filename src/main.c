#include "ft_malcolm.h"

int g_stop = 0;

void handler(int sig)
{
	printf("\nSignal reçu:%d\n", sig);
	g_stop = 1;
}

int main(int argc, char **argv)
{
	signal(SIGINT, handler);

	unsigned char buffer[1518];

	if (argc != 5)
		return 1;
	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0)
		printf("erreur\n");
	while (!g_stop)
	{
		int b = recvfrom(sock, buffer, 1518, 0, NULL, NULL);
		if (b > 0)
		{
			if (buffer[12] == 8 && buffer[13] == 6)
			{
				printf("An ARP request has been broadcast.\n");
				printf("	mac address of request: %X:%X:%X:%X:%X:%X\n", buffer[6], buffer[7],buffer[8],buffer[9],buffer[10],buffer[11]);
				printf("	IP address of request: %d:%d:%d:%d\n", buffer[28], buffer[29], buffer[30],  buffer[31]);
			}
			return 1;
		}
	}

	(void)argv;
}
