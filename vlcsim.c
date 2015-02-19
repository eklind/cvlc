#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "dbg.h"

#define MAX_DATA 256
typedef unsigned char byte;


unsigned int get_bit(byte n, int bitnum){
  return (n & (1 << bitnum)) >> bitnum;;
}

int chars_to_bits(char *in, char *out)
{
  FILE *in_file = NULL;
  FILE *out_file = NULL;

  in_file = fopen(in, "r");
  check(in_file, "Failed to open input file.");

  out_file = fopen(out, "w");
  check(out_file, "Failed to open output file.");

  debug("Storing %s as bits in %s.", in, out);
  debug("Buffer length: %d", MAX_DATA);

  //Buffer for storing the chars read from file
  byte buffer[MAX_DATA] = {0};

  //Buffer for storing the bits for every char. Must be 8 times larger.
  char bit_buffer[MAX_DATA * 8] = {0};

  unsigned int bit = 0;
  int len = 0;
  int i = 0;
  int j = 0;

  //Read chars from the file
  while((len = fread(buffer, sizeof(byte), MAX_DATA - 1, in_file)) > 0){

    //Convert the buffer to bits
    for(i = 0;i < len;i++){
      printf("%c -> ", buffer[i]);
      for(j = 0;j < 8;j++){
        bit = get_bit(buffer[i], j);
        printf("%d", bit);

        //Convert to ASCII
        if(bit == 0)  bit_buffer[i*8 + j] = '0';
        else if(bit == 1) bit_buffer[i*8 + j] = '1';
        else sentinel("Illegal bit value.");
      }
      printf("\n");

    }
    //Write the bits to file
    fwrite(bit_buffer, sizeof(char), len * 8, out_file);
  }

  //Cleanup
  fflush(out_file);
  fclose(in_file);
  fclose(out_file);

  return 0;

 error:
  if(in_file) fclose(in_file);
  if(out_file) fclose(out_file);
  return -1;
}

int bits_to_chars(char *in, char *out)
{
  FILE *in_file = NULL;
  FILE *out_file = NULL;

  in_file = fopen(in, "r");
  check(in_file, "Failed to open input file.");

  out_file = fopen(out, "w");
  check(out_file, "Failed to open output file.");

  debug("Storing %s as chars in %s.", in, out);

  //Buffer for storing bits read from file
  char bit_buffer[MAX_DATA * 8 + 1] = {0};

  //Buffer for storing the calculated ascii values
  char buffer[MAX_DATA] = {0};

  char bit = 0;
  int len = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  
  //Read the bits into bit_buffer
  while((len = fread(bit_buffer, sizeof(char), MAX_DATA * 8, in_file)) > 0){
    debug("Read %d bits.", len);
    check(len % 8 == 0, "Must read bits in blocks of 8.");

    //Looping over bytes read
    for(i = 0;i < len / 8;i++){

      //Looping over the bits in every byte
      for(j = 0;j < 8;j++){

        //Convert the chars read into ones and zeros
        if(bit_buffer[i*8 + j] == '0') bit = 0;
        else if(bit_buffer[i*8 + j] == '1') bit = 1;

        printf("%d", bit);

        //Calculate the value of the current bit
        for(k = 0;k < j;k++) bit *= 2;

        //Add the value to the buffer slow
        buffer[i] += bit;
      }
      printf(" -> %c\n", buffer[i]);
    }
    
    //Write the chars to file
    fwrite(buffer, sizeof(char), len / 8, out_file);
  }

  //Cleanup
  fflush(out_file);
  fclose(in_file);
  fclose(out_file);

  return 0;

 error:
  if(in_file) fclose(in_file);
  if(out_file) fclose(out_file);

  return -1;  
}


int main(int argc, char *argv[])
{
  int rc = 0;
  rc = chars_to_bits("in_file.txt", "out_file.txt");
  rc = bits_to_chars("out_file.txt", "char_file.txt");
}
