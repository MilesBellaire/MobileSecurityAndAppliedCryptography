#include <stdio.h>
#include <stdlib.h>

// Level0
unsigned long* encryptmsg(char* msg, unsigned long key, int len);
char* decryptmsg(unsigned long* msg, unsigned long key, int len);
void encrypt(unsigned long key, unsigned long *m);
void decrypt(unsigned long key, unsigned long *m);

// Level1
unsigned long desRound(unsigned long x, unsigned long key, int round);

// Level2
unsigned int mangler(unsigned long key, unsigned int x);
unsigned long getKey(unsigned long key, int round);

// Level3
unsigned long permute64to56(unsigned long x);
unsigned long permute56to48(unsigned long x);
unsigned int permute32to32(unsigned int x);
unsigned int circularLeftShift(unsigned int x, int shift);
unsigned long sbox32to48(unsigned int x);
unsigned int sbox48to32(unsigned long x); 

// Utility
void ulong_to_char(unsigned long *ul_array, int ul_len, char **char_array, int *char_len);
void char_to_ulong(char *char_array, int char_len, unsigned long **ul_array, int *ul_len);

// Constants
int rounds = 16;
int sbox[4][16] = {
   {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
   {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
   {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
   {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
};

/*
-------------------------
Public Functions(Level 0)
-------------------------
*/

unsigned long* encryptmsg(char* msg, unsigned long key, int len) {
   unsigned long *M = malloc(sizeof(unsigned long) * ((len/8) + 1));
   
   for(int i = 0; i <= len/8; i++) {
      M[i] = *((unsigned long*)msg + i);
      if(i == len/8) {
         if(len%8 == 0) continue;
         M[i] &= ((1 << (len%8*8)) - 1);
      }
      encrypt(key, &M[i]);
   }
   return M;
}

char* decryptmsg(unsigned long* M, unsigned long key, int len) {
   int msgLen = len*8+1;
   char *msg;
   
   for(int i = 0; i <= len/8; i++) {
      decrypt(key, &M[i]);
   }
   ulong_to_char(M, len, &msg, &msgLen);
   return msg;
}

void encrypt(unsigned long key, unsigned long *m) {
   unsigned long tmp;
   unsigned long M = *m;

   // Permute key
   key = permute64to56(key);

   // Do rounds
   for(int r = 1; r <= rounds; r++) {
      M = desRound(M, key, r);
   }

   // Flip left and right
   tmp = M & 0xffffffff;
   M = M >> 32 | tmp << 32;

   *m = M;
}

void decrypt(unsigned long key, unsigned long *m) {
   unsigned long tmp;

   // Permute key
   key = permute64to56(key);

   // Reverse rounds
   for(int r = rounds; r >= 1; r--) {
      *m = desRound(*m, key, r);
   }

   // Flip left and right
   tmp = *m & 0xffffffff;
   *m = *m >> 32 | tmp << 32;
}

/*
-----------------------------
High Level Functions(Level 1)
-----------------------------
*/
unsigned long desRound(unsigned long x, unsigned long key, int round) {
   unsigned int left = x >> 32;
   unsigned int right = x & 0xffffffff;
   unsigned int tmp = right;
   unsigned long subKey;

   // Get SubKey
   subKey = getKey(key, round);

   // Right side Operations
   right = mangler(subKey, right);
   right = right ^ left;

   // Left side Operations
   left = tmp;

   // Rebuild
   x = left;
   x = x << 32 | right;

   return x;
}

/*
-----------------------------------
Round Subjective Functions(Level 2)
-----------------------------------
*/

unsigned int mangler(unsigned long key, unsigned int x) {
   unsigned long tmp;
   
   tmp = sbox32to48(x);
   tmp ^= key;
   x = sbox48to32(tmp);

   x = permute32to32(x);

   return x;
}

// Key is currently 56 bits
unsigned long getKey(unsigned long key, int round) {

   unsigned int left = key >> 26;
   unsigned int right = key & 0x3ffffff;

   // Do circular left shifts
   int shift = round - (round <= 1) - (round <= 2) - (round <= 9) - (round <= 16);
   left = circularLeftShift(left, round);
   right = circularLeftShift(right, shift);

   // Combine then permute
   key = left;
   key = key << 26 | right;
   key = permute56to48(key);

   return key;
}

/*
------------------------------------------
Key and Mangler Support Functions(Level 3)
------------------------------------------
*/

// Gets rid of every 8th bit and reverses the order
unsigned long permute64to56(unsigned long x) {
   unsigned long ret = 0;

   for(int i = 0; i < 8; i++) {
      ret = ret << 7;
      ret |= x & 0x7f;

      x = x >> 8;
   }
   
   return ret;
}

// Gets rid of every 8th bit and reverses the order
// Needs to be tested
unsigned long permute56to48(unsigned long x) {
   unsigned long ret = 0;
   for(int i = 0; i < 7; i++) {
      ret = ret << 7;
      ret |= x & 0x7f;

      x = x >> 8;
   }
   
   return ret;
}

// Moves every odd bit to the left and every even bit to the right
unsigned int permute32to32(unsigned int x) {
   unsigned int left = 0, right = 0;

   for(int i = 0; i < 16; i++) {
      left = left << 1;
      right = right << 1;

      left |= x & 1;
      right |= x & 2;

      x = x >> 2;
   }
   return left << 16 | right;
}

unsigned int circularLeftShift(unsigned int x, int shift) {
   return (x << shift) | (x >> (32 - shift));
}

// Returns 48 bit value
// Selects first and last bit to get row and middle two bits for col
unsigned long sbox32to48(unsigned int x) {
   int row, col, tmp;
   unsigned long ret = 0;

   for(int i = 0; i < 8; i++) {
      tmp = x & 0xf; // Gets last 4 bits

      row = tmp >> 3 << 1 | tmp & 1; // Selects first and last bit
      col = tmp & 6 >> 1; // Selects middle two bits

      ret |= sbox[row][col];

      ret = ret << 6;
      x = x >> 4;
   }

   return ret;
}
// Returns 32 bit value
// Selects first and last bit to get row and middle four bits for col
unsigned int sbox48to32(unsigned long x) {
   int row, col, tmp;
   unsigned int ret = 0;

   for(int i = 0; i < 8; i++) {
      tmp = x & 0x3f; // Gets last 6 bits
      row = tmp >> 5 << 1 | tmp & 1; // Selects first and last bit
      col = tmp & 0x1e >> 1; // Selects middle 4 bits

      ret |= sbox[row][col];

      ret = ret << 4;
      x = x >> 6;
   }

   return ret;
}

/*
---------------------------------
General Helper Functions(Utility)
---------------------------------
*/

void ulong_to_char(unsigned long *ul_array, int ul_len, char **char_array, int *char_len) {
    // Calculate the length of the resulting char array
    *char_len = ul_len * sizeof(unsigned long);

    // Allocate memory for the char array
    *char_array = (char *)malloc(*char_len);
    if (*char_array == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Copy each unsigned long value to the char array byte-by-byte
    char *dest = *char_array;
    for (int i = 0; i < ul_len; i++) {
        unsigned long ul_value = ul_array[i];
        for (int j = 0; j < sizeof(unsigned long); j++) {
            *dest++ = (char)(ul_value & 0xFF); // Extract the least significant byte
            ul_value >>= 8; // Shift right to process the next byte
        }
    }
}

void char_to_ulong(char *char_array, int char_len, unsigned long **ul_array, int *ul_len) {
    // Calculate the length of the resulting unsigned long array
    *ul_len = (char_len + sizeof(unsigned long) - 1) / sizeof(unsigned long);

    // Allocate memory for the unsigned long array
    *ul_array = (unsigned long *)malloc(*ul_len * sizeof(unsigned long));
    if (*ul_array == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Copy bytes from char array to unsigned long array
    unsigned long *dest = *ul_array;
    for (int i = 0; i < char_len; i += sizeof(unsigned long)) {
        unsigned long ul_value = 0;
        for (int j = 0; j < sizeof(unsigned long); j++) {
            ul_value |= (unsigned long)(unsigned char)char_array[i + j] << (8 * j);
        }
        *dest++ = ul_value;
    }
}