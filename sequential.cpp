#include "sequential.h"


Registro add_new()
{
    std::string codigo, nombre, carrera;
    int ciclo;
    cout << "codigo" << '\n';
    cin >> codigo;
    cout << "nombre" << '\n';
    cin >> nombre;
    cout << "carrera" << '\n';
    cin >> carrera;
    cout << "Ciclo" << '\n';
    cin >> ciclo;
    Registro nuevo(codigo, nombre, carrera, ciclo);
    return nuevo;
}

int main()
{
    sequential_ordered_file_structure sofs;
    std::vector<Registro> vectorito;
    for (int i = 0; i < 3; i ++)
    {
       vectorito.push_back(add_new());  
    }
    sofs.insertAll(vectorito);
    
    sofs.add(add_new());
    sofs.add(add_new());
    sofs.print();
    cout << sofs.search("2202").nombre;
    for (auto x : sofs.search("2202", "4404"))
    {
        cout << x.nombre;
    }
}