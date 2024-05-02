#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "des.h"

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

int main(int argc, char* argv[]) {
   int input = 0, array_length;
   char filename[100];
   unsigned long* encrypted_text;
   char *decrypted_text, *text_content;
   

   // Get the filename of the text to encrypt
   if(argc != 2) {
      printf("Proper usage: %s <filename>\n", argv[0]);
      return 1;
   }

   strcpy(filename, argv[1]);

   // Read the encrypted text from a binary file
   encrypted_text = read_binary_file(filename, &array_length);

   // Decrypt the encrypted text
   decrypted_text = decryptmsg(encrypted_text, 0x1234567890abcdef, array_length);

   // Save the decrypted text to a text file
   save_to_text_file("out.txt", decrypted_text);
   printf("Decrypted text saved to out.txt\n");

   free(decrypted_text);

   return 0;
}
