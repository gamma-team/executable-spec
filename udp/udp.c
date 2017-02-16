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
           "\t%s <rx|tx> <len>\n"
           "\nInput is read from stdin, output is sent to stdout.\n"
           "len is the length of the input\n",
           name);
}

int
main (int argc, char **argv)
{
  int status;
  FILE *fp_in, *fp_out;
  uint8_t buf_in[IP_MAX_DGRAM_LEN], buf_out[IP_MAX_DGRAM_LEN];
  bool rx;
  unsigned long long len;
  uint16_t buf_out_len;
  uint32_t result_addr_src, result_addr_dst;
  uint16_t port_dst, port_src;
  uint16_t result_port_dst, result_port_src;
  uint8_t proto;
  uint32_t addr_src, addr_dst;
  long data_len;

  if (argc < 3)
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
  errno = 0;
  len = strtoull (argv[2], NULL, 0);
  if (0 != errno)
    {
      fprintf (stderr, "Invalid len\n");
      return EXIT_FAILURE;
    }

  fp_in = stdin;
  fp_out = stdout;
  if (rx)
    {
      /* RX input file format:
       * Source address
       * Destination address
       * Protocol
       * IP datagram data section
       */
      assert (1 == fread (&addr_src, sizeof (addr_src), 1, fp_in));
      assert (1 == fread (&addr_dst, sizeof (addr_dst), 1, fp_in));
      assert (1 == fread (&proto, sizeof (proto), 1, fp_in));
      data_len = len - ftell (fp_in);
      assert (1 == fread (buf_in, data_len, 1, fp_in));
      status = udp_rx (true, addr_src, addr_dst, proto, buf_in, len, buf_out,
                       &buf_out_len, &result_port_dst, &result_port_src,
                       &result_addr_src);
    }
  else
    {
      /* TX input file format:
       * Source address
       * Destination address
       * Source port
       * Destination port
       * Data for the UDP datagram data section
       */
      assert (1 == fread (&addr_src, sizeof (addr_src), 1, fp_in));
      assert (1 == fread (&addr_dst, sizeof (addr_dst), 1, fp_in));
      assert (1 == fread (&port_src, sizeof (port_src), 1, fp_in));
      assert (1 == fread (&port_dst, sizeof (port_dst), 1, fp_in));
      data_len = len - ftell (fp_in);
      assert (1 == fread (buf_in, data_len, 1, fp_in));
      status = udp_tx (true, addr_src, addr_dst, port_src, port_dst, buf_in,
                       len, buf_out, &buf_out_len, &result_addr_src,
                       &result_addr_dst);
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
          /* RX file format:
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
          assert (1 == fwrite (buf_out, buf_out_len, 1, fp_out));
        }
      else
        {
          /* TX file format:
           * Source address
           * Destination address
           * UDP datagram
           */
          assert (1 == fwrite (&result_addr_src, sizeof (result_addr_src),
                               1, fp_out));
          assert (1 == fwrite (&result_addr_dst, sizeof (result_addr_dst),
                               1, fp_out));
          assert (1 == fwrite (buf_out, buf_out_len, 1, fp_out));
        }
    }
  status = EXIT_SUCCESS;

err:
  assert (0 == fclose (fp_out));
  assert (0 == fclose (fp_in));

  return status;
}