#include "global.h"
/* SaveData_GetSaveRequest: Returns header byte 9 paired with the tentative save request setter */
#define SaveData_GetSaveRequest sub_08227F4C
extern u8 *gUnk_030053F8;
u32 sub_08227F4C(void)
{
    return gUnk_030053F8[9];
}
