#include "global.h"

#define Inventory_SetEntryFromScript sub_0822DC24
#include "engine.h"
s32 sub_0822D6E0(s32);
void sub_0822D83C(s32, const void *);
extern const u8 gUnk_08E6092C[][32];
/* Script: finds item -f among the first 16 entries and gives it template -t
   (gUnk_08E6092C, 32 bytes each); 1 if found (tentative). */
u32 Inventory_SetEntryFromScript(void)
{
    s32 id, t, i;
    if (!Script_SeekToKeyword('f'))
        return 0;
    id = Script_GetValue();
    if (!Script_SeekToKeyword('t'))
        return 0;
    t = Script_GetValue();
    for (i = 0; i <= 15; i++) {
        if (sub_0822D6E0(i) == id) {
            sub_0822D83C(i, gUnk_08E6092C[t]);
            return 1;
        }
    }
    return 0;
}
