#!/usr/bin/env python2
#
# Data generator for custom input files for the UDP RX executable spec
#
# Note: Scapy is licensed under GPLv2, so this program is as well.
#
# Copyright 2017 Patrick Gauvin
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
# USA.
import argparse
import socket
import struct
import sys
from scapy.all import IP, UDP

def write_file(f_out, f_in, args):
    if f_in:
        data = f_in.read()
    else:
        data = args.data

    p = (IP(src=args.src, dst=args.dst)
         / UDP(sport=args.sport, dport=args.dport) / data)
    f_out.write(struct.pack('!B', p.proto))
    f_out.write(socket.inet_aton(p.src))
    f_out.write(socket.inet_aton(p.dst))
    f_out.write(str(p.getlayer('UDP')))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create input-data files for UDP RX')
    parser.add_argument('src', help='IPv4 source address, e.g., 127.0.0.1')
    parser.add_argument('dst',
                        help='IPv4 destination address, e.g., 127.0.0.1')
    parser.add_argument('sport', type=int, help='UDP source port')
    parser.add_argument('dport', type=int, help='UDP destination port')
    parser.add_argument('--data', default='',
                        help='Data for the UDP payload (string)')
    parser.add_argument('-o', dest='fname_output', default=None,
                        help='Output file')
    parser.add_argument('-i', dest='fname_input', default=None,
                        help='Data input for the UDP payload, overrides --data')
    args = parser.parse_args()

    if args.fname_input:
        f_in = open(args.fname_input, 'r')
    else:
        f_in = None

    if args.fname_output:
        with open(args.fname_output, 'w') as f:
            write_file(f, f_in, args)
    else:
        write_file(sys.stdout, f_in, args)
