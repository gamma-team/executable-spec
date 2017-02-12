all: ip udp

ip:
	gcc pcap_to_ipv4_udp.c -lpcap -o ptiu

udp:
	gcc ipv4_to_udp.c -o itu

clean:
	-rm -rvf ptiu itu *.bin
