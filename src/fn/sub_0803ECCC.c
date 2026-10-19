#include "global.h"

void ActorParticles_SpawnSideBurst(u8 *);
void CallbackList_ResetDispatch(u8 *);
void ActorParticles_EmitOffsetBurst(u8 *);
void Sound_PlaySong(u32);
void ScenePool_ReleaseActorSlot(u32, u8 *, u8 *, u32);
s32 Mod(s32, s32);

u32 ScenePool_StatePlayRemovalEffects(u32 a, u8 *b, u8 *e, u32 d)
{
    u32 f;
    u32 z;
    u32 m;
    if (e[0x64] != 0) {
        z = 0;
        e[0x64] = z;
        ActorParticles_SpawnSideBurst(b);
        CallbackList_ResetDispatch(b);
        *(u32 *)(b + 0x5ac) = z;
        *(u32 *)(b + 0x5b4) = z;
        *(u32 *)(b + 0xc0) = z;
        *(u32 *)(b + 0xc8) = z;
    }
    m = 2;
    *(u32 *)(b + 0x5fc) |= m;
    *(u32 *)(b + 0x5fc) |= 1;
    *(u32 *)(b + 0x5fc) |= 0xc;
    f = *(u32 *)(e + 0x44);
    if (f <= 0x1d) {
        if (Mod(f, 0xc) == 0) {
            ActorParticles_EmitOffsetBurst(b);
            Sound_PlaySong(0x16e);
        }
    } else if (f <= 0x3b) {
        if (Mod(f, 6) == 0) {
            ActorParticles_EmitOffsetBurst(b);
            Sound_PlaySong(0x16e);
        }
    }
    if (*(u32 *)(e + 0x44) > 0x77)
        ScenePool_ReleaseActorSlot(a, b, e, d);
    (*(u32 *)(e + 0x44))++;
    return 0;
}
