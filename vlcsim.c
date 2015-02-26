#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "dbg.h"
#include "bstrlib.h"
#include "mac1.h"

#define MAX_DATA 256
#define SAMPLES 15
typedef unsigned char byte;

bstring char_in_buffer;   /* Char input buffer. */
bstring char_out_buffer;  /* Char output buffer. */
bstring bit_in_buffer;    /* Bit input buffer. */
bstring bit_out_buffer;   /* Bit output buffer. */
bstring sample_buffer;    /* Bits conerted to samples. */

/* Description: Read an int and return the indicated bit.
 * Author: Unknown
 * Date: Unknown
 */
unsigned int mask_bit(unsigned int n, int bitnum){
  return (n & (1 << bitnum)) >> bitnum;;
}

/* Description: Read a char and return the string of 8 bits it consists of.
 * Author: Albin Severinson
 * Date: 26/02/15
 */
char *get_bits(char n)
{
  char bits[8] = {0};
  int i = 0;
  for(i = 0;i < 8;i++){
    bits[i] = mask_bit(n, i);
  }
  return bits;
}

/* Description: Read chars from a file and store in buffer.
 * Author: Albin Severinson
 * Date: 19/02/15
 */
int read_chars(char *in)
{
  assert(in && "Invalid file handle when loading chars.");
  int rc = 0;

  FILE *in_file = NULL;
  struct bStream *stream = NULL;

  //Open the input file
  in_file = fopen(in, "r");
  check(in_file, "Failed to open input file.");

  //Open as a bStream
  stream = bsopen((bNread) fread, in_file);
  check(stream, "Failed to open the bStream.");

  char_in_buffer = bfromcstr("");

  //Read chars into memory
  rc = bsreadln(char_in_buffer, stream, '\n');
  check(rc == BSTR_OK, "Failed to read chars into memory.");

  //Trim whitespace
  check(btrimws(char_in_buffer) == BSTR_OK, "Failed to trim whitespace.");

  debug("Buffer: %s", bdata(char_in_buffer));

  in_file = bsclose(stream);
  fclose(in_file);

  return 0;
  
 error:
  bdestroy(char_out_buffer);
  if(stream) in_file = bsclose(stream);
  if(in) fclose(in_file);
  return -1;
}

/* Description: Read a buffer of chars and convert to a bstring of bits.
 * Author: Albin Severinson
 * Date: 26/02/15
 */
int chars_to_bits()
{
  assert(char_in_buffer && "Char buffer can't be NULL when converting to bits.");
  assert(char_in_buffer->mlen > 0 && "Char buffer must be filled when converting to bits.");

  int rc = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  char c = 0;
  unsigned char crc = 0;

  debug("Converting chars to bits.");

  //Initialize the bit buffer
  bit_out_buffer = bfromcstr("");
  check(bit_out_buffer, "Failed to init the bit buffer.");
  
  //Read chars from the char buffer, convert to bits, and append to
  //bit buffer
  for(i = 0;(c = bchar(char_in_buffer, i)) != '\0';i++){
    printf("%c -> ", c);

    //Convert a char to bits
    for(j = 0;j < 8;j++){
      rc = binsertch(bit_out_buffer, i*8 + j, 1, mask_bit(c, j));
      check(rc == BSTR_OK, "Failed to append bits.");
      printf("%d", bchar(bit_out_buffer, i*8 + j));
    }

    //Add to CRC
    crc += c;

    printf("\n");
  }

  printf("CRC: %d -> ", crc);

  //Append the CRC
  for(k = 0;k < 8;k++){
    rc = binsertch(bit_out_buffer, i*8 + k, 1, mask_bit(crc, k));
    check(rc == BSTR_OK, "Failed to append CRC.");
    printf("%d", bchar(bit_out_buffer, i*8 + k));
  }
  printf("\n");

  printf("Bit output buffer: ");
  for(i = 0;i < blength(bit_out_buffer);i++){
    printf("%d", bchar(bit_out_buffer, i));
  }
  printf("\n");

  return 0;

 error:
  bdestroy(bit_out_buffer);
  return -1;
}

/* Description: Sample a bit with simulated noise added, and attempt
 * to average out the noise.
 * Author: Albin Severinson
 * Date: 19/02/15
 */
char get_next_bit(int n)
{
  int i = 0;
  char c = 0;
  double d = 0;

  c = bchar(bit_out_buffer, n);
  
  //Read bits from the bit out buffer, convert to samples and add noise.
  for(i = 0;i < SAMPLES;i++){

    //Multiply with 100 and add noise
    d += c * 100;

    //TODO better noise generation
    d += ((random() % 200) - (random() % 200));
  }

  d = d / SAMPLES;
  //printf("[%f]", d);
  if(d > 50) return 1;
  else return 0;
}

/* Description: Read a bstring of bits and attempt to reconstruct the data.
 * Author: Albin Severinson
 * Date: 19/02/15
 */
int bits_to_chars()
{
  assert(bit_out_buffer && "Bit output buffer must be filled when converting to chars.");
  assert(blength(bit_out_buffer) > 0 && "But output buffer must be filled when converting to chars.");
  assert(blength(bit_out_buffer) % 8 == 0 && "Bit buffer size must be a multiple of 8.");

  int rc = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  char c = 0;
  unsigned char crc = 0;
  unsigned char got_crc = 0;
  int measured_bit = 0;
  int real_bit = 0;
  int bit_error = 0;

  debug("Converting bits to chars.");

  //Init the output buffer
  char_out_buffer = bfromcstr("");
  check(char_out_buffer, "Failed to init char output buffer.");

  //Add up the bits in every byte to a char
  for(i = 0;i < blength(bit_out_buffer) / 8;i++){
    c = 0;

    //Read 8 bits at a time
    for(j = 0;j < 8;j++){

      //Get the real bit for comparison
      real_bit = bchar(bit_out_buffer, i*8 + j);

      //Get the measured bit
      measured_bit = get_next_bit(i*8 + j);
      printf("%d", measured_bit);

      //Check for bit errors
      if(real_bit != measured_bit) bit_error++;

      //Calculate the value of the current bit
      for(k = 0;k < j;k++) measured_bit *= 2;
      
      //Add up
      c += measured_bit;
    }
    printf(" -> %c\n", c);

    //Append char to buffer
    rc = binsertch(char_out_buffer, i, 1, c);
    check(rc == BSTR_OK, "Failed to append char.");

    //Add to CRC
    crc += c;
  }

  //Get the CRC that was sent along the data
  got_crc = bchar(char_out_buffer, i - 1);

  //Subtract that from the computed CRC, since the crc isn't part of
  //the sum when calculated on the sender side.
  crc -= got_crc;

  printf("Computed CRC: %d / Got CRC: %d", crc, got_crc);
  printf(" [%d bit(s) in error]\n", bit_error);

  //Check if the CRC matches
  if(crc != got_crc){
    printf("CRC failed! Re-reading buffer.\n");
    //TODO: Re-read the buffer if this fails.
  }

  printf("Char output buffer: ");
  for(i = 0;i < blength(char_out_buffer);i++){
    printf("%c", bchar(char_out_buffer, i));
  }
  printf("\n");

  return 0;

 error:
  bdestroy(char_out_buffer);
  return -1;
}

int main(int argc, char *argv[])
{
  int rc = 0;
  rc = read_chars("in_file.txt"); 
  rc = chars_to_bits();
  rc = bits_to_chars();

  //Send to MAC1 layer
  rc = send_bstring(bit_out_buffer);
  bstring recieved_bstring = get_bstring();
  printf("Recieved bstring from MAC1: %s\n", bdata(recieved_bstring));

  return rc;
}
