#include "global.h"
#include "engine/runtime.h"
/* SaveData_RequestSave: Sets header byte 9 to one; tentatively requests saving */
static inline void set_flag(u32 value, u8 *ptr)
{
    ptr[9] = value;
}
void SaveData_RequestSave(void)
{
    set_flag(1, gSessionState);
}
