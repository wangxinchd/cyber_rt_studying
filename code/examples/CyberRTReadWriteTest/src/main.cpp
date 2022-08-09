#include <iostream>
#include <string>
#include "bussiness_global.h"
#include "bussiness_reader.h"
#include "bussiness_writer.h"


int main(int argc, char *argv[])
{
    std::cout << "Started..." << std::endl;    

    int type = 0;
    if (argc >= 2)
    {
        if (strcmp(argv[1], "writer") == 0)
        {
            type = 1;
        }
        else if (strcmp(argv[1], "reader") == 0)
        {
            type = 2;
        }
    }

    if (type == 0)
    {
        std::cout << "Error: Incorrect arguments." << std::endl;
        std::cout << "Usage: " << std::endl << std::endl;
        std::cout << argv[0] << " writer|reader" << std::endl << std::endl;
        return 0;
    }

    initCyber(argv[0]);

    switch (type)
    {
        case 1:
        {
            BussinessWriter writer(1.0);
            writer.init();
            writer.run();
            break;
        }
        case 2:
        {
            BussinessReader reader;
            reader.init();
            reader.run();
            break;
        }
    }

    std::cout << "Stoped..." << std::endl;

    return 0;
}
