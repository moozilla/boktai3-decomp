// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_terrain.h"
struct S {
    u8 f[1];
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 b8;
    u8 b9;
    u8 g[0xd4 - 0xa];
    u8 d4[1];
};
/* Local copy of SceneActorTerrain_GetHeight. */
static inline s32 StateActor_GetTerrainHeight(s32 cell, const struct ContactPosition *position)
{
    register u32 attributes asm("r1") = *SceneActorTerrain_GetCell(cell);
    u32 kind = attributes >> 4;
    s32 height = (attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= ((const u8 *)position)[4];
            break;
        case 2:
            height -= ((const u8 *)position)[0];
            break;
    }
    return height;
}
#define H(o)  (*(u16 *)((u8 *)p + (o)))
#define SH(o) (*(s16 *)((u8 *)p + (o)))
#define DW(o) (*(s32 *)((u8 *)p + (o)))
void StateActor_UpdateAnimation16State(u32 a, struct S *p)
{
    u32 r;
    struct ContactPosition *pos;

    do {
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
                case 15:
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
                    r = 0;
                    break;
                case 27:
                    r = 0x10;
                    break;
                case 28:
                case 29:
                default:
                    r = 0;
                    break;
            }
            Actor_PlayAnimationEntry(p, r, 1, 0, 4);
            {
                u16 mask = 0x80 << 1;
                H(0xe) |= mask;
            }
        }
        {
            u16 mask = 0x80 << 3;
            H(0xe) |= mask;
        }

        pos = (struct ContactPosition *)((u8 *)p + 0x68);
        {
            s32 cell = SceneActorTerrain_GetCellIndex(pos);
            s32 height = StateActor_GetTerrainHeight(cell, pos);

            if (SH(0x6a) <= height) {
                SH(0x6a) = height;
                p->b7 = 1;
            } else {
                s32 v = -DW(0x208);
                SH(0x72) = v;
            }
        }
    } while (0);
}
