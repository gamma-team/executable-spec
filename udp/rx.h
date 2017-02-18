/*
 * UDP receiver for data from the IP layer to the application layer
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
#ifndef RX_H
#define RX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define RX_ERROR_NONE (0x0)
#define RX_ERROR_CHECKSUM (0x1)
#define RX_ERROR_PORT (0x2) /* no longer used */
#define RX_ERROR_IP_HDR_LEN (0x4) /* no longer used */
#define RX_ERROR_NOT_UDP (0x8)

/* UDP receiver executable spec
 *
 * verbose: Enable debug printing to stderr if true
 * addr_src: IPv4 source address in network byte order
 * addr_dst: IPv4 destination address in network byte order
 * proto: Protocol of dgram from the IP header
 * dgram: IP data section
 * dgram_len: Length of dgram
 * out: Output array for the datasection of dgram if it is UDP
 * out_len: Length of data written to out
 * out_port_dst: Output for the destination port read from the UDP header
 *               (network byte order)
 * out_port_src: Output for the source port read from the UDP header (network
 *               byte order)
 * out_addr_src: Set to the value of addr_src
 *
 * Returns 0 on success
 */
int udp_rx (bool verbose, uint32_t addr_src, uint32_t addr_dst, uint8_t proto,
            const uint8_t *dgram, size_t dgram_len, uint8_t *out,
            uint16_t *out_len, uint16_t *out_port_dst, uint16_t *out_port_src,
            uint32_t *out_addr_src);

#endif /* RX_H */
