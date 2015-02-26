* Description
vlcsim program for simulating VLC communication.

* Features
- Can read a file as chars and convert to bits. Both the bits and the
  chars are stored in memory as bstrings.
- Can read bits and simulate noise to the reading.
- Can read bits and reconstruct a sequence of chars.

* TODO
- Better noise simulation.
- Better interpretation of noisy readings.
- CRC implementation.
  - Implemented, but the implementation sucks.
- Reread buffer if CRC fails.
  - Works, but see above.
- Separate the sender and reader into two threads.
- Packet construction.
  - The buffer is sort-of a packet now.
