#ifndef FT_MALCOLM_H
# define FT_MALCOLM_H

# include <sys/socket.h>
# include <net/ethernet.h>
# include <signal.h> 
# include <stdio.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <sys/types.h>
# include <errno.h>
# include <string.h>
# include <linux/if_packet.h>
# include <linux/if_ether.h>
# include <ifaddrs.h>
# include <netinet/in.h>
# include <net/if.h>
# include <sys/time.h>
# include "libft.h"


char *if_idxtoname(int index);
int hex_to_dec(char c);
void free_tab(char **tab);
int convert_mac(char *mac_str, unsigned char *mac_tab);
int check_args(int argc, char **argv, unsigned char *source_mac, unsigned char *target_mac, struct in_addr *target_ip, struct in_addr *source_ip);
void fill_arp_reply(unsigned char *buffer, unsigned char *target_mac, unsigned char *source_mac, struct in_addr target_ip, struct in_addr source_ip);
void receive_arp(struct sockaddr_ll addr, unsigned char *buffer);

#endif