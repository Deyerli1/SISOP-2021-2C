#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char **argv)
{
    if(argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        cout << "Bienvenido al juego de palabras Autodefinido. En esta" << endl <<
        "oportunidad usted debe inferir la palabra de la que se trata" << endl <<
        "en base a una definicion que se muestre en pantalla" << endl <<
        "Por ejemplo:" << endl <<
        "Si la definicion es: El que hace pan" << endl <<
        "La palabra en cuestion es: Panadero"  << endl;

        return EXIT_SUCCESS;
    }    

    system("./servidor");
    return EXIT_SUCCESS;
}