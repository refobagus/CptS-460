#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h> 

#define ETHER_TYPE ETHERTYPE_IP

#define BUF_SIZ		65536
#define SEND 0
#define RECV 1

#define MY_DEST_MAC0	0x00
#define MY_DEST_MAC1	0x00
#define MY_DEST_MAC2	0x00
#define MY_DEST_MAC3	0x00
#define MY_DEST_MAC4	0x00
#define MY_DEST_MAC5	0x00

#define DEST_MAC0	0x00
#define DEST_MAC1	0x00
#define DEST_MAC2	0x00
#define DEST_MAC3	0x00
#define DEST_MAC4	0x00
#define DEST_MAC5	0x00


int main(int argc, char *argv[])
{
	int mode;
	int sockfd;
	int ret;
	int i;
	int sendLen = 0;
	char hw_addr[6];
	char interfaceName[IFNAMSIZ];
	char sender[INET6_ADDRSTRLEN];
	char buf[BUF_SIZ];
	struct ifreq if_idx;
	struct ifreq if_mac;
	struct ifreq if_ip;
	struct sockaddr_ll sk_addr;
	struct sockaddr_storage their_addr;
	struct ifreq ifopts;
	ssize_t recvLen;
	uint8_t buf2[BUF_SIZ];
	memset(buf, 0, BUF_SIZ);

	int correct=0;
	if (argc > 1){
		if(strncmp(argv[1],"Send", 4)==0){
			if (argc == 5){
				mode=SEND; 
				//sscanf(argv[3], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &hw_addr[0], &hw_addr[1], &hw_addr[2], &hw_addr[3], &hw_addr[4], &hw_addr[5]);
				//strncpy(buf, argv[4], BUF_SIZ);
				correct=1;
				//printf("  buf: %s\n", buf);
			}
		}
		else if(strncmp(argv[1],"Recv", 4)==0){
			if (argc == 3){
				mode=RECV;
				correct=1;
			}
		}
		strncpy(interfaceName, argv[2], IFNAMSIZ);
	 }
	 if(!correct){
		fprintf(stderr, "./455_proj2 Send <InterfaceName>  <DestHWAddr> <Message>\n");
		fprintf(stderr, "./455_proj2 Recv <InterfaceName>\n");
		exit(1);
	 }
	
	

	if(mode == SEND) //**************SENDING********************
	{	
		struct ether_header *eh = (struct ether_header *) buf;
		struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
		if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0){ //open socket to send on
			perror("socket() failed");
		}
		//assign socket to specified interface
		memset(&if_idx, 0, sizeof(struct ifreq));
		strncpy(if_idx.ifr_name, interfaceName, IFNAMSIZ-1);
		if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) //assign socket to device
			perror("SIOCGIFINDEX");
		
		//mac address
		memset(&if_mac, 0, sizeof(struct ifreq));
		strncpy(if_mac.ifr_name, interfaceName, IFNAMSIZ-1);
		if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0) //assign  MAC address
			perror("SIOCGIFHWADDR");

		
		memset(buf, 0, BUF_SIZ);
		
		eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
		eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
		eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
		eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
		eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
		eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
		eh->ether_dhost[0] = MY_DEST_MAC0;
		eh->ether_dhost[1] = MY_DEST_MAC1;
		eh->ether_dhost[2] = MY_DEST_MAC2;
		eh->ether_dhost[3] = MY_DEST_MAC3;
		eh->ether_dhost[4] = MY_DEST_MAC4;
		eh->ether_dhost[5] = MY_DEST_MAC5;

		eh->ether_type = htons(ETH_P_IP);
		sendLen += sizeof(struct ether_header);

		buf[sendLen++] = 0xde;
		buf[sendLen++] = 0xad;
		buf[sendLen++] = 0xbe;
		buf[sendLen++] = 0xef;

		sk_addr.sll_ifindex = if_idx.ifr_ifindex;

		sk_addr.sll_halen = ETH_ALEN;
		sk_addr.sll_addr[0] = MY_DEST_MAC0;
		sk_addr.sll_addr[1] = MY_DEST_MAC1;
		sk_addr.sll_addr[2] = MY_DEST_MAC2;
		sk_addr.sll_addr[3] = MY_DEST_MAC3;
		sk_addr.sll_addr[4] = MY_DEST_MAC4;
		sk_addr.sll_addr[5] = MY_DEST_MAC5;

		//sending packet
		if (sendto(sockfd, buf, sendLen, 0, (struct sockaddr*)&sk_addr, sizeof(struct sockaddr_ll)) < 0)
			printf("Send failed\n");
	}
	else if (mode == RECV) //*************RECIEVING********************
	{
		struct ether_header *eh = (struct ether_header *) buf2;
		struct iphdr *iph = (struct iphdr *) (buf2 + sizeof(struct ether_header));
		struct udphdr *udph = (struct udphdr *) (buf2 + sizeof(struct iphdr) + sizeof(struct ether_header));

		memset(&if_ip, 0, sizeof(struct ifreq));

		if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) < 0){ //open socket to recieve on
			perror("socket() failed");
		}


		strncpy(ifopts.ifr_name, interfaceName, IFNAMSIZ-1);

		ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
		ifopts.ifr_flags |= IFF_PROMISC;
		ioctl(sockfd, SIOCSIFFLAGS, &ifopts);


	repeat:	printf("listener: Waiting to recvfrom...\n");
		recvLen = recvfrom(sockfd, buf2, BUF_SIZ, 0, NULL, NULL);
		printf("listener: got packet %lu bytes\n", recvLen);


		if (eh->ether_dhost[0] == DEST_MAC0 &&
				eh->ether_dhost[1] == DEST_MAC1 &&
				eh->ether_dhost[2] == DEST_MAC2 &&
				eh->ether_dhost[3] == DEST_MAC3 &&
				eh->ether_dhost[4] == DEST_MAC4 &&
				eh->ether_dhost[5] == DEST_MAC5) {
			printf("Correct destination MAC address\n");
		} else {
			printf("Wrong destination MAC: %x:%x:%x:%x:%x:%x\n",
							eh->ether_dhost[0],
							eh->ether_dhost[1],
							eh->ether_dhost[2],
							eh->ether_dhost[3],
							eh->ether_dhost[4],
							eh->ether_dhost[5]);
			ret = -1;
			goto done;
		}

		((struct sockaddr_in *)&their_addr)->sin_addr.s_addr = iph->saddr;
		inet_ntop(AF_INET, &((struct sockaddr_in*)&their_addr)->sin_addr, sender, sizeof sender);


		strncpy(if_ip.ifr_name, interfaceName, IFNAMSIZ-1);
		if (ioctl(sockfd, SIOCGIFADDR, &if_ip) >= 0) { 
			printf("Source IP: %s\n My IP: %s\n", sender, 
					inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));

			if (strcmp(sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr)) == 0)	{
				printf("Not showing...\n");
				ret = -1;
				goto done;
			}
		}
		
		ret = ntohs(udph->len) - sizeof(struct udphdr);

		printf("\tData:");
		for (i=0; i<recvLen; i++) printf("%02x:", buf2[i]);
		printf("\n");

	done:	goto repeat;

		close(sockfd);
		return ret;
	}

	return 0;
}

