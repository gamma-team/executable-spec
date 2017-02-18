/*
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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <arpa/inet.h>
#include "checksum.h"
#include "config.h"
#include "tx.h"

struct udp_dgram_pseudo_hdr {
    uint32_t addr_src;
    uint32_t addr_dst;
    uint16_t proto;
    uint16_t udp_len;
};

struct udp_dgram_hdr {
    uint16_t port_src;
    uint16_t port_dst;
    uint16_t len;
    uint16_t checksum;
};

/* NOTE: Didn't bother to mimic HDL flow like with udp_rx */
int
udp_tx (bool verbose, uint32_t addr_src, uint32_t addr_dst, uint16_t port_src,
        uint16_t port_dst, const uint8_t *data, size_t data_len,
        uint8_t *out, uint16_t *out_len, uint32_t *out_addr_src,
        uint32_t *out_addr_dst)
{
  struct udp_dgram_hdr *hdr;
  struct udp_dgram_pseudo_hdr pseudo_hdr;
  uint8_t *payload;

  assert (UINT16_MAX >= sizeof (*hdr) + data_len);

  hdr = (struct udp_dgram_hdr *)out;
  payload = out + sizeof (*hdr);

  hdr->port_src = port_src;
  hdr->port_dst = port_dst;
  hdr->len = htons (sizeof (*hdr) + data_len);
  pseudo_hdr.addr_src = addr_src;
  pseudo_hdr.addr_dst = addr_dst;
  pseudo_hdr.proto = htons (UDP_PROTO);
  pseudo_hdr.udp_len = hdr->len;
  /* Copy data payload */
  for (size_t i = 0; i < data_len; ++i)
    payload[i] = data[i];
  *out_len = ntohs (hdr->len);
  /* Handle checksum calculation */
  checksum_reset ();
  checksum_update (hdr->port_src);
  checksum_update (hdr->port_dst);
  checksum_update (hdr->len);
  checksum_update32 (pseudo_hdr.addr_src);
  checksum_update32 (pseudo_hdr.addr_dst);
  checksum_update (pseudo_hdr.proto);
  checksum_update (pseudo_hdr.udp_len);
  size_t word_len = data_len / 2;
  for (uint16_t *d = (uint16_t *)payload; d < (uint16_t *)payload + word_len;
       ++d)
    checksum_update (*d);
  if (0 != data_len % 2)
    checksum_update (htons (data[data_len - 1] << 8));
  hdr->checksum = checksum_get_hdr_fmt ();
  /* Fill in remaining outputs */
  *out_addr_src = pseudo_hdr.addr_src;
  *out_addr_dst = pseudo_hdr.addr_dst;

  if (verbose)
    {
      fprintf (stderr, "Source port: %" PRIu16 "\n", ntohs (hdr->port_src));
      fprintf (stderr, "Destination port: %" PRIu16 "\n",
               ntohs (hdr->port_dst));
      fprintf (stderr, "Length: %" PRIu16 "\n", ntohs (hdr->len));
      fprintf (stderr, "Checksum: %#" PRIx16 "\n", ntohs (hdr->checksum));
    }

  return 0;
}
