#include "global.h"

#include "engine.h"

// confidence=low: steers an actor one cell along its heading, then spawns a sun disc

// confidence=low: steers an actor one cell along its heading, then spawns a sun disc

u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, u32);

// confidence=low: steers an actor one cell along its heading, then spawns a sun disc
struct Sub08089340Position {
    s16 x, y, z, pad;
};
struct Sub08089340Phase {
    u8 unk_000[0x634];
    struct Sub08089340Position position;
    u8 unk_63C[0x640 - 0x63C];
    u8 cellFlags;
};
struct Sub08089340View {
    u8 unk_00[8];
    u16 mode;
    u8 unk_0A[4];
    u16 counter;
};
struct Sub08089340Actor {
    u8 unk_00[4];
    u8 *object;
    u8 unk_08[0x59 - 0x08];
    u8 direction;
    u8 unk_5A[2];
    struct Sub08089340Position position;
    u8 unk_64[0xB6 - 0x64];
    u16 spawnA;
    u8 unk_B8[0xBE - 0xB8];
    u16 spawnB;
    u8 unk_C0[0xC6 - 0xC0];
    u16 spawnC;
    u8 unk_C8[0xD0 - 0xC8];
    u32 spawnD;
    u8 unk_D4[0xDC - 0xD4];
    u8 *source;
    u8 unk_E0[0x2B0 - 0xE0];
    u8 flag2B0;
    u8 unk_2B1;
    u8 flag2B2;
    u8 flag2B3;
    u8 unk_2B4[0x2F4 - 0x2B4];
    u16 terrainCell;
    u8 unk_2F6[0x3D0 - 0x2F6];
    struct Sub08089340Phase *phase;
};
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SunDiscPool_SpawnDisc(void *, void *, s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
u8 sub_0812E118(s32, s32, s32, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);

static inline u8 Sub08089340_TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline s32 Sub08089340_Width(void)
{
    return *(s16 *)(gUnk_030052F4->cells + 4);
}

static inline u8 *Sub08089340_Cell(s32 cell)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return override + 4;
    return gUnk_030052F4->cells + (cell * 4 + 12);
}

#ifdef NONMATCHING
// fuzzy: 49.8%
// scope shape 302 (differences once low registers are set aside)
// confidence=low: steers an actor one cell along its heading, then spawns a sun disc
void sub_08089340(struct Sub08089340Actor *actor)
{
    struct Sub08089340Phase *phase = actor->phase;
    struct Sub08089340View *view = (struct Sub08089340View *)(actor->object + 0x48);
    s32 angle, quadrant, cell, next, steps, target, height, x, z;
    u32 shape;
    struct Sub08089340Position probe;

    if (Sub08089340_TakeFlag(&actor->flag2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    if (view->mode == 5 && view->counter == 0) {
        angle = actor->direction - ((((s32)actor->direction + 0x20) & 0xFF) >> 6 << 6);
        if (angle > 0x80)
            angle -= 0x100;
        else if (angle < -0x80)
            angle += 0x100;
        if (angle < 0)
            angle = -angle;
        if (angle <= 0x1f) {
            phase->position = *(struct Sub08089340Position *)(actor->source + 0xC);
        } else {
            cell = actor->terrainCell;
            {
                u8 *p = Sub08089340_Cell(cell);
                u8 attributes = *p;
                shape = attributes >> 4;
                height = (attributes & 15) << 8;
                if (shape == 1)
                    height -= ((u8 *)&actor->position)[4];
                else if (shape == 2)
                    height -= ((u8 *)&actor->position)[0];
            }
            quadrant = ((s32)actor->direction + 0x20) / 64 & 3;
            switch (quadrant) {
                case 0:
                    next = cell + 1;
                    steps = 0;
                    do {
                        if (!sub_0812E118(cell, next, height, shape))
                            break;
                        next++;
                        steps++;
                    } while (steps < 1);
                    target = next - 1;
                    break;
                case 1:
                    next = cell + Sub08089340_Width();
                    steps = 0;
                    do {
                        if (!sub_0812E118(cell, next, height, shape))
                            break;
                        next += Sub08089340_Width();
                        steps++;
                    } while (steps < 1);
                    target = next - Sub08089340_Width();
                    break;
                case 2:
                    next = cell - 1;
                    steps = 0;
                    do {
                        if (!sub_0812E118(cell, next, height, shape))
                            break;
                        next--;
                        steps++;
                    } while (steps < 1);
                    target = next + 1;
                    break;
                case 3:
                    next = cell - Sub08089340_Width();
                    steps = 0;
                    do {
                        if (!sub_0812E118(cell, next, height, shape))
                            break;
                        next -= Sub08089340_Width();
                        steps++;
                    } while (steps < 1);
                    target = next + Sub08089340_Width();
                    break;
                default:
                    target = actor->terrainCell;
                    break;
            }
            x = Mod(target, Sub08089340_Width());
            z = Div(target, Sub08089340_Width());
            phase->position.x = (x << 8) + 0x80;
            phase->position.y = (*Sub08089340_Cell(target) & 15) << 8;
            phase->position.z = (z << 8) + 0x80;
            quadrant = ((s32)actor->direction + 0x20) / 64 & 3;
            switch (quadrant) {
                case 0:
                    phase->position.x += 0x7f;
                    phase->position.z = actor->position.z;
                    break;
                case 1:
                    phase->position.x = actor->position.x;
                    phase->position.z += 0x7f;
                    break;
                case 2:
                    phase->position.x -= 0x7f;
                    phase->position.z = actor->position.z;
                    break;
                case 3:
                    phase->position.x = actor->position.x;
                    phase->position.z -= 0x7f;
                    break;
            }
            x = phase->position.x >> 8;
            z = phase->position.z >> 8;
            if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                target = 0;
            else
                target = gUnk_030052F4->rows[z] + x;
            {
                u8 attributes = *Sub08089340_Cell(target);
                height = (attributes & 15) << 8;
                shape = attributes >> 4;
                if (shape == 1)
                    height -= ((u8 *)&phase->position)[4];
                else if (shape == 2)
                    height -= ((u8 *)&phase->position)[0];
            }
            phase->position.y = height;
        }
        probe = actor->position;
        probe.y += 0xDC;
        SunDiscPool_SpawnDisc(&probe, &phase->position, actor->spawnA, actor->spawnB, actor->spawnD,
                              actor->spawnC, phase->cellFlags, 0x1e, -0x14);
        Sound_PlaySong(0xD6);
    }
    if (actor->flag2B3)
        actor->flag2B0 = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08089340);
#endif
