#!/usr/bin/env python2
#
# Data generator for custom input files for the UDP TX executable spec
#
# Copyright 2017 Patrick Gauvin
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
import argparse
import socket
import struct
import sys

def write_file(f, args):
    f.write(socket.inet_aton(args.src))
    f.write(socket.inet_aton(args.dst))
    f.write(struct.pack('!HH', args.sport, args.dport))
    f.write(args.data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create input-data files for UDP TX')
    parser.add_argument('src')
    parser.add_argument('dst')
    parser.add_argument('sport', type=int)
    parser.add_argument('dport', type=int)
    parser.add_argument('data')
    parser.add_argument('-o', dest='fname', default=None)
    args = parser.parse_args()

    if args.fname:
        with open(args.fname, 'w') as f:
            write_file(f, args)
    else:
        write_file(sys.stdout, args)
