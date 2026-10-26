#include "global.h"
#include "scene_actor_terrain.h"
// confidence=low: timed-effect projectile update that turns at terrain steps.

struct TEConfig {
    u16 flags;
    u8 pad2[4];
    s8 angle;
    u8 pad7;
    s32 speed;
    u8 pad0C[0x48 - 0xC];
    s32 type;
};
struct TEntry {
    u32 frame;
    u8 pad4[0xC];
    struct ContactPosition vel;
    struct TEConfig config;
    u8 pad[0x174 - 0x18 - sizeof(struct TEConfig)];
    u16 prev[4];
    s16 turn;
};
struct TESprite {
    u8 pad[0x1C];
    u16 pos[3];
};
static inline s32 CellIndex(u16 *a)
{
    s32 x = ((s16)a[0]) >> 8;
    s32 z = ((s16)a[2]) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

void Projectile_SetUpdateCallback(void *, void *);
void Task4E01_SpawnImpactChildren(void);
void Sound_PlaySong(u32);
s32 Task4E01_SelectTerrainTurn(u16 *a, u16 *b, s32 original);

static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 speed)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * speed);
}

s32 TimedEffect_UpdateTerrainTurn(void *ctx, struct TEntry *e)
{
    struct TESprite *sp = (struct TESprite *)((u8 *)e + 0x8C);
    struct TEConfig *config = &e->config;
    u8 *attr;
    s32 type;
    attr = SceneActorTerrain_GetCell(CellIndex((u16 *)((u8 *)e + 0xA8)));
    if ((u32)((*attr & 15) << 8) > (u32)(s16)sp->pos[1]) {
        type = config->type;
        if (type == 5 && e->frame != 0) {
            e->turn = Task4E01_SelectTerrainTurn(sp->pos, e->prev, e->turn);
            {
                s32 n = e->turn;
                config->angle = (n << 5) - 0x60;
            }
            polar(&e->vel, config->angle, config->speed);
            Sound_PlaySong(0x3D0);
            return 0;
        } else if (type == 3 && !(config->flags & 0x10)) {
            s16 a = (((u16)config->angle + 0x70) >> 5) & 7;
            u16 ub = a;
            s16 b = ub;
            s32 t = Task4E01_SelectTerrainTurn(sp->pos, e->prev, e->turn);
            e->turn = t;
            if (!(t & 1)) {
                if ((s16)t == 0 && a == 6) {
                    e->turn = 1;
                } else if (e->turn == 6 && b == 0) {
                    e->turn = 5;
                } else {
                    e->turn = (e->turn - b) / 2 + (u16)e->turn;
                }
            }
            {
                s32 n = e->turn;
                config->angle = (n << 5) - 0x60;
            }
            config->flags |= 0x20;
            Projectile_SetUpdateCallback(config, Task4E01_SpawnImpactChildren);
            Sound_PlaySong(0x161);
            return 0;
        } else if (type == 9) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return 1;
    }
}
