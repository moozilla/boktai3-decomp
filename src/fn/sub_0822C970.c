#include "global.h"
#include "runtime.h"
struct Obj {
    u8 pad[0x19];
    u8 state;
    u16 pad1a;
    s16 a, b;
    u16 count;
};
struct Context {
    u8 pad[0x874];
    s16 a, b;
};
extern u8 *gGameState;
void Solar_StartMeasurement(void), sub_0822C598(struct Obj *);
u16 sub_0822C8D8(u8 *);
s32 Solar_GetIntensity(void), Solar_GetLevelForIntensity(s32), Solar_ApplyIntensityModifiers(s32);
void sub_0822C970(struct Obj *obj)
{
    switch (obj->state) {
        case 0:
            if (++obj->count > 29) {
                Solar_StartMeasurement();
                obj->state = 1;
                obj->count = 0;
            }
            break;
        case 1:
            if (++obj->count > 59) {
                obj->state = 2;
                obj->count = 0;
            }
            break;
        case 2:
            obj->a = sub_0822C8D8((u8 *)obj);
            obj->b = Solar_GetLevelForIntensity(obj->a);
            (*(s16 *)(gGameState + 0x874)) = Solar_ApplyIntensityModifiers(obj->a);
            (*(s16 *)(gGameState + 0x876)) =
                Solar_GetLevelForIntensity((*(s16 *)(gGameState + 0x874)));
            sub_0822C598(obj);
            gSolarSavedLevel = (*(s16 *)(gGameState + 0x874));
            gSolarSavedCachedLevel = (*(s16 *)(gGameState + 0x876));
            break;
    }
}
