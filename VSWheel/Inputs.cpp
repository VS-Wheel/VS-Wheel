#include "Inputs.h"

uint8_t ped_getValue(uint16_t value)
{   
    return ((value / PEDDIV) - PEDZERO);
}