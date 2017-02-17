/* Program to convert UDP packets to IPv4 packets
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
    int current_byte;
    int i;
    int packet_length;
    int apuh[16]; /* Addresses plus udp header */

    if(argc != 3)
    {
        fprintf(stderr, "Need exactly two argument: udp packets filename and desired ipv4 packets filename\n");
        exit(1);
    }

    udp_filename = argv[1];
    ip_filename = argv[2];

    rp = fopen(udp_filename, "rb");
    if(rp == NULL) {
        fprintf(stderr, "error reading udp file\n");
        exit(2);
    }

    wp = fopen(ip_filename, "ab");
    if(wp == NULL) {
        fprintf(stderr, "error opening/creating new ip file\n");
        exit(3);
    }

    while((current_byte = fgetc(rp)) != EOF){
        current_byte = apuh[0];
        for(i=1;i<16; i++) {
            if((apuh[i] = fgetc(rp)) == EOF) {
                printf("Reached end of packet during header read, exiting\n");
                fclose(rp);
                fclose(wp);
                exit(4);
            }
        }
        packet_length = (apuh[12]<<8)|apuh[13];
        packet_length += 20;

        fputc(0x45, wp); /* IP version 4, header 5 words (20 bytes) */
        fputc(0x00, wp); /* ToS */
        fputc(packet_length>>8, wp); /* Total packet length */
        fputc(packet_length & 0xFF, wp);
        fputc(0x00, wp); /* Identification used mainly for fragmentation */
        fputc(0x00, wp);
        fputc(0x00, wp); /* Flags and fragment offset */
        fputc(0x00, wp);
        fputc(0x40, wp); /* Time to Live (64 hops) */
        fputc(0x11, wp); /* UDP Protocol */
        fputc(0x00, wp); /* TODO: calculate checksum */
        fputc(0x00, wp);
        for(i=0;i<8;i++) /* Source and destination addresses */
            fputc(apuh[i], wp);
        packet_length -= 28; /* Subtract IP and UDP headers */
        for(i=0;i<packet_length;i++) /* Put rest of data in file */
            fputc(fgetc(rp), wp);
    }

    fclose(rp);
    fclose(wp);
    return 0;
}
