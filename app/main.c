#include "Sys.h"
#include "SysTimer.h"

int main(void)
{
    SysInitialize();
    SysStatusLedSet(3);

    while(1)
    {
        SysPoll();
    }
}

