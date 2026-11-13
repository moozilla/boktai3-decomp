void Actor_UpdateBlinkBurstDisappearance(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    u8 done;
    if (take(p)) {
        p->state = 13;
        Anim_SetSequence(&p->anim, p->arg, 13, 0, 0);
        AnimData_SelectPaletteBank((u8 *)p + 0xe08, 346);
        o = &p->obj;
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
                        done = 1;
                    } else {
                        a->frame = 0;
                        done = 1;
                    }
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
        p->blinkTimer = 0;
        p->blinkInterval = 15;
    }
    if (Mod(++p->blinkTimer, p->blinkInterval) == 0) {
        p->blinkTimer = 0;
        if (p->obj.flags & 1)
            p->obj.flags &= ~1;
        else
            p->obj.flags |= 1;
        if (--(*(u16 *)&p->blinkInterval) == 0) {
            s32 duration = 10;
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 32, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 64, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 96, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 128, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 160, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 192, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 224, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 256, 60, duration, 3, duration, 5, 60, duration);
            ActorShadow_Disable(p->p2);
            p->obj.flags |= 1;
            Actor_SetState(p, 1, 2);
            {
                u32 active = 4;
                register void (*fn)(void *, void *) = Actor_OnInteractionChildUpdate;
                u8 reset = 1;
                s32 i;
                for (i = 0; i < 16; i++) {
                    struct ActorChildSlot *child = &p->children[i];
                    union DisplaySequenceControl control;
                    control.raw = child->control;
                    if (active & control.raw) {
                        u32 flags;
                        child->fn = fn;
                        control.raw = reset | control.raw;
                        child->control = control.raw;
                    }
                }
            }
        }
    }
}

