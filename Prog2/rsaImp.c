#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void rsaEncrypt(int n, int e, long** m, int len);
void rsaDecrypt(int n, int d, long** c, int len);
void getKeys(int *p, int *q, int *e, int *d, int *n);
static int getPhi(int p, int q);
static int getD(int e, int phi);
static int getE(int d, int phi);
static void getPQ(int *p, int *q);
static int gcd(int a, int b);
static int isPrime(int n);
static long poMod(long base, long exp, long mod);


void rsaEncrypt(int n, int e, long** m, int len) {
   for(int i = 0; i < len; i++) {
      *(*m+i) = poMod(*(*m+i), e, n);
   }
}

void rsaDecrypt(int n, int d, long** c, int len) {
   for(int i = 0; i < len; i++) {
      *(*c+i) = poMod(*(*c+i), d, n);
   }
}

void getKeys(int *p, int *q, int *e, int *d, int *n) {
   getPQ(p, q);
   int phi = getPhi(*p, *q);
   *e = getE(*d, phi);
   *d = getD(*e, phi);
   *n = *p * *q;

   if(*e * *d%phi != 1) {
      printf("Error in getKeys\n");
   }
}

static int isPrime(int n) {
   for(int i = 2; i < n/2+1; i++) {
      if(n%i == 0) {
         return 0;
      }
   }
   return 1;
}

static int gcd(int a, int b) {
   if(b == 0) {
      return a;
   }
   return gcd(b, a%b);
}

static int getPhi(int p, int q) {
   return (p-1)*(q-1);
}

static int getE(int d, int phi) {
   for(int i = 2; i < phi; i++) {
      if(gcd(i, phi) == 1) {
         return i;
      }
   }
   return -1;
}

static int getD(int e, int phi) {
   for(int i = 2; i < phi; i++) {
      if((i*e) % phi == 1) {
         return i;
      }
   }
   return -1;
}

// Get the 10th and 19th prime numbers between 1000 and 10000
static void getPQ(int *p, int *q) {
   int count = 0;

   for(int i = 1000; i < 10000; i++) {
      if(isPrime(i)) {
         count++;
         if(count == 10) {
            *p = i;
         }
         if(count == 19) {
            *q = i;
            break;
         }
      }
   }
}

static long poMod(long base, long exp, long mod) {
   long result = 1;
   while(exp > 0) {
      if(exp % 2 == 1) {
         result = (result * base) % mod;
      }
      base = (base * base) % mod;
      exp /= 2;
   }
   return result;
}