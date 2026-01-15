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

# define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif