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
#include <stdint.h>
#include <arpa/inet.h>
#include "checksum.h"

/* 32bit to accumulate carries */
static uint32_t checksum_accum;

void
checksum_reset (void)
{
  checksum_accum = 0;
}

uint16_t
checksum_get (void)
{
  /* Add wrap-around bits */
  return htons ((checksum_accum & 0xffff) + (checksum_accum >> 16 & 0xffff));
}

uint16_t
checksum_get_hdr_fmt (void)
{
  uint16_t t;

  /* "Checksum is the 16-bit one's complement of the one's complement sum
   * of..."
   */
  t = ~checksum_get ();
  /* "If the computed  checksum  is zero,  it is transmitted  as all ones..."
   */
  if (0 == t)
    return 0xffff;
  else
    return t;
}

void
checksum_update (uint16_t val)
{
  checksum_accum += ntohs (val);
}

void
checksum_update32 (uint32_t val)
{
  checksum_update (val & 0xffff);
  checksum_update (val >> 16 & 0xffff);
}
