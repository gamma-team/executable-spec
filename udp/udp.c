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
#include "config.h"
#include "rx.h"
#include "tx.h"

void
usage (char *name)
{
  fprintf (stderr,
           "Usage:\n"
           "\t%s <rx|tx <addr_src> <addr_dst> <port_src>> <port_dst> <len>\n"
           "\nInput is read from stdin, output is sent to stdout.\n"
           "\nRX Mode:\n"
           "Input is a single raw IP datagram of length len."
           "\nTX Mode:\n"
           "Input is a raw UDP data payload of length len.\n",
           name);
}

int
main (int argc, char **argv)
{
  int status;
  FILE *fp_in, *fp_out;
  uint8_t buf_in[IP_MAX_DGRAM_LEN], buf_out[IP_MAX_DGRAM_LEN];
  bool rx;
  size_t len;
  uint16_t buf_out_len;
  uint32_t result_addr_src, result_addr_dst;
  uint16_t port_dst, port_src;
  uint16_t result_port_dst, result_port_src;
  char *addr_src, *addr_dst;

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
  if (rx)
    {
      if (argc < 4)
        {
          fprintf (stderr, "Not enough arguments\n");
          usage (argv[0]);
          return EXIT_FAILURE;
        }
      port_dst = atoi (argv[2]);
      len = atoi (argv[3]);
    }
  else
    {
      if (argc < 7)
        {
          fprintf (stderr, "Not enough arguments\n");
          usage (argv[0]);
          return EXIT_FAILURE;
        }
      addr_src = argv[2];
      addr_dst = argv[3];
      port_src = atoi (argv[4]);
      port_dst = atoi (argv[5]);
      len = atoi (argv[6]);
    }

  fp_in = stdin;
  fp_out = stdout;
  assert (1 == fread (buf_in, len, 1, fp_in));
  if (rx)
    status = udp_rx (true, port_dst, buf_in, len, buf_out, &buf_out_len,
                     &result_port_dst, &result_port_src,
                     &result_addr_src);
  else
    status = udp_tx (true, addr_src, addr_dst, port_src, port_dst, buf_in,
                     len, buf_out, &buf_out_len, &result_addr_src,
                     &result_addr_dst);
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
