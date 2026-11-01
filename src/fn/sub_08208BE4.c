#include "global.h"
#include "player_cue_sequence.h"
#include "display_animation.h"
#include "scene_vec.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0820843C(void);
void sub_082086B8(void);
void CannonTurret_InitCollider(void *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
#define H(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
/* The state callback, sequence, defeat flag and resource slot that the script setup writes. */
// confidence=low: field names follow FanCannon, whose layout this tail mirrors.
struct CannonTurretTail {
    u8 pad0[0x108];
    void (*update)(void);
    u8 sequence[0x24];
    s16 dead;
    u8 pad132[0x32];
    void *resource;
};
s32 CannonTurret_InitializeFromScript(u8 *p, void *parent)
{
    void *resource;
    u32 sequenceResult;
    if (Script_SeekToKeyword('p')) {
        H(0x1c) = Script_GetValue();
        H(0x1e) = Script_GetValue();
        H(0x20) = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        W(0x11c) = r * r;
    } else
        W(0x11c) = 0x40000;
    if (Script_SeekToKeyword('a')) {
        H(0x120) = Script_GetValue();
        H(0x122) = Script_GetValue();
        H(0x124) = Script_GetValue();
    } else {
        H(0x120) = 30;
        H(0x122) = 0;
        H(0x124) = 30;
    }
    if (Script_SeekToKeyword('l')) {
        {
            s32 health = Script_GetValue();
            H(0x136) = health;
            H(0x132) = health;
            if ((s16)health < 0)
                H(0x146) = 1;
        }
    } else {
        H(0x136) = 120;
        H(0x132) = 120;
    }
    if (Script_SeekToKeyword('s'))
        H(0x12a) = Script_GetValue();
    else
        H(0x12a) = 12;
    if (Script_SeekToKeyword('S'))
        H(0x13e) = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        H(0x138) = Script_GetValue();
        H(0x13a) = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        H(0x13c) = Script_GetValue();
    else
        H(0x13c) = 2;
    if (Script_SeekToKeyword('D'))
        W(0x160) = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        W(0x154) = Script_GetValue();
        W(0x158) = Script_GetValue();
        W(0x15c) = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        H(0x140) = Script_GetValue();
        H(0x142) = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        H(0x144) = Script_GetValue();
        if (H(0x144) > 99)
            H(0x144) = 99;
        if (H(0x144) <= 0)
            H(0x144) = 1;
    } else
        H(0x144) = 0;
    *(void **)(p + 0x148) = parent;
    AnimData_Load(p + 0x50, 0x7e7d);
    Sprite_Init(p + 0x24, p + 0x50, 1);
    *(struct SceneVec *)(p + 0x40) = *(struct SceneVec *)(p + 0x1c);
    AnimData_SelectPaletteBank(p + 0x50, 0x1bc);
    resource = ScriptProc_Start(0x922e, 0x5184);
    ((struct CannonTurretTail *)p)->resource = resource;
    if (H(0x146) == 0) {
        ((struct CannonTurretTail *)p)->update = sub_0820843C;
        sequenceResult = Anim_SetSequence(p + 0x10c, resource, 8, 0, 0);
    } else {
        ((struct CannonTurretTail *)p)->update = sub_082086B8;
        ((struct CannonTurretTail *)p)->dead = 1;
        sequenceResult =
            Anim_SetSequence(p + 0x10c, ((struct CannonTurretTail *)p)->resource, 10, 0, 0);
    }
    PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)(p + 0x24), (void *)(p + 0x10c));
    if (H(0x146) == 0)
        CannonTurret_InitCollider(p);
    if (H(0x13e) != 0) {
        u8 *body = p + 0xc0;
        SceneBody_Init(body, *(void **)(p + 0x148), p + 0x40, 0, 4, p);
        SceneBody_SetSprite(body, p + 0x24);
        TerrainOverlay_QueueAddActor(body);
    }
}
