#include "global.h"

void Otenko_DispatchModeHandler(u32, u32);

void Otenko_UpdateMode(u32 a, u32 b)
{
    Otenko_DispatchModeHandler(a, b);
}
