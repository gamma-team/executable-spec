/*
 * UDP transmitter for data from the application layer to the IP layer
 *
 * Copyright 2017 Patrick Gauvin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TX_H
#define TX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define TX_ERROR_NONE (0x0)

/* UDP transmitter executable spec
 *
 * verbose: Enable debug printing to stderr if true
 * addr_src: IPv4 source address in network byte order
 * addr_dst: IPv4 destination address in network byte order
 * port_src: UDP source port in network byte order
 * port_dst: UDP destination port in network byte order
 * data: UDP data section data
 * data_len: Length of data
 * out: Output for the complete UDP datagram
 * out_len: Length of data written to out
 * out_addr_src: Set to the value of addr_src
 * out_addr_dst: Set to the value of addr_dst
 *
 * Returns 0 on success
 */
int udp_tx (bool verbose, uint32_t addr_src, uint32_t addr_dst,
            uint16_t port_src, uint16_t port_dst, const uint8_t *data,
            size_t data_len, uint8_t *out, uint16_t *out_len,
            uint32_t *out_addr_src, uint32_t *out_addr_dst,
            uint8_t *out_proto);

#endif /* TX_H */
