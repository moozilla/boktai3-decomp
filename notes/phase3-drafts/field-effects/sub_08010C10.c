// Pin-free draft of sub_08010C10 (src/group_objects.c): 2 ROM / 2 draft; AGBCC_SPILL=0:1 matches.
void sub_08010C10(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    struct GroupVec delta;
    struct GroupVec *offset;
    int active;
    if (m->actor)
        p->target = m->actor->position;
    active = p->active;
    offset = &p->offset;
    if (active) {
        p->active = 0;
        p->anchor = p->position;
        p->direction = randomValue();
        ZeroFill8(offset);
    }
    Vec16_Lerp(&p->anchor, &p->anchor, &p->target, index + 8, 5);
    if (p->speed == 0) {
        const u16 *table;
        p->direction = Math_Atan2(-p->offset.x, -p->offset.z);
        table = gRandomTable;
        p->direction += (randomWith(table) & 15) - 7;
        p->speed = (randomWith(table) & 7) + 8;
    } else
        p->speed--;
    polar(&delta, p->direction, p->duration);
    {
        struct GroupVec *src = &delta;
        p->offset.x += delta.x;
        offset->y += src->y;
        offset->z += src->z;
    }
    p->position.x = p->anchor.x + p->offset.x;
    p->position.z = p->anchor.z + p->offset.z;
    BoardPiece_SetFacingAnim(m, p, 0);
    p->timer++;
}
