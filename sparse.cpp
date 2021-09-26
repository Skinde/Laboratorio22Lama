#include "sparse.h"

int main()
{
    sparse sparse_file;
    //This (should) trigger buffer overflow
    for (int i=1; i < 100; i++)
    {
        Registro reg2(to_string(3303*i),"Miguel", "CS",5);
        Registro reg3(to_string(4404*i),"Jesus","CS",5);
        sparse_file.add(reg2);
        sparse_file.add(reg3);
    }

    cout << sparse_file.search("3303").begin()->nombre;
     
    
}