#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdint.h>                     // Defines EXIT_FAILURE


// Fonction AddAB
int AddAB (int A, int B)
{
    return (A + B);

}

// FonctionA 
int FunctionA (int ValA, int ValB)
{
    int Res;
   Res =  AddAB(ValA, ValB);
}




void main() {
    int Res1, Res2;
    int V1 = 10;
    int V2 = 20;
    int V3 = 30;
    int V4 = 40;

     // Appel de la fonction  FuncionA
    Res1 = FunctionA(V1, V2);
    Res2 = FunctionA(V3, V4);
}
