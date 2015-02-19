* Description
vlcsim program for simulating VLC communication.
* Features
- Can read a file as chars and convert to bits. The bits are stored in
  a separate file.
- Can read a file of bits and convert to chars.
* TODO
- Expand the bit list into a vector of samples.
- Read samples back to bits.
- Add noise to samples. See how this affects reading. Bit error rate.
- Separate out the code.
  - Bits should be cached in memory separately. No need to write the
    bits to file between every step.
