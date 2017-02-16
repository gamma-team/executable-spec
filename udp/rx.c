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
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <inttypes.h>
#include "checksum.h"
#include "config.h"
#include "rx.h"

/* Think of these as registers */
static int error;
static size_t count;
static uint8_t hdr_ip_hdr_len;
static uint32_t hdr_ip_addr_src;
static uint32_t hdr_ip_addr_dst;
static uint16_t hdr_ip_len;
static uint16_t hdr_udp_port_src;
static uint16_t hdr_udp_port_dst;
static uint16_t hdr_udp_checksum;
static uint16_t hdr_udp_len;

/* Defines the data consumption interface. Think of len as a valid signal,
 * since transactions at the end may not always match the bus width. out_len
 * can be treated as a valid signal as well.
 * It doesn't truly mimic a pipeline, but it goes through the data processing
 * steps that will be encountered in HDL in a sequential fashion with limited
 * data, rather than just extracting data from a complete datagram.
 */
static void
udp_rx_pipeline (uint16_t port, const uint8_t *data, size_t len, uint8_t *out,
                 size_t *out_len)
{
  if (0 == count)
    hdr_ip_hdr_len = IP_HDR_LEN_MIN;
  /* Require minimum 4 byte bus, should always get minimum of 32bits at a time
   * during header data transfer. IP and UDP header fields never cross 32bit
   * boundaries either, so don't allow non-dword aligned len.
   */
  if (count < hdr_ip_hdr_len + UDP_HDR_LEN)
    {
      if (4 > len)
        assert (count + len >= hdr_ip_hdr_len + UDP_HDR_LEN);
      /* Check alignment if this transfer will only be header data */
      if (count + len <= hdr_ip_hdr_len + UDP_HDR_LEN)
        assert (0 == len % 4);
    }

  *out_len = 0;
  /* Discard data if an error has occured */
  if (error)
    return;
  for (size_t i = count; i < count + len; ++i, ++data)
    {
      if (i < hdr_ip_hdr_len + UDP_HDR_LEN)
        {
          /* unpack big endian */
          uint16_t s = *(uint16_t *)data;
          uint32_t l = *(uint32_t *)data;
          if (i < hdr_ip_hdr_len)
            {
              /* Handle virtual header checksumming and information extraction
               * from the IP header
               */
              switch (i)
                {
                case IP_HDR_OFF_VER_IHL:
                  hdr_ip_hdr_len = (*data & 0xf);
                  if (hdr_ip_hdr_len < 5)
                    error |= RX_ERROR_IP_HDR_LEN;
                  /* Convert to octet length */
                  hdr_ip_hdr_len *= 4;
                  break;
                case IP_HDR_OFF_LEN:
                  hdr_ip_len = ntohs (s);
                  checksum_update (htons (hdr_ip_len - hdr_ip_hdr_len));
                  break;
                case IP_HDR_OFF_PROTO:
                  if (UDP_PROTO != *data)
                    error |= RX_ERROR_NOT_UDP;
                  checksum_update (htons (UDP_PROTO));
                  break;
                case IP_HDR_OFF_ADDR_SRC:
                  hdr_ip_addr_src = ntohl (l);
                  checksum_update32 (l);
                  break;
                case IP_HDR_OFF_ADDR_DST:
                  hdr_ip_addr_dst = ntohl (l);
                  checksum_update32 (l);
                  break;
                default:
                  break;
                }
            }
          else
            {
              switch (i - hdr_ip_hdr_len)
                {
                case UDP_HDR_OFF_PORT_SRC:
                  hdr_udp_port_src = ntohs (s);
                  checksum_update (s);
                  break;
                case UDP_HDR_OFF_PORT_DST:
                  hdr_udp_port_dst = ntohs (s);
                  checksum_update (s);
                  /* Raise error if port does not match */
                  if (hdr_udp_port_dst != port)
                    error |= RX_ERROR_PORT;
                  break;
                case UDP_HDR_OFF_LEN:
                  hdr_udp_len = ntohs (s);
                  checksum_update (s);
                  break;
                case UDP_HDR_OFF_CHK:
                  hdr_udp_checksum = ntohs (s);
                  checksum_update (s);
                  break;
                default:
                  break;
                }
            }
        }
      else
        {
          if (0 == i % 2)
            {
              /* last octet in an odd-length payload */
              if (i + 1 >= hdr_ip_len)
                /* pad with zero; use htons for portability */
                checksum_update (htons (*data << 8));
              else
                checksum_update (*(uint16_t *)data);
            }
          *out = *data;
          ++out;
          ++*out_len;
        }
    }
  count += len;
}

int
udp_rx (bool verbose, uint16_t port, const uint8_t *ip_dgram,
        size_t ip_dgram_len, uint8_t *out, uint16_t *out_len,
        uint16_t *port_src, uint32_t *addr_src)
{
  assert (ip_dgram_len <= UINT16_MAX);

  error = RX_ERROR_NONE;
  count = 0;
  checksum_reset ();

  *out_len = 0;
  for (size_t i = 0; i < ip_dgram_len; i += UDP_DATA_WIDTH_BYTES)
    {
      size_t l;
      if (ip_dgram_len - i < UDP_DATA_WIDTH_BYTES)
        udp_rx_pipeline (port, &ip_dgram[i], ip_dgram_len - i, out, &l);
      else
        udp_rx_pipeline (port, &ip_dgram[i], UDP_DATA_WIDTH_BYTES, out, &l);
      *out_len += l;
      out += l;
    }

  /* Skip check if header checksum is 0 */
  if (0 != hdr_udp_checksum)
    /* 0xffff sum indicates validity */
    if (0xffff != checksum_get ())
      error |= RX_ERROR_CHECKSUM;

  if (verbose)
    {
      struct in_addr a;

      a.s_addr = htonl (hdr_ip_addr_src);
      fprintf (stderr, "Source Address: %s\n", inet_ntoa (a));
      a.s_addr = htonl (hdr_ip_addr_dst);
      fprintf (stderr, "Destination Address: %s\n", inet_ntoa (a));
      fprintf (stderr, "Source Port: %" PRIu16 "\n", hdr_udp_port_src);
      fprintf (stderr, "Destination Port: %" PRIu16 "\n", hdr_udp_port_dst);
      fprintf (stderr, "UDP Header Checksum: %#" PRIx16 "\n",
               hdr_udp_checksum);
      fprintf (stderr, "Data Length from Header: %#" PRIx16 "\n",
               hdr_udp_len - UDP_HDR_LEN);
      fprintf (stderr, "Data Length from Datapath: %#" PRIx16 "\n", *out_len);
      fprintf (stderr, "Error: %d\n", error);
    }

  *port_src = hdr_udp_port_src;
  *addr_src = hdr_ip_addr_src;
  if (RX_ERROR_NONE == error)
    return 0;
  else
    return -1;
}
