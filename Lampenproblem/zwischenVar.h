#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <gmpxx.h>

#define saveVar(var) saveVariable(std::string(#var), var)
#define readVar(var) var = readVariable(std::string(#var), var)

inline void saveVariable(std::string dateiname, mpz_class zahl)
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

inline mpz_class readVariable(std::string dateiname, mpz_class useless_var)
{
    mpz_class zahl;

    FILE* file = fopen(dateiname.c_str(), "rb");
    if (!file) throw std::runtime_error("Fehler beim Laden einer Variable: Fehler beim Öffnen der Datei zum Lesen.");        return 0;

    mpz_inp_raw(zahl.get_mpz_t(), file);

    fclose(file);

    return zahl;
}

inline void saveVariable(std::string dateiname, std::string var)
{
    std::ofstream outFile(dateiname, std::ios::trunc | std::ios::out);
    if (outFile.is_open())
    {
        outFile << var << std::flush;
        outFile.close();
    }
    else std::cerr << "Fehler beim Öffnen der Datei zum Speichern: " << dateiname << std::endl;
}

inline std::string readVariable(std::string dateiname, std::string useless_var)
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

inline void saveVariable(std::string dateiname, uint64_t var)
{
    std::ofstream outFile(dateiname, std::ios::binary | std::ios::out);
    if (outFile.is_open())
    {
        outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
        outFile.close();
    }
    else std::cerr << "Fehler beim Öffnen der Datei zum Speichern: " << dateiname << std::endl;
}

inline uint64_t readVariable(std::string dateiname, uint64_t useless_var)
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
