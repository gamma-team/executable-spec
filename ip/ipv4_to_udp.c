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
    int ip_header[20];
    int current_byte;
    int packet_length;
    int i;
    int checksum;

    if(argc != 3)
    {
        fprintf(stderr, "Need exactly two arguments: ipv4 packets filename and desired udp packets filename\n");
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
        /* Check first byte to see if the packet is valid */
        if((current_byte >> 4) & 0x0F != 4) {
            printf("Corrupted ipv4 packet encountered, exiting\n");
            break;
        }
        ip_hdr_len = (int)(current_byte & 0x0F)*4;
        /* if(ip_hdr_len < 20 || ip_hdr_len > 60) { */
        if(ip_hdr_len != 20) {
            printf("IPv4 header length either too short or too long, exiting\n");
            break;
        }

        /* Store rest of header in array */
        ip_header[0] = current_byte;
        for(i=1;i<20;i++)
            ip_header[i] = fgetc(rp);
        if(ip_header[19] == EOF) {
            printf("IPv4 header ended early, exiting\n");
            break;
        }

        /* Write source and destination addresses to file */
        for(i=12; i<20; i++)
            fputc(ip_header[i], wp);

        /* Write protocol type to file */
        fputc(ip_header[9], wp);

        /* Verify checksum */
        checksum = 0;
        for(i=0;i<10;i++) {
            checksum += ip_header[2*i]<<8;
            checksum += ip_header[2*i+1];
        }
        while(checksum > 0xFFFF)
            checksum = (checksum>>16) + (checksum&0xFFFF);
        if(checksum != 0xFFFF) {
            printf("Invalid checksum encountered, exiting\n");
            break;
        }

        /* Calculate packet length then copy rest of packet based on it */
        packet_length = (ip_header[2]<<8)|ip_header[3];
        for(i=0; i<packet_length-ip_hdr_len; i++)
            fputc(fgetc(rp), wp);
    }

    fclose(rp);
    fclose(wp);
    return 0;
}
