# UDP executable spec make file
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
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c99 -D_DEFAULT_SOURCE
OBJ=udp.o rx.o tx.o checksum.o
CLEANFILES=$(OBJ) udp

all: udp

udp: $(OBJ)
	$(CC) -o $@ $^

checksum.o: checksum.c checksum.h
	$(CC) $(CFLAGS) -c -o $@ $<

rx.o: rx.c rx.h config.h checksum.h
	$(CC) $(CFLAGS) -c -o $@ $<

tx.o: tx.c tx.h config.h checksum.h
	$(CC) $(CFLAGS) -c -o $@ $<

udp.o: udp.c config.h rx.h tx.h
	$(CC) $(CFLAGS) -c -o $@ $<

check: udp
	@set -e; \
	for i in rx-odd rx-odd2 rx-even ; do \
	  ./udp rx < tests/$$i.bin > $$i.res.bin; \
	  cmp tests/$$i.res.bin $$i.res.bin; \
	  echo $$i pass ; \
	done
	@set -e; \
	for i in tx-odd tx-odd2 tx-even ; do \
	  ./udp tx < tests/$$i.bin > $$i.res.bin; \
	  cmp tests/$$i.res.bin $$i.res.bin; \
	  echo $$i pass ; \
	done

clean:
	rm -f $(CLEANFILES)
