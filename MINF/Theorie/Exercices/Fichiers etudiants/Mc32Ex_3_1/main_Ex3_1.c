#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdint.h>                     // Defines EXIT_FAILURE

void main() {

   uint8_t i;
   bool bNegatif;
   char c;
   uint8_t code;

   int8_t  a = 7;
   int16_t b = 321;
   int32_t  d = 0x12345678;

   // addition 8 bits
   a = a + 0x31;

   // addition 16 bits
   b = b + 0x123;

   // addition 32 bits
  d = d + 0x10000;

   // If then else
   if (a >= 0) {
      bNegatif = 0;
   } else {
      bNegatif = 1;
   }

   // boucle for et switch
   for (i=0; i<6 ; i++) {
   
      switch (i) {
         case 1 : code = 'A' ; break;
         case 2 : code = 'B' ; break;
         case 3 : code = 'C' ; break;
         default : code = 'Z'; break;
      }
   }

   // boucle while
   i=10;
   while (i > 0 ) {
       i--;
   }
}
