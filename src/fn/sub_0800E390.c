#include "palette_set.h"
void *sub_0800E390(u32 arg)
{
    struct PaletteSet *obj;
    obj = Task_Create(12, 0x2C8);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void *)PaletteSet_Update, (void *)sub_0800E228);
        if (PaletteSet_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
