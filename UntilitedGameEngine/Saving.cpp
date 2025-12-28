#include "Saving.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include "GLOBALS.h"

namespace fs = std::filesystem;

void Saving::SaveAll(const std::vector<Object>& Drawables)
{
    fs::path dir = appData + "\\UntilitedGameEngine\\Savings\\" + ProjectName;

    fs::create_directories(dir); // Luo kansiot automaattisesti

    fs::path filePath = dir / "World.json";

    std::ofstream myFile(filePath);

    if (!myFile.is_open())
        return;

    for (const Object& obj : Drawables)
    {
        myFile << "Name: " << obj.Name << "\n";
        myFile << "Anchored: " << obj.Anchored << "\n";
        myFile << "Selected: " << obj.Selected << "\n";

        myFile << "Position: "
            << obj.pos.x << " " << obj.pos.y << " " << obj.pos.z << "\n";

        myFile << "Velocity: "
            << obj.Velocity.x << " " << obj.Velocity.y << " " << obj.Velocity.z << "\n";

        myFile << "Orientation: "
            << obj.Orientation.x << " " << obj.Orientation.y << " " << obj.Orientation.z << "\n";

        myFile << "Color: "
            << obj.color.x << " " << obj.color.y << " " << obj.color.z << "\n";
        myFile << "MeshPath: " << obj.Path << "\n";

        myFile << "--------------------------\n";
    }

    myFile.close();
}

std::vector<Object> Saving::LoadDrawables(const std::string& path)
{
    std::vector<Object> Drawables;
    std::ifstream myFile(path);

    if (!myFile.is_open())
        return Drawables;

    std::string line;
    Object obj;

    while (std::getline(myFile, line))
    {
        if (line.rfind("Name:", 0) == 0)
        {
            obj.Name = line.substr(6);
        }
        else if (line.rfind("Anchored:", 0) == 0)
        {
            obj.Anchored = std::stoi(line.substr(10)) != 0;
        }
        else if (line.rfind("Selected:", 0) == 0)
        {
            obj.Selected = std::stoi(line.substr(10)) != 0;
        }
        else if (line.rfind("Position:", 0) == 0)
        {
            std::istringstream ss(line.substr(10));
            ss >> obj.pos.x >> obj.pos.y >> obj.pos.z;
        }
        else if (line.rfind("Velocity:", 0) == 0)
        {
            std::istringstream ss(line.substr(10));
            ss >> obj.Velocity.x >> obj.Velocity.y >> obj.Velocity.z;
        }
        else if (line.rfind("Orientation:", 0) == 0)
        {
            std::istringstream ss(line.substr(13));
            ss >> obj.Orientation.x >> obj.Orientation.y >> obj.Orientation.z;
        }
        else if (line.rfind("Color:", 0) == 0)
        {
            std::istringstream ss(line.substr(7));
            ss >> obj.color.x >> obj.color.y >> obj.color.z;
        }
        else if (line.rfind("MeshPath:", 0) == 0)
        {
            obj.Path = line.substr(10);
        }

        else if (line.rfind("--------------------------", 0) == 0)
        {
            Drawables.push_back(obj);
            obj = Object();
        }
    }

    if (!obj.Name.empty())
        Drawables.push_back(obj);

    return Drawables;
}
