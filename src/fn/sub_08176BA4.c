#include "global.h"
extern u16 gUnk_03004BD8, gUnk_03005214, gUnk_030051C4, gUnk_030051DC, gUnk_03005210;
extern u32 gUnk_0300523C;
void Hud_InitSelectionBounds(u32, u32);
void Hud_UpdateEquipmentPrompt(void *);
void Hud_SetUpdateCallback(void *, void (*)(void *));
void Sound_ResumePersistentSongs(void);
void HealthBurstEffect_Create(void *, u32);
struct Args {
    u16 count;
    u16 pad;
    s32 *values;
};
void Script_Call(void *, struct Args *);
struct PackedVec {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
void EffectManager_SpawnParticles(u32, u32, u32, struct PackedVec *, struct PackedVec *, u32, u32);
static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= mask;
}
void SceneHud_FinishPageExit(u8 *p)
{
    u32 zero;
    s32 value;
    struct Args args;
    struct PackedVec pos, size;
    s32 value2;
    struct Args args2;
    u16 *timer = (u16 *)(p + 0xa4c);
    Hud_InitSelectionBounds(*timer * 4, 0x1084);
    if (!*timer) {
        *(vu16 *)0x04000050 = *(u16 *)(p + 0xa10);
        if (*(u16 *)(p + 0x43d8) & 0x200)
            gUnk_03004BD8 |= 0x200;
        if (*(u16 *)(p + 0x43d8) & 0x400)
            gUnk_03004BD8 |= 0x400;
        if (*(u16 *)(p + 0x43d8) & 0x800)
            gUnk_03004BD8 |= 0x800;
    }
    {
        u32 next = *(u16 *)(p + 0xa4c) + 1;
        zero = 0;
        *(u16 *)(p + 0xa4c) = next;
        if ((u16)next > 16) {
            Hud_SetUpdateCallback(p, Hud_UpdateEquipmentPrompt);
            gUnk_03005214 = *(u16 *)(p + 0x43e0);
            gUnk_030051C4 = *(u16 *)(p + 0x43e2);
            gUnk_030051DC = *(u16 *)(p + 0x43e4);
            gUnk_03005210 = *(u16 *)(p + 0x43e6);
            clearFlags(&gUnk_0300523C, ~4);
            Sound_ResumePersistentSongs();
            if (p[0x487c]) {
                HealthBurstEffect_Create(*(void **)(p + 0xa40), p[0x487d]);
                p[0x487c] = zero;
            }
            if (p[0x4874]) {
                void *script = *(void **)(p + 0x4878);
                if (script) {
                    args.count = 1;
                    value = p[0x4875];
                    args.values = &value;
                    Script_Call(script, &args);
                }
                p[0x4875] = zero;
                p[0x4874] = zero;
            }
            if (p[0x4870]) {
                s32 i;
                struct PackedVec *a, *b;
                pos = *(struct PackedVec *)(*(u8 **)(p + 0xa40) + 0x30);
                pos.x -= 512;
                pos.z -= 512;
                pos.y -= 256;
                size.x = 1024;
                size.z = 1024;
                size.y = 512;
                a = &pos;
                b = &size;
                for (i = 2; i >= 0; i--)
                    EffectManager_SpawnParticles(4, p[0x4871], *(u16 *)(p + 0x4872), a, b, 300,
                                                 120);
                p[0x4871] = 0;
                p[0x4870] = 0;
            }
            if (p[0x487f]) {
                if (*(void **)(p + 0x4878)) {
                    args2.count = 1;
                    value2 = p[0x487f] - 1;
                    args2.values = &value2;
                    Script_Call(*(void **)(p + 0x4880), &args2);
                }
                p[0x487f] = 0;
            }
        }
    }
}
