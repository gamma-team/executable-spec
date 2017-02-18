Programs
--------

udp
  The main executable specification program. It generates outputs of the TX
  and RX paths depending on the first argument. Run with no arguments for a
  usage printout. Files from the input generation scripts or the IP executable
  spec should be used as input.

udp_tx_in_gen.py
  Generates custom input files for the udp program in rx mode. Run with '-h'
  for usage.

udp_tx_in_gen.py
  Generates custom input files for the udp program in tx mode. Run with '-h'
  for usage.

Build
-----

  make all

Test
----

To verify function on your machine, all tests should pass:

  make check
