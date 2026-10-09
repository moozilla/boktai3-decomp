// COMPILER: old_agbcc
// CFLAGS: -O2
/* Independently reconstructed; callers match newlib's __errno interface.
 * Returns the existing reentrancy-record pointer (errno is its first field).
 * See docs/LIBM.md for linked-call and boundary evidence. */
extern int *const gUnk_08E88140;
int *sub_0824DA3C(void)
{
    return gUnk_08E88140;
}
