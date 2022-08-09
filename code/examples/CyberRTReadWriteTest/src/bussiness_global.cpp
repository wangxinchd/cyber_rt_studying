#include "bussiness_global.h"
#include "cyber/cyber.h"

void initCyber(const char* pModule)
{
    if (!pModule)
    {
        return;
    }

    // init cyber framework
    apollo::cyber::Init(pModule);

    return;
}
