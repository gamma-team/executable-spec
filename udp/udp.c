/*
 * Main program for the UDP executable spec
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "config.h"
#include "rx.h"
#include "tx.h"

void
usage (char *name)
{
  fprintf (stderr,
           "Usage:\n"
           "\t%s <rx|tx> [--verbose|-v]\n"
           "\nInput is read from stdin, output is sent to stdout. In verbose\n"
           "mode, extra information about the transaction is printed to stderr\n",
           name);
}

int
main (int argc, char **argv)
{
  int status;
  FILE *fp_in, *fp_out;
  uint8_t buf_in[IP_MAX_DGRAM_LEN], buf_out[IP_MAX_DGRAM_LEN];
  bool rx, verbose;
  size_t len;
  uint16_t buf_out_len;
  uint32_t result_addr_src, result_addr_dst;
  uint16_t port_dst, port_src;
  uint16_t result_port_dst, result_port_src;
  uint8_t result_proto;
  uint8_t proto;
  uint32_t addr_src, addr_dst;

  if (argc < 2)
    {
      fprintf (stderr, "Not enough arguments\n");
      usage (argv[0]);
      return EXIT_FAILURE;
    }
  if (0 == strcmp (argv[1], "rx"))
    rx = true;
  else if (0 == strcmp (argv[1], "tx"))
    rx = false;
  else
    {
      fprintf (stderr, "Invalid argument\n");
      usage (argv[0]);
      return EXIT_FAILURE;
    }
  verbose = false;
  if (argc >= 3)
    if (0 == strcmp (argv[2], "--verbose")
        || 0 == strcmp (argv[2], "-v"))
      verbose = true;

  fp_in = stdin;
  fp_out = stdout;
  if (rx)
    {
      /* RX input file format (all integer types are network byte order):
       * Source address
       * Destination address
       * Protocol
       * IP datagram data section (up to 65535 bytes)
       */
      assert (1 == fread (&addr_src, sizeof (addr_src), 1, fp_in));
      assert (1 == fread (&addr_dst, sizeof (addr_dst), 1, fp_in));
      assert (1 == fread (&proto, sizeof (proto), 1, fp_in));
      len = fread (buf_in, 1, UINT16_MAX, fp_in);
      if (UINT16_MAX != len)
        assert (!ferror (fp_in));
      status = udp_rx (verbose, addr_src, addr_dst, proto, buf_in, len,
                       buf_out, &buf_out_len, &result_port_dst,
                       &result_port_src, &result_addr_src);
    }
  else
    {
      /* TX input file format (all integer types are network byte order):
       * Source address
       * Destination address
       * Source port
       * Destination port
       * Data for the UDP datagram data section (up to 65535 - 8 bytes)
       */
      assert (1 == fread (&addr_src, sizeof (addr_src), 1, fp_in));
      assert (1 == fread (&addr_dst, sizeof (addr_dst), 1, fp_in));
      assert (1 == fread (&port_src, sizeof (port_src), 1, fp_in));
      assert (1 == fread (&port_dst, sizeof (port_dst), 1, fp_in));
      len = fread (buf_in, 1, UINT16_MAX - UDP_HDR_LEN, fp_in);
      if (UINT16_MAX - UDP_HDR_LEN != len)
        assert (!ferror (fp_in));
      status = udp_tx (verbose, addr_src, addr_dst, port_src, port_dst,
                       buf_in, len, buf_out, &buf_out_len, &result_addr_src,
                       &result_addr_dst, &result_proto);
    }
  if (0 != status)
    {
      fprintf (stderr, "Transfer error: %x\n", status);
      status = EXIT_FAILURE;
      goto err;
    }
  else
    {
      if (rx)
        {
          /* RX output file format (all integer types are network byte order):
           * Source address
           * Source port
           * Destination port
           * UDP datagram's data payload
           */
          assert (1 == fwrite (&result_addr_src, sizeof (result_addr_src),
                               1, fp_out));
          assert (1 == fwrite (&result_port_src, sizeof (result_port_src),
                               1, fp_out));
          assert (1 == fwrite (&result_port_dst, sizeof (result_port_dst),
                               1, fp_out));
          if (0 != buf_out_len) /* Zero length payloads are legal */
            assert (1 == fwrite (buf_out, buf_out_len, 1, fp_out));
        }
      else
        {
          /* TX output file format (all integer types are network byte order):
           * Source address
           * Destination address
           * Protocol
           * UDP datagram
           */
          assert (1 == fwrite (&result_addr_src, sizeof (result_addr_src),
                               1, fp_out));
          assert (1 == fwrite (&result_addr_dst, sizeof (result_addr_dst),
                               1, fp_out));
          assert (1 == fwrite (&result_proto, sizeof (result_proto), 1,
                               fp_out));
          assert (1 == fwrite (buf_out, buf_out_len, 1, fp_out));
        }
    }
  status = EXIT_SUCCESS;

err:
  assert (0 == fclose (fp_out));
  assert (0 == fclose (fp_in));

  return status;
}
