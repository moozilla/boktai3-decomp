#include "global.h"
#include "ram.h"
struct CollisionPoint {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
struct Col {
    u8 pad0[0x1C];
    u32 renderFlags;
    u8 pad20[0x48 - 0x20];
    u8 sequence[0x58 - 0x48];
    void *animData;
    u8 pickupEntry[0x40];
    u8 contactEntry[0x40];
    struct ColState {
        s16 x, y, z;
        u8 padE2[0xEC - 0xE2];
        u16 hp;
        u8 type;
        u8 padEF;
        u8 active, tick;
        u8 padF2[2];
    } st;
    void (*update)(void *);
};
struct ColManager {
    u8 header[0x18];
    struct Col entries[48];
};
struct ColManager *Collectible_CreateManager(void);
s32 Terrain_GetCellClearance(void *);
void Collectible_UpdateEntry(void *);
void Collectible_OnPickup();
void Collectible_OnContact();
void SpatialIndex_InitEntry(void *, void *, void *, void *, s32, s32, s32, s32, s32, void (*)(),
                            void *);
void Anim_SetSequence(void *, void *, u8, u8, u8);

s32 sub_08115AC8(s32 x, s32 y, s32 type, s32 hp)
{
    struct ColManager *m;
    s32 i;
    if (gActorParticlePool == NULL) {
        m = Collectible_CreateManager();
        if (m == NULL)
            return -1;
    } else {
        m = gActorParticlePool;
    }
    for (i = 0; i < 48; i++) {
        if (!m->entries[i].st.active) {
            struct CollisionPoint pos, size, offset;
            s32 range, seq, flip;
            s32 clearance;
            m->entries[i].st.x = x << 3;
            m->entries[i].st.y = y << 3;
            m->entries[i].st.z = m->entries[i].st.y - 0x60;
            clearance = Terrain_GetCellClearance(&m->entries[i].st.x);
            if (clearance < m->entries[i].st.y)
                m->entries[i].st.y = clearance;
            m->entries[i].st.active = 1;
            m->entries[i].st.tick = 1;
            m->entries[i].update = Collectible_UpdateEntry;
            m->entries[i].st.hp = hp;
            m->entries[i].st.type = type;
            switch (m->entries[i].st.type) {
                case 0:
                    size.x = 16;
                    size.y = 8;
                    size.z = 14;
                    offset.x = 0;
                    offset.y = -size.y;
                    offset.z = -10;
                    range = 0x28;
                    seq = 0;
                    flip = 0;
                    break;
                case 2:
                    size.x = 16;
                    size.y = 8;
                    size.z = 14;
                    offset.x = 0;
                    offset.y = -size.y;
                    offset.z = -10;
                    range = 0x50;
                    seq = 0;
                    flip = 1;
                    break;
                case 1:
                    size.x = 8;
                    size.y = 16;
                    size.z = 8;
                    offset.x = 0;
                    offset.y = -size.y;
                    offset.z = 0;
                    range = 0xF0;
                    seq = 3;
                    flip = 0;
                    break;
                default:
                    return -1;
            }
            pos.x = m->entries[i].st.x;
            pos.y = (s16)(m->entries[i].st.y - 0x60) - m->entries[i].st.z;
            pos.z = m->entries[i].st.z;
            SpatialIndex_InitEntry(m->entries[i].pickupEntry, &pos, &size, &offset, 12, 6, 2, range,
                                   12, Collectible_OnPickup, &m->entries[i]);
            SpatialIndex_InitEntry(m->entries[i].contactEntry, &pos, &size, &offset, 20, 6, 2, 0,
                                   12, Collectible_OnContact, &m->entries[i]);
            Anim_SetSequence(m->entries[i].sequence, m->entries[i].animData, seq, flip, 0);
            m->entries[i].renderFlags |= 1;
            return i;
        }
    }
    return -1;
}
