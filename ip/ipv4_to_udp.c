/* Program to convert IPv4 packets to UDP packets
 *
 * Author: Antony Gillette
 * Date: 02/2017
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    const char *ip_filename;
    const char *udp_filename;
    FILE *rp;
    FILE *wp;
    unsigned int ip_hdr_len;
    int protocol;
    int current_byte;
    int packet_length;
    int i;

    if(argc != 3)
    {
        fprintf(stderr, "Need exactly two argument: ipv4 packets filename and desired udp packets filename\n");
        exit(1);
    }

    ip_filename = argv[1];
    udp_filename = argv[2];

    rp = fopen(ip_filename, "rb");
    if(rp == NULL) {
        fprintf(stderr, "error reading ipv4 file\n");
        exit(1);
    }

    wp = fopen(udp_filename, "ab");
    if(wp == NULL) {
        fprintf(stderr, "error opening/creating new udp file\n");
        exit(1);
    }

    while((current_byte = fgetc(rp)) != EOF){
        if((current_byte >> 4) & 0x0F != 4) {
            printf("Corrupted ipv4 packet encountered, exiting\n");
            break;
        }
        ip_hdr_len = (int)(current_byte & 0x0F)*4;
        if(ip_hdr_len < 15 || ip_hdr_len > 60) {
            printf("IPv4 header length either too short or too long, exiting\n");
            break;
        }
        fgetc(rp); /* Skip ToS */
        packet_length = (fgetc(rp)<<8)|fgetc(rp);

        fseek(rp, 5, SEEK_CUR); /* Skip ID/flags/fragments/TTL */
        protocol = fgetc(rp);
        /* TODO: Add IPv4 checksum check (bytes 11/12) */
        fseek(rp, 2, SEEK_CUR); /* Skip checksum */
        /* Write source and destination addresses to file */
        for(i=0; i<8; i++)
            fputc(fgetc(rp), wp);
        /* Write protocol type to file */
        fputc(protocol, wp);
        /* Now at start of UDP header */
        /* TODO: Add UDP total length check (bytes 5/6), checksum check (bytes 7/8) */
        for(i=0; i<packet_length-ip_hdr_len; i++)
            fputc(fgetc(rp), wp);
    }

    fclose(rp);
    fclose(wp);
    return 0;
}
