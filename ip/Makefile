all: ip to_udp from_udp

ip:
	gcc pcap_to_ipv4_udp.c -lpcap -o ptiu

to_udp:
	gcc ipv4_to_udp.c -o itu

from_udp:
	gcc udp_to_ipv4.c -o uti

clean:
	-rm -rvf ptiu itu uti *.bin
