#include "ft_malcolm.h"

char	*if_idxtoname(int index)
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

int	hex_to_dec(char c)
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

void	free_tab(char **tab)
{
	int i = 0;
	while(tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

int	convert_mac(char *mac_str, unsigned char *mac_tab)
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
			if (ft_strlen(mac_split[i]) != 2)
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

int	check_args(int argc, char **argv, unsigned char *source_mac, unsigned char *target_mac, struct in_addr *target_ip, struct in_addr *source_ip)
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

void	fill_arp_reply(unsigned char *buffer, unsigned char *target_mac, unsigned char *source_mac, struct in_addr target_ip, struct in_addr source_ip)
{
	ft_memcpy(buffer, target_mac, 6);
	ft_memcpy(&buffer[6], source_mac, 6);
	ft_memcpy(&buffer[22],&source_mac[0], 6);
	ft_memcpy(&buffer[28], &(source_ip.s_addr), 4);
	ft_memcpy(&buffer[32], &target_mac[0], 6);
	ft_memcpy(&buffer[38], &(target_ip.s_addr), 4);
}

void	receive_arp(struct sockaddr_ll addr, unsigned char *buffer)
{
	char *name = if_idxtoname(addr.sll_ifindex);
	printf("Found available interface: %s\n", name);
	printf("An ARP request has been broadcast.\n");
	printf("	mac address of request: %02X:%02X:%02X:%02X:%02X:%02X\n", buffer[6], buffer[7],buffer[8],buffer[9],buffer[10],buffer[11]);
	printf("	IP address of request: %d:%d:%d:%d\n", buffer[28], buffer[29], buffer[30],  buffer[31]);
	free(name);
}


int compare_ip(unsigned char *buffer, struct in_addr ip)
{
	unsigned char *ip_usc;

	ip_usc = (unsigned char *)ip.s_addr;
	//ip_usc[0] = (ip.s_addr >> 24) &0xFF;
	//ip_usc[1] = (ip.s_addr >> 16) &0xFF;
	//ip_usc[2] = (ip.s_addr >> 8) &0xFF;
	//ip_usc[3] = ip.s_addr &0xFF;
	int flag = 0;
	for (int i=0; i < 4; i++)
	{
		printf("%d : %d\n", buffer[i], ip_usc[i]);
		if (buffer[i] != ip_usc[i])
		{
			flag = 1;
		}

	}
	if (flag)
		return 0;
	return 1;
}