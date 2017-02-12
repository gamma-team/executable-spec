pcap_to_ipv4_udp.c
  reads pcap and strips out everything but IPv4 packets containing UDP

ipv4_to_udp.c
  converts IPv4 packets to UDP packets

Notes:
- You may need to apt-get install libpcap-dev or the equivalent
- Run 'make all' to build
