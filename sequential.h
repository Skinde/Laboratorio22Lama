#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <string>
#include <cstring>
#include <fstream>


#define PAGE_SIZE 4096

using namespace std;


struct Registro
{
    char codigo[5] = "";
    char nombre[20] = "";
    char carrera[15] = "";
    int ciclo;
    int next = -1;
    Registro()
    {
        
    }
    Registro(string codigo_input, string nombre_input, string carrera_input, int ciclo_input)
    {
        strcpy(codigo, codigo_input.c_str());
        strcpy(nombre, nombre_input.c_str());
        strcpy(carrera, carrera_input.c_str());
        ciclo = ciclo_input;

    }
    void print()
    {
        for (int i = 0; i < 5; i++)
        {
            cout << codigo[i];
        }
        cout << '\n';
        for (int i = 0; i < 20; i++)
        {
            cout << nombre[i];
        }
        cout << '\n';
        for (int i = 0; i < 15; i++)
        {
            cout << carrera[i];
        }
        cout << '\n';
    }
};

bool CompararRegistrosPorCodigo(const Registro &a, const Registro &b)
{
    return stoi(a.codigo) < stoi(b.codigo);
}


struct sequential_ordered_file_structure
{
    string file_path = "data.txt";
    vector<Registro> Ordered_Part;
    vector<Registro> Unordered_part;
    const int unordered_part_limit = (PAGE_SIZE/sizeof(Registro));

    //Rewrites the whole file 
    void write()
    {
        ofstream file;
        file.open(file_path);
        for (auto reg : Ordered_Part)
        {
            file << reg.codigo << reg.nombre << reg.carrera << reg.ciclo << reg.next;
        }
        cout << ":";
        for (auto reg : Unordered_part)
        {
            file << reg.codigo << reg.nombre << reg.carrera << reg.ciclo << reg.next;
        }
    }
    //Appends at the unordered part
    void append(Registro reg)
    {
        ofstream file;
        file.open(file_path, std::ios_base::app);
        file << reg.codigo << reg.nombre << reg.carrera << reg.ciclo;
    }
    //O(n)
    Registro search(string key)
    {
        //Performas a binary search over the ordered part O(log_2 n)
        int index = ceil((float)Ordered_Part.size()/2.0);
        int size_of_search = ceil((float)(Ordered_Part.size())/2.0);
        for (int repetitions = 0; repetitions < floor(log2 ((float)Ordered_Part.size())); repetitions++)
        {
            if (key == Ordered_Part[index].codigo)
            {
                return Ordered_Part[index];
            }
            else if (stoi(key) < stoi(Ordered_Part[index].codigo))
            {
                index = index - size_of_search;
            }
            else
            {
                index = index + size_of_search;
            }
            size_of_search/2;
        }
        //Its not in the ordered part search the unordered part O(n)
        Registro search = Ordered_Part[index];
        while (search.codigo != key && search.codigo != "-1")
        {
            if (search.next > Ordered_Part.size())
            {
                search = Unordered_part[search.next];
            }
            else
            {
                search = Ordered_Part[search.next - Ordered_Part.size()];
            }
            
        }
        return Ordered_Part[index];
        
    }



    void insertAll(vector<Registro> registros)
    {
        sort(registros.begin(), registros.end(), CompararRegistrosPorCodigo);
        if (!Ordered_Part.size())
        {
            Ordered_Part = registros;
            for (int i = 0; i < Ordered_Part.size() - 1; i++)
            {
                Ordered_Part[i].next = i + 1;
            }
        }
        else
        {
            for (auto nuevos : registros)
            {
                Ordered_Part.push_back(nuevos);
            }
            sort(Ordered_Part.begin(), Ordered_Part.end(), CompararRegistrosPorCodigo);
            for (int i = 0; i < Ordered_Part.size() - 1; i++)
            {
                Ordered_Part[i].next = i + 1;
            }
        }
        write();
    }


    void add(Registro registro)
    {
        if (Unordered_part.size() <= unordered_part_limit)
        {
            
            
            //Finds the prev pointer and the next pointer using an O(n) search
            int just_under = 0;
            int just_under_position;
            int just_over = 99999;
            int just_over_position;
            bool just_under_on_ordered_part = true;
            bool just_over_on_ordered_part = true;
            Unordered_part.push_back(registro);
            for (int i = 0; i < Ordered_Part.size(); i++)
            {
                if (stoi(Ordered_Part[i].codigo) > just_under && stoi(Ordered_Part[i].codigo) < stoi(registro.codigo))
                {
                    just_under = stoi(Ordered_Part[i].codigo);
                    just_under_position = i;
                }
                if (stoi(Ordered_Part[i].codigo) < just_over && stoi(Ordered_Part[i].codigo) > stoi(registro.codigo))
                {
                    just_over = stoi(Ordered_Part[i].codigo);
                    just_over_position = i;
                }
            }
            for (int i = 0; i < Unordered_part.size(); i++)
            {
                if (stoi(Unordered_part[i].codigo) > just_under && stoi(Unordered_part[i].codigo) < stoi(registro.codigo))
                {
                    just_under = stoi(Unordered_part[i].codigo);
                    just_under_position = i;
                    just_under_on_ordered_part = false;
                }
                if (stoi(Unordered_part[i].codigo) < just_over && stoi(Unordered_part[i].codigo) > stoi(registro.codigo))
                {
                    just_over = stoi(Unordered_part[i].codigo);
                    just_over_position = i;
                    just_over_on_ordered_part = false;
                }
            }
            if (just_under_on_ordered_part)
            {
                Ordered_Part[just_under_position].next = Ordered_Part.size() + Unordered_part.size() -1;
            }
            else
            {
                Unordered_part[just_under_position].next = Ordered_Part.size() + Unordered_part.size() -1;
            }
            if (just_over_on_ordered_part)
            {
                Unordered_part[Unordered_part.size() -1].next = just_under_position + 1;
            }
            else
            {
               Unordered_part[Unordered_part.size() - 1].next = Ordered_Part.size() + just_under_position + 1; 
            }
            registro.next = Ordered_Part.size() + just_under_position + 1;
        }
        else
        {
            Unordered_part.push_back(registro);
            insertAll(Unordered_part);
            Unordered_part.clear();
        }

        append(registro);
        
    }

    vector<Registro> search(string begin, string end)
    {
        vector<Registro> vectorito;
        Registro index = search(begin);
        while(stoi(index.codigo) < stoi(end))
        {
            vectorito.push_back(index);
            if (index.next < Ordered_Part.size())
            {
                index = Ordered_Part[index.next];
            }
            else
            {
                index = Unordered_part[index.next - Ordered_Part.size()];
            }
        }
        return vectorito;
    }
    

    //For debuggin purposes
    void print()
    {
        cout << "Ordered Part" << '\n';
        for (int i = 0; i < Ordered_Part.size(); i++)
        {
            cout << Ordered_Part[i].codigo << " " << Ordered_Part[i].nombre << " " << Ordered_Part[i].carrera << " " << Ordered_Part[i].ciclo << " " << Ordered_Part[i].next << '\n'; 
        }
        cout << "Unordered Part" << '\n';
        for (int i = 0; i < Unordered_part.size(); i++)
        {
            cout << Unordered_part[i].codigo << " " << Unordered_part[i].nombre << " " << Unordered_part[i].carrera << " " << Unordered_part[i].ciclo << " " << Unordered_part[i].next << '\n';
        }
    }
};