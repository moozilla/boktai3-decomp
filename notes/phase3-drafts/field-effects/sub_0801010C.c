int BoardPieces_StartConvergence(struct GroupObjects *m, struct GroupQueue *q,
                                 struct GroupCommand *cmd)
{
    struct PackedVec origin;
    struct GroupVec d;
    int i;
    int radius;
    void *palette;
    struct GroupPiece *p;
    struct GroupPiece *r;
    origin.x = cmd->originX;
    origin.y = cmd->originY;
    origin.z = cmd->originZ;
    radius = cmd->radius0;
    if (radius <= 0)
        radius = 1;
    *(struct PackedVec *)&m->origin = origin;
    m->timer = 0;
    r = m->pieces;
    i = 0;
    palette = m->palette;
    for (; i < 16; i++, r++) {
        int angle;
        struct GroupVec *dp = &d;
        struct GroupVec *o = (struct GroupVec *)&origin;
        p = r;
        {
            u32 state = 7;
            u32 zero = 0;
            p->state = state;
            p->active = 1;
            p->timer = zero;
        }
        dp->x = o->x - p->position.x;
        dp->y = o->y - p->position.y;
        dp->z = o->z - p->position.z;
        angle = Math_Atan2(dp->x, dp->z);
        p->duration = Div(
            scale(dp->x * gUnk_086149C4[(angle + 64) & 255] + dp->z * gUnk_086149C4[angle & 255]),
            radius);
        *(struct PackedVec *)&p->target = origin;
        p->angle = randomValue();
        p->targetY = i * 16 + p->position.y;
        p->speed = (randomValue() & 7) + 7;
        p->sprite.position = p->position;
        p->sprite.flags &= ~1;
        BoardPiece_SetFacingAnim(m, r, 0);
        AnimData_SelectPaletteBank(&r->resource, 0x2D3);
        p->resource.palette = palette;
        ActorShadow_Enable(r->transform);
    }
    m->active = 1;
    return 0;
}
