/* Program to convert a generic pcap file to a file with only ipv4 udp packets
 *
 * Author: Antony Gillette
 * Date: 02/2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <netinet/if_ether.h>

#include <pcap.h>

struct UDP_hdr {
    unsigned short src_port; /* source port */
    unsigned short dst_port; /* destination port */
    unsigned short udp_len; /* datagram length */
    unsigned short udp_checksum; /* datagram checksum */
};

void dump_ipv4_packet(const unsigned char *packet, struct timeval ts,
            unsigned int capture_len, FILE *fp)
{
    struct ip *ip;
    struct UDP_hdr *udp;
    unsigned int ip_hdr_len;
    unsigned int ipv4_length;
    const unsigned char *place_ipv4;

    if(capture_len < sizeof(struct ether_header))
        return;

    /* skip Ethernet header */
    packet += sizeof(struct ether_header);
    capture_len -= sizeof(struct ether_header);

    /* save the place of the start for the ipv4 packet */
    place_ipv4 = packet;
    ipv4_length = capture_len;

    /* ip header not large enough */
    if(capture_len < sizeof(struct ip))
        return;

    ip = (struct ip*) packet;
    ip_hdr_len = ip->ip_hl * 4;

    /* ip header not as large as field says it is */
    if(capture_len < ip_hdr_len)
        return;

    /* not a UDP packet */
    if(ip->ip_p != IPPROTO_UDP)
        return;

    /* dump rest of packet to file */
    fwrite(packet, capture_len, 1, fp);
}

int main(int argc, char *argv[])
{
    pcap_t *pcap;
    const unsigned char *packet;
    const char *pcap_filename;
    const char *ip_filename;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr header;
    FILE *fp;

    if(argc != 3)
    {
        fprintf(stderr, "Need exactly two arguments: pcap dump filename(.pcap) and desired output IPv4 packets filename\n");
        exit(1);
    }

    pcap_filename = argv[1];
    ip_filename = argv[2];

    pcap = pcap_open_offline(pcap_filename, errbuf);
    if(pcap == NULL)
    {
        fprintf(stderr, "error reading pcap file: %s\n", errbuf);
        exit(1);
    }

    fp = fopen(ip_filename, "ab");
    while((packet = pcap_next(pcap, &header)) != NULL)
        dump_ipv4_packet(packet, header.ts, header.caplen, fp);

    fclose(fp);
    return 0;
}
