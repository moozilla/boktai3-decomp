#include "global.h"
#include "engine.h"
#include "ram.h"

/* Two facing rows of nine fragment displacements. */
struct ActorMarkerFragmentOffset {
    s16 x, y, z, pad;
};
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
extern const struct ActorMarkerFragmentOffset gUnk_0824E818[18];
void ActorMarker_InitBouncingParticles(struct ActorMarker *);
void Sound_PlaySongChecked(u32);
static inline u16 NextFragmentLift(const u16 *table, u32 *index)
{
    u32 value = 1 + *index;
    u32 mask = 0x3ff;
    value &= mask;
    *index = value;
    value = table[value];
    mask = 31;
    return value & mask;
}

void ActorMarker_LaunchFragments(struct ActorMarker *p, u32 fast)
{
    s32 row = 0;
    s32 i;
    const struct ActorMarkerFragmentOffset *rowStart;
    const struct ActorMarkerFragmentOffset *table;
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
    rowStart = table + (row8 + row);
    /* Index the immutable offset table relative to the selected row. */
    for (; i < 9; i++) {
        struct ActorMarkerFragment *fragment = &p->fragments[i];
        fragment->state = 0;
        fragment->position[0] = *posX + gUnk_0824E818[(rowStart + i) - table].x;
        fragment->position[1] = *posY + gUnk_0824E818[(rowStart + i) - table].y;
        fragment->position[2] = *posZ + gUnk_0824E818[(rowStart + i) - table].z;
        if (fast) {
            fragment->velocity[0] = gUnk_0824E818[(rowStart + i) - table].x >> 3;
            fragment->velocity[1] = NextFragmentLift(randomTable, randomIndex);
            fragment->velocity[2] = gUnk_0824E818[(rowStart + i) - table].z >> 3;
        } else {
            fragment->velocity[0] = gUnk_0824E818[(rowStart + i) - table].x >> 5;
            fragment->velocity[1] = NextFragmentLift(randomTable, randomIndex);
            fragment->velocity[2] = gUnk_0824E818[(rowStart + i) - table].z >> 5;
        }
    }
    Sound_PlaySongChecked(0xc4);
}
