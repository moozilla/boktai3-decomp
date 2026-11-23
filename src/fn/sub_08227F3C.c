#include "global.h"
#include "runtime.h"
/* SaveData_RequestSave: Sets header byte 9 to one; tentatively requests saving */
#define SaveData_RequestSave sub_08227F3C
static inline void set_flag(u32 value, u8 *ptr)
{
    ptr[9] = value;
}
void sub_08227F3C(void)
{
    set_flag(1, gSessionState);
}
