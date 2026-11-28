#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void *DeckCannon_LoadAnimation(void);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void DeckCannon_UpdateMuzzleFlash(struct DeckCannonShot *);
void DeckCannon_DestroyMuzzleFlash(struct DeckCannonShot *);

void DeckCannon_InitMuzzleFlash(struct DeckCannonBattery *battery, struct DeckCannonShot *shot,
                                struct ContactPosition *origin, s32 horizontal)
{
    void *resource = DeckCannon_LoadAnimation();

    AnimData_Load(shot->animation, 0x8D95);
    AnimData_SelectPaletteBank(shot->animation, 0x8F);
    Sprite_Init(&shot->sprite, shot->animation, 0);
    if (horizontal) {
        Anim_RestartSequence(&shot->sequence, resource, 0, 0, 9);
    } else {
        Anim_RestartSequence(&shot->sequence, resource, 0, 0, 8);
    }
    if (horizontal) {
        shot->sprite.position.x = origin->x - 0xC0;
        shot->sprite.position.y = origin->y + 0x80;
        shot->sprite.position.z = origin->z;
    } else {
        shot->sprite.position.x = origin->x;
        shot->sprite.position.y = origin->y + 0x80;
        shot->sprite.position.z = origin->z - 0xC0;
    }
    shot->finished = 0;
    {
        void (*update)(struct DeckCannonShot *) = DeckCannon_UpdateMuzzleFlash;
        void (*destroy)(struct DeckCannonShot *) = DeckCannon_DestroyMuzzleFlash;
        shot->update = update;
        shot->destroy = destroy;
    }
}
