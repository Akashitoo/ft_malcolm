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
			printf("trame recu \n");
			printf("%d%d\n", buffer[12], buffer[13]);
			return 1;
		}
	}

	(void)argv;
}
