#include "global.h"
#include "engine_globals.h"
#include "player.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 86.4%
// fix.py best (sweep after the inlined-insn fix)
struct S {
    u8 f[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 g[0x18 - 0xa];
    u8 b18;
    u8 g2[0xd4 - 0x19];
    u8 d4[1];
};
void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 DisplayAnimation_TestFinished(void *);

void Sound_PlaySong(u32);
u8 Callback_InvokeActor(void *, u32);
void sub_080148CC(s32, s32, void *, s32, s32, s32, s32, s32, s32, s32, s32);
void PlayerCue_BeginModeSeven(void *, s32);
struct Vec {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
static inline s32 Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}
static inline s32 RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
// confidence=low: emits two four-direction effect groups during a two-stage animation.
#define StateActor_UpdateEightWayBurst sub_081CA490
inline static s32 Random_081CA490(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return (*((typeof(gRandomTable) *)0x0203B400))[gRandomIndex];
}

void StateActor_UpdateEightWayBurst(u32 a, struct S *p, s32 tick)
{
    u32 r;
    if (p->b9 != 0) {
        p->b9 = 0;
        p->b7 = 0;
        switch (p->b1) {
            case 4:

            case 5:

            case 6:

            case 7:

            case 8:

            case 9:

            case 10:

            case 11:

            case 12:

            case 13:

            case 14:
                r = 0;
                break;

            case 15:
                r = 8;
                break;

            case 16:

            case 17:

            case 18:

            case 19:

            case 20:

            case 21:

            case 22:

            case 23:

            case 24:

            case 25:

            case 26:

            case 27:

            case 28:

            case 29:
                r = 0;
                break;

            default:
                r = 0;
                break;
        }

        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->b18 = 0;
        {
            s32 choice = (u8)Random_081CA490();
            if (choice <= 84)
                Sound_PlaySong(0x2DE);
            else if (choice <= 169)
                Sound_PlaySong(0x2DF);
            else
                Sound_PlaySong(0x2E0);
        }
    }
    if (((u8)Callback_InvokeActor(p, *((u32 *)(((u8 *)p) + 0x2E4)))) == 1)
        return;
    if ((p->b18 == 0) && (tick == 3)) {
        register const u16 *table asm("r6");
        register s32 ten asm("r8") = 10;
        register s32 fourteen asm("sl");
        struct Vec position = *((struct Vec *)(((u8 *)p) + 0x68));
        *((volatile s16 *)(&position)) += 200;
        table = gRandomTable;
        sub_080148CC(4, 0x26, &position, 0, RandomFrom(table) & 63, 12, ten, 5, 8, 8, 5);
        sub_080148CC(4, 0x26, &position, 64, RandomFrom(table) & 63, 12, ten, 5, 8, 8, 5);
        sub_080148CC(4, 0x26, &position, 128, RandomFrom(table) & 63, 12, ten, 5, 8, 8, 5);
        sub_080148CC(4, 0x26, &position, 192, RandomFrom(table) & 63, 12, ten, 5, 8, 8, 5);
        fourteen = 14;
        sub_080148CC(2, 0x26, &position, 0, RandomFrom(table) & 63, ten, 4, 4, 4, fourteen, 4);
        sub_080148CC(2, 0x26, &position, 64, RandomFrom(table) & 63, ten, 4, 4, 4, fourteen, 4);
        sub_080148CC(2, 0x26, &position, 128, RandomFrom(table) & 63, ten, 4, 4, 4, fourteen, 4);
        sub_080148CC(2, 0x26, &position, 192, RandomFrom(table) & 63, ten, 4, 4, 4, fourteen, 4);
        PlayerCue_BeginModeSeven(gPlayers[0], 2);
    }
    if (DisplayAnimation_TestFinished(p->d4)) {
        switch (p->b18) {
            case 0:
                switch (p->b1) {
                    case 4:

                    case 5:

                    case 6:

                    case 7:

                    case 8:

                    case 9:

                    case 10:

                    case 11:

                    case 12:

                    case 13:

                    case 14:
                        r = 0;
                        break;

                    case 15:
                        r = 6;
                        break;

                    case 16:

                    case 17:

                    case 18:

                    case 19:

                    case 20:

                    case 21:

                    case 22:

                    case 23:

                    case 24:

                    case 25:

                    case 26:

                    case 27:

                    case 28:

                    case 29:
                        r = 0;
                        break;

                    default:
                        r = 0;
                        break;
                }

                Actor_PlayAnimationEntry(p, r, 2, 0, 4);
                p->b18++;
                *((u32 *)(((u8 *)p) + 0x10)) = 0;
                break;

            case 1:
                p->b7 = 1;
                p->b18++;
                *((u32 *)(((u8 *)p) + 0x10)) = 0;
                break;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081CA490);
#endif
