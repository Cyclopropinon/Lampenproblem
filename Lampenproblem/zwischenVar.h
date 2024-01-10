#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <gmpxx.h>

#define saveVar(var) saveVariable(std::string(#var), var)
#define readVar(var) var = readVariable(std::string(#var), var)

void saveVariable(std::string dateiname, mpz_class zahl)
{
    FILE* file = fopen(dateiname.c_str(), "wb");
    if (!file)
	{
        std::cerr << "Fehler beim Öffnen der Datei zum Speichern: " << dateiname << std::endl;
        return;
    }

    mpz_out_raw(file, zahl.get_mpz_t());

    fclose(file);
}

mpz_class readVariable(std::string dateiname, mpz_class useless_var)
{
    mpz_class zahl;

    FILE* file = fopen(dateiname.c_str(), "rb");
    if (!file) throw std::runtime_error("Fehler beim Laden einer Variable: Fehler beim Öffnen der Datei zum Lesen.");        return 0;

    mpz_inp_raw(zahl.get_mpz_t(), file);

    fclose(file);

    return zahl;
}

void saveVariable(std::string dateiname, std::string var)
{
    std::ofstream outFile(dateiname, std::ios::trunc | std::ios::out);
    if (outFile.is_open())
    {
        outFile << var << std::flush;
        outFile.close();
    }
    else std::cerr << "Fehler beim Öffnen der Datei zum Speichern: " << dateiname << std::endl;
}

std::string readVariable(std::string dateiname, std::string useless_var)
{
    std::ifstream inFile(dateiname, std::ios::in);
    if (inFile.is_open())
    {
        std::string var((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        return var;
    }
    else throw std::runtime_error("Fehler beim Laden einer Variable: Fehler beim Öffnen der Datei zum Lesen.");
}

void saveVariable(std::string dateiname, uint64_t var)
{
    std::ofstream outFile(dateiname, std::ios::binary | std::ios::out);
    if (outFile.is_open())
    {
        outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
        outFile.close();
    }
    else std::cerr << "Fehler beim Öffnen der Datei zum Speichern: " << dateiname << std::endl;
}

uint64_t readVariable(std::string dateiname, uint64_t useless_var)
{
    std::ifstream inFile(dateiname, std::ios::in);
    if (inFile.is_open())
    {
        uint64_t var;
        inFile.read(reinterpret_cast<char*>(&var), sizeof(var));
        inFile.close();
        return var;
    }
    else throw std::runtime_error("Fehler beim Laden einer Variable: Fehler beim Öffnen der Datei zum Lesen.");
}

void saveVariable(std::string dateiname, std::vector<mpz_class> var)
{
    const uint64_t size = var.size();
    const std::string dir = dateiname + "/";
    saveVariable(dir + "size", size);
    for (uint64_t i = 0; i < size; i++)
    {
        saveVariable(dir + std::to_string(i), var[i]);
    }
}

std::vector<mpz_class> readVariable(std::string dateiname, std::vector<mpz_class> useless_var)
{
    const std::string dir = dateiname + "/";
    uint64_t size = readVariable(dir + "size", (uint64_t)1729);
    std::vector<mpz_class> var;
    var.reserve(size);
    for (uint64_t i = 0; i < size; i++)
    {
        var.push_back(readVariable(dir + std::to_string(i), mpz_class{}));
    }
    return var;
}

void saveVariable(std::string dateiname, bool var)
{
    saveVariable(dateiname, (uint64_t)var);
}

bool readVariable(std::string dateiname, bool useless_var)
{
    return readVariable(dateiname, (uint64_t)1729) != 0;
}

void saveVariable(const std::string &dateiname, const std::vector<bool> &var)
{
    std::ofstream outputFile(dateiname, std::ios::binary);

    if (!outputFile.is_open())
    {
        std::cerr << "Fehler beim Öffnen der Datei: " << dateiname << std::endl;
        return;
    }

    // Schreibe die Größe des Vektors in die Datei
    size_t size = var.size();
    outputFile.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

    // Schreibe die Daten des Vektors in die Datei
    for (bool bit : var)
    {
        outputFile.write(reinterpret_cast<const char*>(&bit), sizeof(bool));
    }

    outputFile.close();
}

std::vector<bool> readVariable(const std::string &dateiname, const std::vector<bool> &useless_var)
{
    std::ifstream inputFile(dateiname, std::ios::binary);

    if (!inputFile.is_open())
    {
        std::cerr << "Fehler beim Öffnen der Datei: " << dateiname << std::endl;
        return {};
    }

    // Lese die Größe des Vektors aus der Datei
    size_t size;
    inputFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));

    // Lese die Daten des Vektors aus der Datei
    std::vector<bool> result;
    result.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        bool bit;
        inputFile.read(reinterpret_cast<char*>(&bit), sizeof(bool));
        result.push_back(bit);
    }

    inputFile.close();
    return result;
}

void saveVariable(std::string dateiname, int var)
{
    std::ofstream outFile(dateiname, std::ios::binary | std::ios::out);
    if (outFile.is_open())
    {
        outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
        outFile.close();
    }
    else std::cerr << "Fehler beim Öffnen der Datei zum Speichern: " << dateiname << std::endl;
}

int readVariable(std::string dateiname, int useless_var)
{
    std::ifstream inFile(dateiname, std::ios::in);
    if (inFile.is_open())
    {
        int var;
        inFile.read(reinterpret_cast<char*>(&var), sizeof(var));
        inFile.close();
        return var;
    }
    else throw std::runtime_error("Fehler beim Laden einer Variable: Fehler beim Öffnen der Datei zum Lesen.");
}


/*
template <typename SaveType>
auto readVariable(const string dateiname)
{
    using BaseType = typename std::remove_reference<SaveType>::type;

    if constexpr (std::is_same_v<BaseType, mpz_class>)
    {
        mpz_class var = load_mpz(dateiname);
        return var;
    }
    else if constexpr (std::is_same_v<BaseType, std::vector<typename BaseType::value_type>>)
    {
        std::vector<typename BaseType::value_type> var;
        size_t i = 0;

        while (true)
        {
            string elementDateiname = dateiname + "/" + to_string(i);
            auto element = readVariable<typename BaseType::value_type>(elementDateiname);

            if (!element.empty()) // Überprüfe, ob das Element vorhanden ist
            {
                var.push_back(element);
                i++;
            }
            else
            {
                break; // Stoppe die Schleife, wenn kein weiteres Element vorhanden ist
            }
        }

        return var;
    }
    else
    {
        BaseType var;
        ifstream inFile(dateiname, ios::binary | ios::in);
        if (inFile.is_open())
        {
            inFile.read(reinterpret_cast<char*>(&var), sizeof(var));
            inFile.close();
            return var;
        }
        else
        {
            cerr << "Fehler beim Öffnen der Datei zum Lesen." << endl;
            // Rückgabe eines leeren Objekts oder eine geeignete Ausnahmebehandlung
            return BaseType{};
        }
    }
}

template <typename SaveType>
void saveVariable(const string dateiname, const SaveType& var)
{
    using BaseType = typename std::remove_reference<SaveType>::type;

    if constexpr (std::is_same_v<BaseType, mpz_class>)
    {
        save_mpz(var, dateiname);
    }
    else if constexpr (std::is_same_v<BaseType, std::vector<typename BaseType::value_type>>)
    {
        for (size_t i = 0; i < var.size(); ++i)
        {
            saveVariable(dateiname + "/" + to_string(i), var[i]);
        }
    }
    else
    {
        ofstream outFile(dateiname, ios::binary | ios::out);
        if (outFile.is_open())
        {
            outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
            outFile.close();
        }
        else
        {
            cerr << "Fehler beim Öffnen der Datei zum Speichern." << endl;
        }
    }
}
*/
