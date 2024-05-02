#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsaImp.h"
#include <time.h> 

long* preProcessing(char* m, int len);
void print(long* m, int len);
int bruteforceD(int e, int n);

int main(int argc, char *argv[]) {
   long *m;
   int p, q, e, d, n;
   int msgLen;

   if(argc != 2) {
      printf("Usage: %s <message>\n", argv[0]);
      exit(1);
   }

   // Get message
   msgLen = strlen(argv[1]);
   printf("Message: \"%s\", len: %d\n", argv[1], msgLen);

   // Get public and private keys
   getKeys(&p, &q, &e, &d, &n);
   printf("p: %d, q: %d, e: %d, d: %d, n: %d\n", p, q, e, d, n);

   // Preprocessing
   m = malloc(msgLen*sizeof(long));
   m = preProcessing(argv[1], msgLen);
   printf("Original: ");
   print(m, msgLen);

   // Encryption
   rsaEncrypt(n, e, &m, msgLen);
   printf("Encrypted: ");
   print(m, msgLen);

   // Decryption
   rsaDecrypt(n, d, &m, msgLen);
   printf("Decrypted: ");
   print(m, msgLen);

   // Brute force
   clock_t start = clock();
   d = bruteforceD(e, n);
   printf("Brute force d: %d\n", d);
   printf("Time for brute force: %f\n", (double)(clock() - start)/CLOCKS_PER_SEC);

   // Encrypt for brute force
   rsaEncrypt(n, e, &m, msgLen);

   // Decrypt with brute force
   rsaDecrypt(n, d, &m, msgLen);
   printf("Decrypted with brute force: ");
   print(m, msgLen);

   // Cleanup
   free(m);

   return 0;

}

// Tales ~500-700 seconds on my comp
int bruteforceD(int e, int n) {
   for(int d = 2; d < n; d++) {
      for(int p = 2; p < n/2+1; p++) {
         if(d % (n/10) == 0 && p == 2) {
            printf("coverage : %.2f%%\n", ((float)d)/n*100);
         }

         if(n%p == 0) {
            int q = n/p;
            int phi = (p-1)*(q-1);

            if((d*e) % phi == 1) {
               return d;
            }
         }
      }
   }
   printf("\n");
   return -1;
}

long* preProcessing(char* m, int len) {
   long *ret = malloc(len * sizeof(int));

   for(int i = 0; i < len; i++) {
      if(m[i] < 'a' || m[i] > 'z') {
         printf("Invalid character: %c\n", m[i]);
         exit(1);
      }
      ret[i] = m[i]-'a';
   }

   return ret;
}

void print(long* m, int len) {
   printf("[");
   for(int i = 0; i < len-1; i++) {
      printf("%ld, ", m[i]);
   }
   if(len > 0) printf("%ld", m[len-1]);
   printf("]\n");
}