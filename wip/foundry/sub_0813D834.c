#define SceneActor_SetEffectSlot sub_0813D834
#define gSceneActorEffectVariant gUnk_02000504
#include "global.h"
#include "engine.h"
struct SceneActorEffectSlots {
    u8 unknown0[0x444];
    u32 flags444;
    u8 unknown448[0x530-0x448];
    u16 slots[2];
    u8 unknown534[0x54A-0x534];
    s8 variant;
    u8 unknown54B[0x5B9-0x54B];
    u8 mode, unknown5BA, active;
};
extern u16 gUnk_02000504;
s32 Mod(s32, s32);
void sub_0813D4E0(void *);
static inline u32 TestMask(u32 *flags, u32 mask) { return *flags & mask; }
void sub_0813D834(struct SceneActorEffectSlots *p, s32 slot, s32 value)
{
    switch (slot) {
    case 0:
        if (p->active && p->mode != 3) sub_0813D4E0(p);
        break;
    case 1:
        if (!p || !TestMask(&p->flags444, 0x20000000)) {
            if (!p->slots[slot]) {
                u16 *variantPtr = &gUnk_02000504;
                u32 index = (gUnk_03005308 + 1) & 1023;
                s32 variant;
                gUnk_03005308 = index;
                variant = Mod(gUnk_0203B400[index], 7);
                variant = ((p->variant + 1) + variant) & 7;
                *variantPtr = variant;
                p->variant = variant;
                Sound_PlaySong(312);
            }
        }
        break;
    default: return;
    }
    p->slots[slot] = value;
}
