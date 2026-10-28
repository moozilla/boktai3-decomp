#include "global.h"
#include "engine.h"
#include "ram.h"

struct ActorMarkerFragment {
    u8 unk_00[0x24];
    u16 position[3];
    u8 unk_2A[0xa];
    s16 velocity[3];
    u8 unk_3A[4];
    u8 state;
    u8 unk_3F;
};
struct ActorMarker {
    u8 unk_00[0xd4];
    u16 position[3];
    u8 unk_DA[0x410 - 0xda];
    struct ActorMarkerFragment fragments[9];
    u8 unk_650[0xb39 - 0x650];
    u8 direction;
};
/* Two facing rows, each containing nine four-halfword fragment offsets. */
extern const u16 gUnk_0824E818[];
void ActorMarker_InitBouncingParticles(struct ActorMarker *);
void Sound_PlaySongChecked(u32);

/* Tentative: launch the nine embedded fragments during an actor break-apart sequence. */
#ifdef NONMATCHING
// fuzzy: 87.4%
// scope shape 8 (differences once low registers are set aside)
void ActorMarker_LaunchFragments(struct ActorMarker *p, u32 fast)
{
    s32 row = 0;
    s32 i;
    u32 mask;
    u32 value;
    const u16 *offset;
    const u16 *table;
    s32 row8;
    const u16 *randomTable;
    u32 *randomIndex;
    const u16 *posX;
    const u16 *posY;
    const u16 *posZ;
    switch (((p->direction + 0x60) >> 6) & 3) {
        case 1:
            row = 0;
            break;
        case 2:
            row = 0;
            break;
        case 0:
        case 3:
            row = 1;
            break;
    }
    ActorMarker_InitBouncingParticles(p);
    i = 0;
    row8 = row * 8;
    posY = &p->position[1];
    posZ = &p->position[2];
    posX = &p->position[0];
    table = gUnk_0824E818;
    randomTable = gRandomTable;
    randomIndex = &gRandomIndex;
    offset = table + (row8 + row) * 4;
    do {
        struct ActorMarkerFragment *fragment = &p->fragments[i];
        u16 x, y, z;
        fragment->state = 0;
        x = offset[0];
        fragment->position[0] = x + *posX;
        y = offset[1];
        fragment->position[1] = y + *posY;
        z = offset[2];
        fragment->position[2] = z + *posZ;
        if (fast) {
            fragment->velocity[0] = (s16)x >> 3;
            value = *randomIndex + 1;
            mask = 0x3ff;
            value &= mask;
            *randomIndex = value;
            value = randomTable[value];
            mask = 0x1f;
            value &= mask;
            fragment->velocity[1] = value;
            fragment->velocity[2] = (s16)z >> 3;
        } else {
            fragment->velocity[0] = (s16)x >> 5;
            value = *randomIndex + 1;
            mask = 0x3ff;
            value &= mask;
            *randomIndex = value;
            value = randomTable[value];
            mask = 0x1f;
            value &= mask;
            fragment->velocity[1] = value;
            fragment->velocity[2] = (s16)z >> 5;
        }
        offset += 4;
        i++;
    } while (i <= 8);
    Sound_PlaySongChecked(0xc4);
}
#else
INCLUDE_ASM("asm/nonmatching", ActorMarker_LaunchFragments);
#endif
