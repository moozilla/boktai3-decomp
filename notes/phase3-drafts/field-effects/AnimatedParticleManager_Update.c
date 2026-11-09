// Pin-free draft of AnimatedParticleManager_Update (src/fn/sub_08015160.c): 32 ROM / 32 draft; AGBCC_PRI=p:+206 matches.
int AnimatedParticleManager_Update(struct Manager *m)
{
    struct Obj *sprites = &m->pieces[0].sprite;
    struct Piece *cursor = m->pieces, *base = cursor;
    int i;
    for (i = 31; i >= 0; sprites = (void *)((u8 *)sprites + 120), base++, cursor++, i--) {
        if (cursor->active) {
            struct Piece *p = base;
            struct Obj *o = sprites;
            struct Anim *a;
            const u16 *entry;
            int done;
            if (cursor->timer >= cursor->horizontalStart) {
                p->horizontalScale = scale8(cursor->horizontalScale, cursor->horizontalDecay);
                p->velocity.x = scale12(p->direction.x, p->horizontalScale);
                p->velocity.z = scale12(p->direction.z, p->horizontalScale);
            }
            if (p->timer >= p->verticalStart) {
                p->verticalScale = scale8(p->verticalScale, p->verticalDecay);
                p->velocity.y = scale12(p->direction.y, p->verticalScale);
            }
            o->position.x += p->velocity.x;
            o->position.y += p->velocity.y;
            if (!(o->flags & 16))
                o->position.z += p->velocity.z;
            a = &p->anim;
            entry = a->data + a->frame;
            o->frame = *entry >> 6;
            {
                union DisplaySequenceControl control;
                u32 bit, first;
                control.raw = a->flags;
                bit = 1;
                first = (u8)(control.raw & bit);
                if (first != (((*entry & 0x30) >> 4) & bit))
                    o->flags |= 4;
                else
                    o->flags &= ~4;
            }
            {
                union DisplaySequenceControl control;
                u32 bit, first;
                control.raw = a->flags;
                bit = 2;
                first = (u8)(control.raw & bit);
                if (first != (((*entry & 0x30) >> 4) & bit))
                    o->flags |= 8;
                else
                    o->flags &= ~8;
            }
            if (++a->timer >= a->rate) {
                a->timer = 0;
                if (a->flags & 4) {
                    if (a->frame == 0) {
                        if (a->flags & 8) {
                            a->timer = a->rate;
                        } else {
                            a->frame = a->count - 1;
                        }
                        done = 1;
                    } else {
                        a->frame--;
                        done = 0;
                    }
                } else {
                    if (++a->frame >= a->count) {
                        if ((u8)(a->flags & 8)) {
                            a->timer = a->rate;
                            a->frame = a->count - 1;
                        } else {
                            a->frame = 0;
                        }
                        done = 1;
                    } else
                        done = 0;
                }
                entry = a->data + a->frame;
                a->factor = frame_factor(*entry);
                a->rate = (a->factor * a->scale) >> 6;
                if (!a->rate)
                    a->rate = 1;
            } else
                done = 0;
            if (done) {
                p->active = 0;
                Sprite_Remove(o);
            }
            p->timer++;
        }
    }
    m->timer++;
    return 0;
}
