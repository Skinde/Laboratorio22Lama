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

bool OrdenarRegistro(Registro const & a, Registro const & b)
{
    return atoi(a.codigo) < atoi(b.codigo);
}


struct sparse{
    string file_path1 = "index1.dat";
    string file_path2 = "index2.dat";
    string file_path3 = "index3.dat";
    string overflow_path = "overflow.dat";

    void write_index(vector<std::pair<int,int>> data, string path)
    {
        ofstream file;
        file.open(path, ios::app);
        for (auto x : data)
        {
            file << to_string(x.first) << ':' << to_string(x.second) << ':';
        }
    }

    vector<std::pair<int,int>> read_index_bucket(string path, int position)
    {
        ifstream file;
        file.open(path);
        string addrs;
        string max;
        vector<std::pair<int,int>> output;
        file.seekg(position);
        for (int i = 0; i < PAGE_SIZE/sizeof(Registro) && getline(file,addrs,':');i++)
        {
            getline(file,max,':');
            std::pair<int,int> nuevo_par(stoi(addrs),stoi(max));
            output.push_back(nuevo_par);
        }
        return output;
    }

    vector<Registro> read_bucket(string path, int position)
    {
        ifstream file;
        file.open(path);
        Registro current;
        vector<Registro> output;
        file.seekg(position);
        for (int i = 0; i < PAGE_SIZE/sizeof(Registro) && file.read((char *)&current, sizeof(current)) ; i++)
        {
            output.push_back(current);
        }
        return output;
    }

    void write_bucket(string file_path, int position, vector<Registro> vectorito)
    {
        ofstream file;
        file.open(file_path);
        file.seekp(position);
        for (auto reg : vectorito)
        {
            file.write((char *) & reg, sizeof(reg));
        }
    }

    int find_bucket(string key)
    {
        vector<std::pair<int,int>> current_bucket;
        current_bucket = read_index_bucket(file_path1, 0);
        int next_place = 0;
        for (auto p : current_bucket)
        {
            if (p.second > stoi(key))
            {
                break;
            }
            next_place = p.first;
        }

        

        //Read second index
        int next_place2 = 0;
        current_bucket = read_index_bucket(file_path2, next_place2);
        for (auto l : current_bucket)
        {
            if (l.second > stoi(key))
            {
                break;
            }
            next_place2 = l.first;
        }
        return next_place2;
    }

    int get_size(string path)
    {
        ifstream file;
        file.open(path);
        Registro reg;
        int i = 0;
        while(file.read((char *) & reg, sizeof(Registro)))
        {
            i++;
        }
        return i;
    }

    int get_size_index(string path)
    {
        ifstream file;
        file.open(path);
        Registro reg;
        int i = 0;
        string no_importa;
        while(getline(file,no_importa,':'))
        {
            getline(file,no_importa,':');
            i++;
        }
        return i;
    }

    void add(Registro registro)
    {

        //Read first index
        vector<std::pair<int,int>> current_bucket;
        current_bucket = read_index_bucket(file_path1, 0);
        int next_place = 0;
        for (auto p : current_bucket)
        {
            if (p.second > atoi(registro.codigo))
            {
                break;
            }
            next_place = p.first;
        }

        

        //Read second index
        
        current_bucket = read_index_bucket(file_path2, next_place);
        int next_place2 = 0;
        for (auto l : current_bucket)
        {
            if (l.second > atoi(registro.codigo))
            {
                break;
            }
            next_place2 = l.first;
        }

        //Read third index (data)        
        vector<Registro> bucket;
        bucket = read_bucket(file_path3, next_place2);
        
        //Check if register is full
        if (get_size(file_path3) >= PAGE_SIZE/sizeof(Registro))
        {
            //Check if Register.code > last element of bucket, if it is create a new bucket. if it isn't add to the overflow
            if (atoi(registro.codigo) > atoi(bucket.end()->codigo))
            {
                //Check if index2's current bucket is full
                if (get_size_index(file_path2) >= PAGE_SIZE/(sizeof(int) + sizeof(char) + sizeof(int)))
                {
                    //Add a new bucket to index1
                    ofstream index1;
                    index1.open(file_path1, ios::app);
                    index1 <<  next_place2 + (sizeof(int) + sizeof(char) + sizeof(int)) * current_bucket.size()  << ':' << registro.codigo << ':'; 
                }
                ofstream index2;
                index2.open(file_path2, ios::app);
                index2 << next_place2 + (sizeof(Registro) * bucket.size()) << ':' << registro.codigo << ':';
            }
            else
            {
                //Add to the overflow bucket
                ofstream overflow;
                overflow.open(overflow_path,ios::app);
                overflow.write((char *) & registro, sizeof(registro));
            }
            
        }
        else
        {
            //Push and write
            bucket.push_back(registro);
            sort(bucket.begin(), bucket.end(), OrdenarRegistro);
            write_bucket(file_path3, next_place, bucket);
        }
    }
    vector<Registro> search(string key)
    {

        //Read first index
        vector<std::pair<int,int>> current_bucket;
        current_bucket = read_index_bucket(file_path1, 0);
        int next_place = 0;
        for (auto p : current_bucket)
        {
            if (p.second > stoi(key))
            {
                break;
            }
            next_place = p.first;
        }

        

        //Read second index
        int next_place2 = 0;
        current_bucket = read_index_bucket(file_path2, next_place2);
        for (auto l : current_bucket)
        {
            if (l.second > stoi(key))
            {
                break;
            }
            next_place2 = l.first;
        }

        

        //Read third index (data)        
        vector<Registro> bucket;
        bucket = read_bucket(file_path3, next_place2);
        vector<Registro> output;
        for (auto algo : bucket)
        {
            if (algo.codigo == key)
            {
                output.push_back(algo);
            }
        }
        return output;

    }

    
    vector<Registro> search(string begin_key, string end_key)
    {
        int starting_bucket_location = 0;
        int ending_bucket_location = 0;

        starting_bucket_location = find_bucket(begin_key);
        ending_bucket_location = find_bucket(end_key);

        vector<Registro> output;
        for (int i = starting_bucket_location; i <= ending_bucket_location; i+=PAGE_SIZE/sizeof(Registro))
        {
            vector<Registro> current_bucket = read_bucket(file_path3, i);
            for (auto reg : current_bucket)
            {
                if (atoi(reg.codigo) >= stoi(begin_key) && atoi(reg.codigo) <= stoi(end_key)) 
                {
                    output.push_back(reg);
                }
            }
        }

        return output;


        
    }
    
};