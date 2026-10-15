// MATCHING_CONCESSION: register-pinning
#include "stacked_actor.h"
#define StackedActor_UpdateSegments sub_08052418
void StackedActor_UpdateSegments(struct StackedActor *actor)
{
    u16 *height = &actor->height;
    if (*height) {
        if (*height <= 2) {
            u16 *worldY;
            u32 y;
            u32 offset;
            actor->segments[0].flags &= ~1;
            worldY = (u16 *)&actor->world.y;
            asm("" : : "r"(worldY));
            y = 2 - *height;
            offset = 171;
            y *= offset;
            offset = 1026;
            y += offset;
            asm("" : "+r"(y));
            {
                u16 *sample = worldY;
                register u32 worldValue asm("r2") = *sample;
                y += worldValue;
            }
            actor->segments[0].position.y = y;
            actor->segments[1].flags |= 1;
            actor->segments[2].flags |= 1;
            {
                struct StackedActorSprite *sprite;
                u32 frame = actor->baseFrame;
                sprite = &actor->sprite;
                sprite->frame = frame;
            }
        } else if (*height <= 4) {
            register u16 *worldY asm("r12");
            u32 y;
            register u32 offset asm("r1");
            actor->segments[0].flags &= ~1;
            worldY = (u16 *)&actor->world.y;
            offset = 1026;
            asm("" : "+r"(offset));
            y = offset;
            asm("" : "+r"(y));
            {
                register u16 *sample asm("r5") = worldY;
                register u32 worldValue asm("r5") = *sample;
                y += worldValue;
            }
            actor->segments[0].position.y = y;
            actor->segments[1].flags &= ~1;
            y = 4 - *height;
            offset = 171;
            y *= offset;
            offset = 684;
            y += offset;
            {
                register u16 *sample asm("r2") = worldY;
                register u32 worldValue asm("r2") = *sample;
                y += worldValue;
                actor->segments[1].position.y = y;
                asm("" : : "r"(worldValue));
            }
            actor->segments[2].flags |= 1;
            {
                struct StackedActorSprite *sprite;
                u32 frame = actor->baseFrame;
                sprite = &actor->sprite;
                sprite->frame = frame;
            }
        } else if (*height <= 6) {
            register u16 *worldY asm("r12");
            u32 y;
            register u32 offset asm("r1");
            actor->segments[0].flags &= ~1;
            {
                u16 *initial = (u16 *)&actor->world.y;
                worldY = initial;
            }
            offset = 1026;
            asm("" : "+r"(offset));
            y = offset;
            asm("" : "+r"(y));
            {
                register u16 *sample asm("r5") = worldY;
                register u32 worldValue asm("r5") = *sample;
                y += worldValue;
            }
            actor->segments[0].position.y = y;
            actor->segments[1].flags &= ~1;
            offset = 684;
            asm("" : "+r"(offset));
            y = offset;
            asm("" : "+r"(y));
            {
                register u16 *sample asm("r5") = worldY;
                register u32 worldValue asm("r5") = *sample;
                y += worldValue;
            }
            actor->segments[1].position.y = y;
            actor->segments[2].flags &= ~1;
            y = 6 - *height;
            offset = 171;
            y *= offset;
            offset = 342;
            y += offset;
            {
                register u16 *sample asm("r2") = worldY;
                register u32 worldValue asm("r2") = *sample;
                y += worldValue;
                actor->segments[2].position.y = y;
                asm("" : : "r"(worldValue));
            }
            {
                struct StackedActorSprite *sprite;
                u32 frame = actor->baseFrame;
                sprite = &actor->sprite;
                sprite->frame = frame;
            }
            actor->sprite.renderMode = 3;
        } else {
            register u16 *worldY asm("r12");
            u32 y;
            register u32 offset asm("r1");
            actor->segments[0].flags &= ~1;
            {
                u16 *initial = (u16 *)&actor->world.y;
                worldY = initial;
            }
            offset = 1026;
            asm("" : "+r"(offset));
            y = offset;
            asm("" : "+r"(y));
            {
                register u16 *sample asm("r5") = worldY;
                register u32 worldValue asm("r4") = *sample;
                y += worldValue;
                actor->segments[0].position.y = y;
                asm("" : : "r"(worldValue));
            }
            actor->segments[1].flags &= ~1;
            offset = 684;
            asm("" : "+r"(offset));
            y = offset;
            asm("" : "+r"(y));
            {
                register u16 *sample asm("r4") = worldY;
                register u32 worldValue asm("r4") = *sample;
                y += worldValue;
                actor->segments[1].position.y = y;
                asm("" : : "r"(worldValue));
            }
            actor->segments[2].flags &= ~1;
            offset = 342;
            asm("" : "+r"(offset));
            y = offset;
            asm("" : "+r"(y));
            {
                register u16 *sample asm("r2") = worldY;
                register u32 worldValue asm("r2") = *sample;
                y += worldValue;
                actor->segments[2].position.y = y;
                asm("" : : "r"(worldValue));
            }
            {
                struct StackedActorSprite *sprite;
                u32 frame = actor->baseFrame;
                sprite = &actor->sprite;
                frame += 8;
                sprite->frame = frame;
            }
            actor->sprite.flags |= 0x200;
            actor->sprite.renderMode = 1;
        }
    } else {
        actor->segments[0].flags |= 1;
        actor->segments[1].flags |= 1;
        actor->segments[2].flags |= 1;
    }
}
