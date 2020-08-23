#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include  <cstring>

#include "logger.h"

using namespace std;

/**
*
*/
int main (int argc, char* argv[])
{
    bool success = true;

    std::cout << "Now : " << logger::get_time_ms() << " " << logger::get_time() << std::endl;
    sleep(2);
    std::cout << "Now : " << logger::get_time_ms() << " " << logger::get_time() << std::endl;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}