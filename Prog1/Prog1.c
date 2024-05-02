#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "des.h"

void testEncDec() {
   unsigned long key = 0xffefcdab89674523;
   unsigned long m = 0x0123356789abcd;

   printf("key: 0x%lx, m: 0x%lx\n", key, m);

   encrypt(key, &m);
   printf("Encrypted: 0x%lx\n", m);

   decrypt(key, &m);
   printf("Decrypted: 0x%lx\n", m);
}

void testEncDecMsg() {
   unsigned long key = 0x2342fe1232432fe1;
   char* msg = "";
   unsigned long *encrypted;

   // Display
   printf("key: 0x%lx, len: %ld\n", key, strlen(msg));
   printf("Original Msg:\n   ");
   for(int i = 0; i < strlen(msg); i++) {
      printf("%x", msg[i]);
   }
   printf("\n   %s\n", msg);
   
   // Encrypt
   // encrypted = encryptmsg(msg, key, strlen(msg));
   encrypted = encryptmsg(msg, key, strlen(msg));
   
   // Display
   printf("Encrypted msg: \n   ");
   for(int i = 0; i < strlen(msg)/8+1; i++) {
      printf("%lx", encrypted[i]);
   }
   printf("\n   %s\n", encrypted);

   // Decrypt
   msg = decryptmsg(encrypted, key, strlen(msg));

   // Display
   printf("Decrypted Msg: \n   ");
   for(int i = 0; i < strlen(msg); i++) {
      printf("%x", msg[i]);
   }
   printf("\n   %s\n", msg);

}

#include <stdio.h>
#include <stdlib.h>

char* read_text_file(const char* filename) {
   FILE* file = fopen(filename, "rb");
   if (file == NULL) {
      fprintf(stderr, "Error opening file %s\n", filename);
      exit(EXIT_FAILURE);
   }

   // Determine the size of the file
   fseek(file, 0, SEEK_END);
   long file_size = ftell(file);
   rewind(file);

   // Allocate memory for the buffer to hold the file contents
   char* buffer = (char*)malloc((file_size + 1) * sizeof(char));
   if (buffer == NULL) {
      fprintf(stderr, "Memory allocation failed\n");
      exit(EXIT_FAILURE);
   }

   // Read the file contents into the buffer
   fread(buffer, sizeof(char), file_size, file);
   buffer[file_size] = '\0'; // Null-terminate the buffer

   // Close the file
   fclose(file);

   return buffer;
}

unsigned long* read_binary_file(const char* filename, int* array_length) {
   FILE* file = fopen(filename, "rb");
   if (file == NULL) {
      fprintf(stderr, "Error opening file %s\n", filename);
      exit(EXIT_FAILURE);
   }

   // Determine the size of the file
   fseek(file, 0, SEEK_END);
   long file_size = ftell(file);
   rewind(file);

   // Calculate the number of unsigned longs needed
   *array_length = (file_size + sizeof(unsigned long) - 1) / sizeof(unsigned long);

   // Allocate memory for the array
   unsigned long* array = (unsigned long*)malloc(*array_length * sizeof(unsigned long));
   if (array == NULL) {
      fprintf(stderr, "Memory allocation failed\n");
      exit(EXIT_FAILURE);
   }

   // Read the file contents into the array
   fread(array, sizeof(unsigned long), *array_length, file);

   // Close the file
   fclose(file);

   return array;
}

void save_to_binary_file(const char* filename, unsigned long* array, int array_length) {
   FILE* file = fopen(filename, "wb");
   if (file == NULL) {
      fprintf(stderr, "Error opening file %s\n", filename);
      exit(EXIT_FAILURE);
   }

   // Write the contents of the array to the file
   fwrite(array, sizeof(unsigned long), array_length, file);

   // Close the file
   fclose(file);
}

void save_to_text_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Write the content to the file
    fprintf(file, "%s", content);

    // Close the file
    fclose(file);
}

int main() {
   int input = 0, array_length;
   char filename[100];
   unsigned long* encrypted_text;
   char *decrypted_text, *text_content;
   

   printf("0: Exit\n1: Encrypt\n2: Decrypt\nInput: ");

   scanf("%d", &input);
   printf("\n");
   switch(input) {
      case 0:
         return 0;
      case 1:
         // Get the filename of the text to encrypt
         printf("File to Encrypt: ");
         scanf("%s", filename);
         printf("\n");

         text_content = read_text_file(filename);

         // Encrypt the text content
         encrypted_text = encryptmsg(text_content, 0x12345678, strlen(text_content));

         // Save the encrypted text to a binary file
         save_to_binary_file("encrypted.bin", encrypted_text, strlen(text_content));
         printf("Encrypted text saved to encrypted.bin\n");
         
         free(text_content);
         break;
      case 2:
         // Get the filename of the encrypted text
         printf("File to Decrypt: ");
         scanf("%s", filename);
         printf("\n");

         // Read the encrypted text from a binary file
         encrypted_text = read_binary_file("encrypted.bin", &array_length);

         // Decrypt the encrypted text
         decrypted_text = decryptmsg(encrypted_text, 0x12345678, array_length);

         // Get the filename to save the decrypted text
         printf("File to Save Decrypted Text: ");
         scanf("%s", filename);
         printf("\n");

         // Save the decrypted text to a text file
         save_to_text_file(filename, decrypted_text);

         free(decrypted_text);
         break;
   }

   return 0;
}
