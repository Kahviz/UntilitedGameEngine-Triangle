#include "Saving.h"
#include <fstream>  // Tiedostojen käsittelyä varten


void Saving::SaveAll(std::vector<Object> Drawables)
{
    std::ofstream myFile("C:\\AppData\\Roaming\\UntilitedGameEngine\\Esimerkki.txt");

    if (myFile.is_open()) { 
        myFile << "Hei maailma!\n"; 
        myFile << "Tämä on toinen rivi.\n";

        myFile.close();  
    }
}
