#include "debug.h"
#include "hardware.h"

int main(void)
{
    SystemAndCoreClockUpdate();
    Delay_Init();

    Hardware();

    while (1)
    {
    }
}
