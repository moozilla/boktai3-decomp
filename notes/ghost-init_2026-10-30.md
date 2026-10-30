# Ghost initializer: typed stores and the reload register pool

Tags: #matching #monster #ghost #compiler #ram

`Ghost_Init` (`08099B44`, 4,752 bytes) matches in ordinary C. The uninstrumented
compiler and the independent translation-unit comparison both report zero differing
bytes. There are no register pins, inline assembly, compiler overrides, or per-consumer
header selectors. The unit consumes the shared `DisplaySequence_Step` implementation.

## The last large discrepancy came from two halfword stores

The constructor reads script keyword `K` into the halfword at actor offset `0xB2`,
then reads and clamps keyword `V` before storing the halfword at `0x9A`. With ordinary
component assignments, agbcc expands each halfword write as a bit insertion. The
initial expansion contains a zero-valued halfword temporary, even though the complete
field is overwritten and the masking operation is subsequently removed.

```c
actor->fieldB2 = Script_GetKeywordValue('K', 2);
```

CSE reused that temporary for the later route-selector reset and absent-route stores.
The zero therefore survived across the body-construction calls in `r4`. This affected
more than those stores: `r4` never entered the function's reload register pool, changing
address and literal scratch registers throughout the initializer.

A private diagnostic build admitted `r4` to the reload pool only where the normal
per-instruction liveness checks allowed it. Its default output was first checked
against the ordinary compiler. With that diagnostic active, the complete entry,
script parsing, attack loop, targeting, and health section matched the ROM. The
remaining misplaced zero identified the source-level boundary to recover. The probe
is not part of the production build or the accepted source.

The accepted operation writes a scalar halfword cell:

```c
static inline void MonsterInit_WriteHalfword(u16 *slot, u32 value)
{
    *slot = value;
}

MonsterInit_WriteHalfword(&actor->fieldB2,
                         Script_GetKeywordValue('K', 2));
MonsterInit_WriteHalfword(&actor->field9A, variant);
```

Both writes matter. Changing only `0xB2` moves the unwanted temporary to the `0x9A`
write. Changing both removes it. The ordinary compiler then materializes the route
zero at the correct place, preserves the failed keyword lookup result for the absent
route, and uses the correct reload register rotation throughout the function.
The same operation completed `Lun_Init` without further register changes.

## The attack loop exposes two strength-reduction passes

The first attack value is written through an indexed setter with a full-width logical
value parameter. The three halfword script values use a halfword-valued reader. The
loop increases its logical index and ends with an explicit break:

```c
while (1) {
    Ghost_SetFirstAttack(actor, i, Ghost_ReadHalfword(24));
    actor->attacks.second[i] = Ghost_ReadHalfword(24);
    actor->attacks.flags[i] = Value(0);
    actor->attacks.parameter[i] = Ghost_ReadHalfword(0);
    ++i;
    if (i > 3)
        break;
}
```

This preserves the forward signed counter. In the first strength-reduction pass the
loop has 63 instructions and two address candidates have benefit 60, so they are
rejected. Other reductions shorten the loop to 56 instructions. The second pass then
accepts the flags pointer while retaining the first-attack base plus byte cursor.
The result is the ROM's pointer order without explicit pointer walks or register hints.

## Visibility transitions and stack ordering

Each initial visibility branch updates the context flags, updates the drawable's
hidden bit, and resets its transition timer at context offset `0x5B2`. Keeping the
timer reset inside each transition reproduces the two address calculations before
the common final halfword store. Moving it after the conditional lets the compiler
share a larger suffix, producing a function 12 bytes shorter than the ROM.

After the instruction stream matched, only PRE stack-slot order differed. One
halfword-valued keyword reader is used consistently for `M`, `F`, `R`, `K`, `C`, and
`l`. This expresses the same storage domain as the optional halfword reader and
produces the natural 903-bucket expression table required by the ROM's slot order.
The final match uses the stock compiler, with no table-size override.

## Recovered interfaces ready for shared extraction

The source has typed attack arrays, targeting fields, body coordinates, route records,
and saved control state. It uses the existing player position, game-state region and
solar fields, profile-entry record, and shared sequence playback record. Ghost's
private tail remains distinct: flags at `0x558`, visibility timer at `0x5B2`, effect
parameters at `0x5C0`, player position at `0x604`, and route at `0x60C`.

The following extraction will replace repeated common actor records and helper bodies
across the completed initializer family. Callback member types must retain their
actual indirect-call contracts; the `0x260` animation query's typed return is what
makes its required stack spill appear.
