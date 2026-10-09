# Boktai 3 / MGS GCL matching evidence

## Scope and provenance

Round 1, 2026-10-08, independently reconstructed Boktai 3 C from the U33J
assembly using the MGS GCL comparison to prioritize and understand functions.
The comparison source is FoxdieTeam's reconstructed MGS Integral source,
revision `f54dbb2a58adfc2755403296c9ebb653fbec277b`, not recovered Konami source:
[basic.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/basic.c),
[command.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/command.c),
[parse.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/parse.c),
[strcode.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgv/strcode.c),
[script.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/game/script.c),
and [libgcl.h](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/libgcl.h).

No LICENSE or COPYING file was found by a filename scan of that pinned
checkout. No external source was copied or adapted into the matching units;
the similarities below are comparative evidence. No new semantic names were
installed in the symbol map. The already named `Script_DecodeOperand` retains
its existing name. These findings support a specific script-system comparison,
not a compiler identification or whole-engine ancestry claim.

`docs/PROCESS_REVIEW.md` contains the earlier hash/table evidence and SolDec
compatibility limits. This round checked which targets were already C before
counting gains: `08219BD8`, `0821ABE8`, `0821AC30`, `0821AC6C`, `0821AD08`,
`0821B794`, `0821B80C`, `0821B82C`, `0821B90C`, `0822580C`, and `08225884`
were already matched on base `68348386352893ad51ac2fec1d02fd95b77cbaaa`.

## Newly byte-exact functions

| Boktai 3 start | Matched bytes | Role established from B3 assembly | MGS comparison |
|---|---:|---|---|
| `0821A66C` | 84 | Compact packet length and payload pointer | B3-specific envelope decoder; MGS block lengths use fixed header layouts |
| `0821A6F0` | 420 | Operand decoder | `GCL_GetNextValue`, with substantial encoding and state differences |
| `0821AD2C` | 108 | Inline procedure ID and argument collection | `GCL_Proc` |
| `0821AF2C` | 136 | Block execution dispatcher | `GCL_ExecBlock` |
| `0821B2B0` | 156 | Variable descriptor decode, memory base, index operands | `GCL_GetVar` role; implementation not copied |
| `0821B478` | 80 | Copy variable descriptor and decode auxiliary words | No direct correspondence established |
| `0821B6C8` | 112 | Two-byte text conversion and CRLF normalization | No correspondence established in reviewed GCL source |
| `0821B738` | 92 | Conditional block selection with `e` / `i` alternatives | Selection part of `GCL_Command_if` |
| `0821B7B0` | 92 | `c` case / `d` default selection and execution | No switch builtin in reviewed MGS basic table |
| `0821B858` | 84 | Iterates operands and processes strings | `PrintCmd`, with output behavior removed/different |
| `0821B8AC` | 96 | Procedure call from current script cursor | No call builtin in reviewed MGS basic table |
| `08225560` | 48 | Registry callback dispatch from script operands | `CharaCmd`, with different callback arguments |
| `082257E4` | 40 | Installs eight engine command entries | Command-list initialization pattern |
| `08225844` | 64 | Lower-bound search of sorted registry | `GM_GetChara` is only a role comparison here |

Total: **14 new functions, 1,612 matched native bytes**. Byte counts include the
function extents measured by `tools/check.py`, including literal pools/alignment.
Matching demonstrates emitted-byte identity, not recovery of original C types
or all behavior under malformed input.

## Encoding, widths, and limits

`0821A66C` uses low nibble lengths 0–12 directly. D/E/F read respectively
one/two/three following little-endian bytes, returning a payload pointer after
2/3/4 header bytes. The maximum encoded length is `0xFFFFFF`. Callers advance
from that returned pointer by the decoded length. This is separate from
`0821A6C0`'s one/two-byte big-endian 15-bit native keyword offset. Native command
IDs in the already matched `0821AC6C` are little-endian 16-bit values.

The newly matched operand decoder establishes:

* C0–FF: integer type 9, value `(opcode & 0x3F) - 1`, range -1–62.
* Scalar 1: signed little-endian 16-bit. Scalars 6 and 8: unsigned
  little-endian 16-bit. Scalars 2/3/4: unsigned byte. Scalars 9/10/13:
  little-endian 32-bit. Scalar 0 ends the stream by returning null.
* Scalar 7: a byte length followed by inline text. Returned value points after
  the length byte; the cursor advances by length plus one. The encoded length
  cannot exceed 255; this routine does not enforce a NUL terminator.
* Scalar 0E: two-byte little-endian string ID passed to `Text_LookupString`;
  the returned operand type is changed to 7. Thus the explicit type-0E path in
  the print handler is not reached by a normal 0E operand through this decoder.
* Families 10/20 delegate to `0821B2B0`; the memory-base and index decoder
  is matched as described below; the final typed load helper remains unmatched.
  Family 40 reads an argument index from the low nibble;
  F extends it with the next byte plus 15 (maximum 270). Family 90 uses a
  low-nibble index and a different lookup helper. Both normalize type to 9.
* Families 80/30/50 use the compact envelope decoder. 80 returns the payload
  pointer, 30 evaluates the payload, and 50 places the keyword byte in type
  bits 16–23 and returns the pointer after that byte. Unknown families and
  unsupported scalar codes leave the value untouched; some return a cursor
  without consuming an envelope. No malformed-input bounds checks exist here.

Unlike MGS `GCL_GetNextValue`, B3 `Script_DecodeOperand` does **not** update the
global next-operand pointer itself. B3 callers such as `Script_SetPc` and vector
helpers manage that state. MGS's fixed GCL scalar/envelope tags and its
big-endian multibyte readers must not be copied into B3 tooling.

`0821AD2C` reads a **signed** 16-bit little-endian procedure ID before decoding
arguments. Its argument storage and `0821B8AC`'s call storage are sixteen
32-bit values on the stack. Both write a 16-bit count at argument-record offset
0 and a pointer at offset 4; emitted code preserves the upper half of the count
word. Local bitfields reproduce that write without claiming original type
recovery. Neither function checks count against sixteen before writing.
`0821AD2C` terminates on type 0; `0821B8AC` additionally checks the cursor for
null before decoding. Both return the procedure executor's result. MGS
`GCL_Proc` uses an eight-element array and emits an over-limit diagnostic,
while its `GCL_ARGS` header also has a 16-bit argc and argv pointer.

`0821B2B0` reads its four-byte variable descriptor in **big-endian** order.
The first byte's low nibble becomes the result type. Descriptor bits 20–23
select a RAM base: 8 selects the pointer at `02000710`, 1 selects `02000700`,
and all other values select `02000708`. The low 16 bits supply the byte offset.
Family 20 decodes two following operands, discards the first decoded value,
and passes the second as the load index to `0821B20C`. Family 10 uses index 0.
The load helper is still unmatched; no new runtime validation of selector
contents or array semantics was performed. The cursor is returned directly.

`0821B478` copies the same four descriptor bytes into a destination record.
Family 20 decodes two values and stores their low halves at offsets 4 and 6;
other families store 1 and 0 there and return the pointer after the descriptor.
The first auxiliary word may describe a count, but that semantic interpretation
was not validated here. Matching source uses local field labels for layout.

## Dispatch, conditions, and command behavior

`0821AF2C` dispatches on the opcode's high nibble: 60 executes a native
command; 70 collects/invokes a procedure; 30 evaluates an expression. Procedure
and expression results are written to VM offset 4 at `02000610`. The native
result is used only to recognize stop value 1. Type 0 or a null cursor returns
0. Unhandled families repeat with an unchanged pointer, so they can stall.

The B3 dispatcher saves state with `0821A8A4` and restores it with `0821A8C8`
on **both** normal completion and native stop value 1. The reviewed MGS
`GCL_ExecBlock` immediately returns on a native GCL_RETURN without executing
its normal stack restoration call. This difference is visible in matched B3
control flow and must survive any future cleanup.

The B3 `if` implementation is split: `0821B738` returns the selected block;
the already matched `0821B794` executes it only if non-null. False conditions
consume the next option; `e` sets the condition true and selects its block,
while `i` decodes another condition. Other options / null continuation yield
null. MGS's corresponding `GCL_Command_if` performs execution inside the same
function and returns GCL_OK; B3's wrapper propagates block execution's result.

The `switch` candidate `0821B7B0` decodes its comparison value, advances the
script cursor, then asks `0821A9B8` for option IDs. A matching `c` value or a `d`
option decodes one block and executes it with two zero context arguments;
missing options return 0. Other options are skipped. The hash/table evidence
supports the command spelling, but this source still keeps its sub-address name.

The `print` candidate `0821B858` has a 512-byte stack buffer. Type 7 calls
`0821B6C8` to convert text into that buffer. Type 0E scans to NUL without
writing output. There is no printf, display, or other visible-output call in
this function. MGS `PrintCmd` prints strings and numbers; this is therefore a
structural comparison, not proof of equivalent output behavior. The conversion
helper emits two bytes for high-bit input, uses row/column adjustments consistent
with an EUC-style to Shift-JIS conversion, skips CR in a CRLF pair, and writes
one terminating NUL. Charset identity remains a candidate interpretation:
no runtime example or complete valid-input characterization was tested. It has
no destination capacity argument, and the print handler supplies no explicit
512-byte capacity check.

## Registry and callback evidence

`08225844` performs lower-bound binary search on eight-byte records with a
32-bit unsigned key at offset 0 and a callback word at offset 4. Start/end are
signed indices; midpoint uses BIOS `Div(start + end, 2)`. After converging,
it checks equality and returns the callback word or zero. It does not separately
check whether the converged index is the end sentinel. A bounded read of the
known `08603300` registry verified 722 distinct keys in increasing order and
an all-zero sentinel at `08604990`; raw table contents were not saved/tracked.
The already matched wrapper supplies start 0 and the initialized 16-bit count.

The `chara` candidate `08225560` reads a script value truncated to u16,
resolves it through the registry wrapper, then calls the returned address with
another script value truncated to u16 and a zero second argument. Missing
callback returns -1; success returns 0 and discards the callback result. The
indirect call matches using an ordinary C function pointer; `08249248` is the
compiler's `bx r4` thunk, not an actor function. Two observed arguments do not
prove every callback's complete prototype or actor identity.

MGS `CharaCmd` instead calls its resolved callback with name, current map,
argc, and argv; the reconstructed source comments flag the argc/argv path as
receiving garbage from a mismatched command signature. This round did not
transfer those arguments or that bug claim to Boktai 3.

`082257E4` calls `082257E0`, resets command-list state via `0821ABDC`, fills
`030025E8` with `{next=0, count=8, entries=08E8791C}`, and returns the result of
`0821ABE8` registration. References use existing data labels so the ROM data
remains movable. The MGS basic table contains if/eval/return/foreach; B3's
reviewed six-entry basic table includes if/switch/eval/call/return/print.

## Negative findings and deferred work

The three-component vector decoder pair `0821AA1C` / `0821AA50` is structurally
close to MGS `GCL_StrToSV`: each decodes three values, stores 32-bit / 16-bit
components respectively, and updates B3's global cursor. Candidates remain
untracked because entry scheduling does not match. The typed variable-load helper `0821B20C` also remains an untracked candidate:
its initial switch layout was close, but shared-result rewrites worsened
register allocation; boolean normalization and shared stores remain residuals.
No external C was used as a
replacement and no matching gain is counted. No coverage, script execution,
callback tracing, namespace renaming, boundary reseeding, or new engine-wide
claim was produced by this matching round.

## Validation

Each new translation unit passed `tools/check.py`; the final combined worker
ROM build printed `build/boktai3.gba: OK`. The intro shift replay passed all
16 screenshot comparisons with data moved by `0x10000`. These screenshots
validate the existing scenario, not malformed scripts or all 722 callbacks.
The parent orchestrator performs combined-batch saved-game validation.

## Round 2: larger interpreter and message functions

Round 2 starts from worker commit `8b00059` and retains the same independent
B3 reconstruction policy. Additional comparison source inspected:
[expr.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/expr.c),
[variable.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/variable.c),
and [message.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgv/message.c).
No source was copied from these files.

| New B3 target | Bytes | Established behavior / comparison |
|---|---:|---|
| `0821AE04` | 204 | Script loader, corresponds structurally to `GCL_LoadScript` |
| `0821B938` | 292 | Integer operator dispatcher, corresponds to MGS `calc` |
| `082255BC` | 100 | Script message construction, corresponds to `MesgCmd` |
| `0821A340` | 168 | Queue insertion and payload copying, related role to `GV_SendMessage` |
| `0821A3E8` | 108 | Lookup of consecutive messages, related role to `GV_ReceiveMessage` |
| `0821B34C` | 152 | Typed variable store, related role to `GCL_SetVar` |
| `0821B3E4` | 148 | Descriptor decode and typed variable store |
| `0821B4C8` | 120 | Store through previously decoded variable reference |
| `0821B540` | 124 | Read through previously decoded variable reference |
| `0821B5BC` | 128 | Copy current variable value to alternate memory base |
| `0821B63C` | 116 | Read variable value from alternate memory base |

These eleven functions add **1,660 byte-exact native bytes**. The correspondence
names remain comparative descriptions; generated sub-address names are kept.
The operator function starts within this worker's range but ends at `0821BA5C`;
that following expression-executor function was not edited.

### Operator encoding

The matched `0821B938` operator dispatch has the following exact IDs:

| IDs | Operation |
|---|---|
| 1 / 2 / 3 | Negate RHS / logical NOT RHS / bitwise complement RHS |
| 4 / 5 / 6 | Add / subtract / multiply |
| 7 / 8 | BIOS `Div` / BIOS `Mod` |
| 9 / 10 | Left shift / **logical** right shift |
| 11 / 12 | Equal / not equal |
| 13 / 14 / 15 / 16 | Signed less / less-or-equal / greater / greater-or-equal |
| 17 / 18 / 19 | Bitwise OR / AND / XOR |
| 20 / 21 | Logical OR / AND |
| 23 | Return RHS |
| Other, including 0 and 22 | Return zero |

MGS `calc` has no shift cases; its equality-through-logical-AND IDs are two
lower, and its assignment ID 20 is handled by `GCL_Expr`, not `calc`. B3 ID 23
returning RHS does not by itself prove assignment semantics. B3 uses BIOS
Div/Mod instead of MGS's native C division/remainder. No division-by-zero or
out-of-range shift behavior was newly exercised. Returning directly from cases
was required for matching; an equivalent shared result variable emitted
operand mutations in different registers.

### Script loader layout

`0821AE04` reads a little-endian initial word into `0200060C`, calls the
already matched `Script_ReadProcTable` on input plus four, and installs the
table and count at `02000438` / offset 4. The table reader counts native words
until **-1**, returns one word beyond the sentinel, and does not byte-swap
entries. This differs from MGS's proc table, which terminates on a zero word
and rewrites two big-endian 16-bit fields in each record.

At the returned base B, B3 stores B at `02000448`, and resolves little-endian
relative offsets from B+4/B+8/B+12 into three further pointers. It advances by
the little-endian word at B, stores that address plus four as the script body,
and stores that address plus its own little-endian word plus eight as another
pointer at context offset 12. Names for these extra sections are not established.
No font setup call exists in this routine, unlike MGS `GCL_LoadScript`.
The routine returns zero and has no file-size/bounds argument.

### Message construction and queue layout

`082255BC` consumes a script value truncated to 16 bits as a message ID, then
collects further values truncated to 16 bits into a sixteen-element stack
array. The loop counter wraps at sixteen bits; the final stored count is eight
bits at message offset 3. The record contains ID at offset 0, an untouched byte
at offset 2, the count byte at offset 3, and a pointer at offset 4. The ID update
preserves the word's upper half; count update preserves the low 24 bits.
The C representation uses bitfields to reproduce those writes, without claiming
original type recovery. There is no sixteen-item bounds check in this routine.
It posts through `0821A340` and returns zero regardless of the posting result.
MGS `MesgCmd` similarly collects shorts but propagates a send failure as -1;
its `GV_MSG` embeds its payload, whereas B3's record points to payload storage.

The byte-exact `0821A340` establishes a 392-byte buffer stride and a payload
start at offset 264. The independent C layout represents this as two 32-bit
counters, thirty-two eight-byte message slots, and sixty-four 16-bit payload
slots; nominal original array capacities/types are not separately recovered. The selected buffer is the pointer returned by `0821A2DC`
plus `03001680` times 392. Insertion defaults to the end; scanning updates the
insertion point to one slot past each matching ID, so the new record follows
the last matching ID. Later entries shift one slot, preserving both words.
The new record clears byte 2, copies ID/count, points into the buffer's payload
area at the used counter, increases that counter, and copies count shorts.
It does not check either array capacity locally. The matched layout and code
alone do not establish that capacity is exceeded in real execution.

MGS `GV_SendMessage` also groups matching addresses, but checks `MAX_MESSAGES`
and copies its embedded fixed-size record instead of appending a separate
payload pool. B3's exact return/capacity/storage behavior must be retained in
future restructuring; this is not a drop-in library replacement.

`0821A3E8` selects the opposite buffer, at base plus `(1 - 03001680)`
times 392. It locates the first matching u16 ID, stores that record pointer
through the supplied output pointer, and returns the number of consecutive
matching records. Missing ID returns zero without writing the output pointer.
After finding the first matching record, it uses the previous count for an
initial bound test, then reloads the buffer count once before scanning the rest
of the group. This reload and the initial test are byte-exact matching details;
an equivalent single inner while loop removes the reload and does not match.
The reviewed MGS receiver checks `GV_PauseLevel` and returns a stored group
length; this B3 function has no pause guard and calculates the group size.

### Variable stores and alternate bases

`0821B34C` handles descriptor types 9 (native 32-bit store, stride 4), 8
(three little-endian byte stores at offsets 0/1/2, stride 4), 1/6 (16-bit store,
stride 2), 2/3 (byte store), and 4 (bit set/clear). For type 4, descriptor bits
16–19 are added to the supplied index, the byte uses arithmetic index >> 3,
and the mask uses index & 7. Zero clears the bit; nonzero sets it. Other types
leave memory unchanged. Type 8's three-byte store is a concrete finding,
not a claim that all uses represent a 24-bit integer. The load counterpart
remains unmatched and its intended width is not inferred from this writer.

`0821B3E4` uses the same big-endian descriptor and dynamic RAM bases as the
round-1 read decoder, then passes the second auxiliary operand as the write
index and its caller's value into `0821B34C`. `0821B4C8` / `0821B540` take a
previously decoded eight-byte reference, add its u16 field at offset 6 to the
caller's index for family 20, and dispatch to typed store/load respectively.
The reference's field at offset 4 is unused by these wrappers.

`0821B5BC` parses a reference, reads its current value, and writes that value
through a different base: descriptor selector 8 chooses the pointer at
`0200070C`; other selectors choose `02000704`. `0821B63C` reads from those same
alternate bases. Both use the descriptor's low 16-bit byte offset and family-20
u16 index field. These relationships establish two groups of base pointers,
not the semantic names or lifetime of the associated storage. No direct
counterpart to these alternate-base operations was identified in the reviewed
MGS variable code. MGS's reviewed type cases and fixed buffers do not justify
importing them into B3.

### Retained trap candidate and negative results

The hash/table `trap` candidate at `08225624` remains **unmatched**. Assembly
inspection and untracked C establish a 44-byte stack record and these local
operations: consume two u16 values; optional `m` value or default `0DD2`;
optional `t` with zero default; remap `?` in two fields to `14C9`, and `*` in
the `m` field to `1516`; collect up to four u16 values for each of `w` and `s`;
`b` sets flag `0x10` and stores a value; `e` decodes a block value; otherwise `p`
sets flag `0x20` and stores a value. It resolves the first consumed u16 through
`0821E104`, supplying a pointer to record offset `0x0E`; non-null resolution is
passed with the original command pointer to `0821E190` and returns 0, otherwise
it returns -1. The meaning of the numeric remaps, record fields, arrays, and
resolved pointer is **not** established. No candidate semantic names were
installed. MGS's `TrapCmd` is only a comparison lead; no behavior equivalence
or transferred field naming is claimed.

Matching the trap's pointer-based zeroing loops keeps a pointer in an extra
high register; integer countdown alternatives instead add counter instructions.
The typed load helper `0821B20C` also remains unmatched: struct-array accesses
fix address-add operand order, while boolean normalization still changes index
register lifetime. These failures are not evidence of a different compiler.

Round-2 bounded trap permuting improved the score from 1,840 to 1,110 in
60 seconds with two workers; no zero-score candidate was found. The improved
source remains untracked under `wip/`, alongside the independent base. A later round-5 semantic audit rejected this generated candidate because it
merged distinct record/vector pointers; see the explicit rejection below. It
must not be promoted or treated as evidence for a new compiler.

All eleven round-2 additions passed per-function checks, and the complete
worker ROM printed `build/boktai3.gba: OK` before separate per-TU commits.
The parent orchestrator handles the combined 35 screenshot comparisons.

## Round 3: engine initialization and lifecycle context

This round starts at `ace10e4`. It extends B3-native lifecycle context around
the verified script subsystem; no additional MGS ancestry or semantic names
are inferred from adjacency or generic scheduler structures.

| New byte-exact B3 target | Bytes | Static behavior |
|---|---:|---|
| `08225328` | 232 | Registry/commands/global initialization and callback registration |
| `0821A054` | 108 | Traversal of fourteen masked callback lists |
| `0821A218` | 108 | Conditional cleanup of eight-byte resource entries |
| `0821A490` | 88 | Relative-offset header copying and indexed lookup |

These four functions add **536 matched native bytes**. Candidate function names remain unchanged.

### Engine initialization

`08225328` first initializes the 722-entry registry count, calls `082250EC`
(the already matched wrapper calling `082258A0` / `08225E20`), installs the
eight engine commands, and installs the six basic commands. This establishes
a direct initialization chain joining both command tables and the callback
registry used by `chara`.

It then clears fourteen word globals in the `030053D8–03005414` region,
clears a word at dynamic memory-base `02000710` plus `0x868`, clears
`030053E8` again after that pointer store, and clears `030054B0`. It invokes
`08227DC4`, `0806EF04`, `082210B8`, and `08177D54`, whose semantics are not
newly established here. It stores u16 value 2 at memory-base plus `0x12`.
At `030025A0`, it installs `082250FC` and `08225324` at callback offsets
8/12 using `0821A04C`, writes 1 at offsets `0x14` and `0x16`, clears offset
`0x10`, inserts the record with `08219F74`, then clears its words at `0x18`
and `0x1C`. The already matched insertion routine indexes the callback-list
array by the byte at `0x14`, so this specifically registers in list 1.
The destruction callback `08225324` is an existing empty function.

### Callback traversal and resource cleanup

`0821A054` visits fourteen eight-byte list records at `03005280`. Each has a
head pointer and a mask. A list runs only when its mask AND the word at
`0300523C` is zero; this word is loaded again for each list, so callbacks may
affect later gates. For each node, the next pointer is saved before invoking
anything. A u16 flags word at node offset `0x12` selects normal callback offset
8 when bit 0 is clear. When bit 0 is set, it calls optional callback offset
12, unlinks through the already matched `08219F94`, then calls `08219D38`.
Both optional callbacks receive the current node pointer. `08219D38`'s
assembly marks a preceding 16-byte linked block header and coalesces adjacent
marked blocks, supporting a deallocation interpretation; no semantic symbol
name was installed and that function remains unmatched in this round.

`0821A218` visits eight-byte entries at `0203B000` using the signed count at
`030052F0`. Only entries with flag byte bit `0x80` set are processed. If entry
mask 1 is set and caller mask 1 is clear, it
retains the entry and records that entry's index. Otherwise entry mask 2
requests `08219D38(data)` before the u16 ID and flag byte are cleared. The
final count is the last retained index plus one; with no retained entry, it
becomes one. These conditions are exact observed code, not an inferred naming
of resource ownership or data type. Its count is reloaded after calls and at
loop tests. The already matched `082250DC` invokes it with zero during the
engine-update countdown-completion path.

### Relative-offset lookup

`0821A490` copies a native four-word header onto the stack. The first word is
used as a count; the next two words become base-relative pointers to a u16 key
array and a native-word relative-offset array. It calls `0821A454` with the
third input argument truncated to u16 as key, the key-array pointer, the fourth
input truncated to u16 as an extra context argument, low index 0, and high
index count minus one. The second input register is unused. The helper's
already matched implementation ignores that context argument and performs a
lower-bound search. Negative result or unsigned index >= count returns null;
otherwise it returns base plus the selected native-word relative offset.
The fourth header word is copied but unused. No file-size check or validation
of key sorting exists locally, and no new table semantic name was established.

### Unmatched engine update: all observed local behavior

`082250FC` remains an **untracked candidate**, not a byte-exact addition.
Its assembly establishes the following state machine on words at node offsets
`0x18` (state) and `0x1C` (countdown). The role as this initialized record's
update callback is verified by `08225328`; its broader game-system name is
not established.

* If `03005408` is zero and all low four bits of halfword `03005260` are set,
  it calls `08224FA4` and returns zero immediately, skipping the rest of the
  callback. A nonzero `03005408` is cleared before ordinary processing.
* State 0 clears `030054B0`, `030053E8`, and the countdown, sets `03003A08`
  to 1, calls `0822502C` and `0821B078`, then consumes the u16 procedure ID
  at `030039F0`. Nonzero invokes `0821AD08(id, 0)`; zero invokes `0821B004`.
  It clears `03003A08`, sets state 1, and clears `030053FC`. The original
  compares the saved ID again after the nonzero call, without rereading RAM.
* State 1 with countdown <= 0: nonzero `030053E8` and cleared mask 2 in
  `0300523C` allow `08224F2C`. Its zero result clears `030053DC` and
  `03005414`, sets `030053FC` to 1, clears masks 2/4/8 in `0300523C`, calls
  `0821A0F8(1)`, sets countdown 3, clears `0xE00` in halfword `03004BD8`,
  and writes `0x40` to `030051D8`. Otherwise, cleared mask 2 allows the word
  at `0300540C` to increment. A set mask 2 skips these updates.
* State 1 with positive countdown decrements it. Reaching zero calls
  `082250D8` and `082250DC`. If `030053F4` has mask `0x200`, it clears that
  mask and conditionally clears mask 8 in `0300523C`. Mask `0x10` in
  `030053E8` invokes `0821B148`; otherwise mask `0x100` invokes `0821B180`
  and stores the u16 from dynamic memory-base plus `0x5A4` via `0821B03C`.
  State becomes zero.
* For all ordinary state paths, nonzero `03005404` invokes `08228928` and is
  cleared. The unsigned word pointed to by `030053F8` increments through
  `0xFFFFFFFF` and then saturates. `030053E4` always increments. With nonzero
  `030053D8`, the signed word at dynamic memory-base plus `0x614` increments
  through `0x7FFFFFFF` and then saturates. The callback returns zero.

The already matched `0821B148` copies `0x89C` bytes from the pointer at
`02000710` to `0200070C`, and `0x400` bytes from `02000708` to `02000704`;
`0821B180` copies those regions in the reverse direction. This adds concrete
copy/restore context to round 2's alternate-base operations. Calling these
regions snapshots is a candidate interpretation, not evidence of exclusive
save-game ownership, scene semantics, or lifetime.

The update draft's first compound flag test initially collapsed to one nibble
comparison; separate goto checks reproduce the original three tests. Its
saved procedure-ID branch still collapses under ordinary C. A u8 inline
predicate keeps the repeated decision and saved registers but materializes an
extra boolean; changes to local widths and comparisons did not solve that
residual. No ASM, alternate compiler claim, raw ROM constants, or fake side
effects were introduced to force a match. The retained trap candidate was not
revisited because the assigned larger update/init targets took priority.

The `0821A184` entry allocator searches for a clear flag mask `0x80`, otherwise
increments the signed entry count and returns null when that incremented count
is greater than 31. Its observed counter/pointer register allocation remains
unmatched. A 60-second search improved score 40 to 20; the best candidate
fixes the counter/pointer allocation but swaps the saved loop-limit/mask
registers. A further explicit-limit draft worsened scheduling and was not
counted as a match. `0821A284` inserts ID/data and sets the flag byte's high bit
after allocation succeeds; it also remains unmatched due to saved-register
and constant ordering. Both independent and permuted candidates remain WIP.

Round 3 validation: all four new translation units passed exact matching; the
complete build printed `build/boktai3.gba: OK` before commits. Combined emulator
regression remains the orchestrator's batch validation.

## Round 4: engine update and adjacent structural families

Round 4 starts at `11bee1d` on `codex/mgs-gcl-round4`. All functions in the
following table were absent from tracked C at that base. The new total is
**14 byte-exact functions, 1,980 native bytes**, including pools/alignment as
measured by the exact checks. The engine state machine described in round 3
is now matched; that previous section remains the behavioral evidence.

| B3 start | Bytes | Established role / comparative evidence |
|---|---:|---|
| `08219AAC` | 120 | Initializes heap, scheduler, entry/cache state and list-0 update task |
| `08219C40` | 116 | Forward traversal and front split of free heap block |
| `08219CB4` | 132 | Reverse traversal and back split of free heap block |
| `08219D38` | 124 | Marks an allocation free and coalesces adjacent free blocks |
| `08219DD8` | 108 | Byte alignment, BIOS word fill, trailing-byte zeroing |
| `0821A184` | 68 | Finds reusable resource entry or increments bounded entry count |
| `0821A284` | 40 | Inserts 16-bit ID, data pointer, caller flags and active flag |
| `0821A520` | 332 | Ten key remaps, paired-word resource lookup and optional inner lookup |
| `0821AA1C` | 52 | Reads three script values into native 32-bit elements |
| `0821AA50` | 56 | Reads three script values into native 16-bit elements; `GCL_StrToSV` role |
| `0821B20C` | 164 | Descriptor-selected scalar/bit read; complements the previously matched stores |
| `082250FC` | 552 | Engine task update, procedure dispatch, request/countdown and counter state |
| `08225448` | 68 | Conditional counter reset and state-field setup following an 8-bit result |
| `082258DC` | 48 | Finds an actor-like linked record by a zero-extended 16-bit ID |

Additional pinned comparison files reviewed this round:
[memory.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgv/memory.c)
and [cache.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgv/cache.c).
The provenance/license restriction at the start of this document still applies:
these were inspected for comparison, with no upstream C copied or adapted.

### Engine update: resolved widths and allocation differences

The round-3 ordinary source removed the second saved procedure-ID decision.
A width-preserving unsigned left-shift test and explicit zero/past labels keep
both observed comparisons without observable extra effects. Shifting a
zero-extended 16-bit value by 16 is zero exactly when that value is zero.
Only the nonzero path can call `0821AD08`; the zero path calls `0821B004`.
The final C reproduces the original repeated comparison after the first call.

The value at current-memory offset `0x5A4` is loaded as signed 16-bit and
passed with sign extension to `0821B03C`. A caller declaration permitting the
full signed word is needed to reproduce `ldrsh`; declaring that argument as
u16 emitted `ldrh` and removed two bytes. The already matched callee stores
the low 16 bits at `030039F0`, so its minimal u16 source does not establish the
original source-level prototype. This is ABI/call-site evidence, not proof
that either independently reconstructed declaration is the historical type.

Explicit mask and halfword-pointer locals reproduce loading the full
`~0xE00` mask before the halfword read. The scheduler clear uses `~14` before
the global read. Finally, ordinary repeated global input references preserve
the original `ands` destination; GCC coalesces them to one observed load.
A bounded 90-second search improved the final score 15 to 0 by removing the
cached input local. Earlier switch, signed/unsigned, two shifted tests, and
boolean-inline variants either removed the repeated decision or emitted
extra boolean materialization. All were discarded, with no volatile fiction,
ASM, fake call effects, or alternate compiler claim used to force matching.

### Heap and resource-entry structure

The B3 heap header contains previous/next pointers, a size/flag word, and a
cleared word, totaling 16 bytes. Both allocation paths round the requested
size upward to 16 bytes and add a header. Bit 31 means free; the low 20 bits
supply block length. The front allocator follows next links and splits at
`block + requested_extent`; the back allocator first reaches the last block,
then follows previous links and splits at `block + block_length - extent`.
A remainder greater than 16 creates a separate header; a remainder of 16 or
less consumes the block. Allocation clears the final header word. Freeing
null or an already free block returns; otherwise it marks free, clears that
word and merges free previous and next neighbors while repairing links.
No input-size validation or externally promised maximum allocation was
established merely by the 20-bit stored length mask.

This differs from reviewed MGS `M_Sys`/`M_Unit` routines: MGS searches a
separate ordered allocation-unit array and derives lengths from adjacent
addresses, with array movement during splitting. B3's linked in-heap headers
are not a byte/layout-compatible counterpart. Similar allocation purpose is
insufficient to install MGS names or claim a shared heap implementation.

`08219DD8` handles leading unaligned bytes while signed length is positive,
then uses a local zero word with BIOS `CpuSet` control `0x05000000` plus the
low 21 bits of the aligned signed word count. It handles the final 0–3 bytes
separately. There is no top-level negative-length rejection: reviewed callers
use positive lengths, but malformed negative lengths are not made safe by the
matching source. Zero-word initialization belongs after the leading-byte
loop; tail decrement precedes pointer increment in the original instruction
schedule. Typed field checks through the newly assigned next link avoid
extra alias-driven reloads in the heap routines. One bounded 60-second search
resolved the front allocator's register swap by an ordinary scope wrapper;
the independently reconstructed back allocator/free routine matched after
field-expression changes.

The entry registry has 8-byte records: 16-bit ID, 8-bit flags, padding byte,
and native pointer. `0821A184` scans the signed current count for a record
without mask `0x80`. If none is reusable, it increments the count first and
returns null when the new count exceeds 31; failure does not roll back the
increment. The active-bit search does not verify ID uniqueness. `0821A284`
stores the low 16-bit ID and pointer, and stores the low flag byte of caller
flags OR signed `0x80`; its high bit therefore marks active. The previously
matched `0821A218` gives meaning to caller masks 1 (retention) and 2 (free data).

MGS `FindCache` instead uses 128 entries, ID modulo 128, 24-bit ID comparison,
and a first-unused cache pointer. B3's linear registry with incremented-count threshold 31,
separate flag byte and 16-bit ID differ materially. That threshold permits
a returned fresh index only when the old count is at most 30; physical array
capacity cannot be inferred from the comparison alone. Cache/resource roles are
comparisons only. The B3 allocator's bounded 90-second search reached score 0
from 20 with ordinary scope wrappers. The insertion search reached 0 from
255 by storing the OR expression directly instead of modifying a saved local;
that simpler independent source also passed the exact check.

### Resource lookup and script value readers

`0821A520` forms a 32-bit key from high 16-bit group and low 16-bit selector,
then calls the existing lower-bound lookup `0821A4E8` with bounds 0 and 10
against `08614D6C`. Ten inputs first replace that pair:

| Input group | Replacement group | Replacement low word |
|---|---|---|
| `922E` | `9225` | `5130` |
| `92B3` | `9305` | `D710` |
| `9B1B` | `9A65` | `4679` |
| `98F5` | `9B05` | `2117` |
| `A635` | `A705` | `6D24` |
| `AE6C` | `AF05` | `AC2C` |
| `C091` | `C305` | `E53E` |
| `CB05` | `C8E5` | `5F29` |
| `CEEF` | `CEE5` | `4F2D` |
| `CEAA` | `CF05` | `0A4D` |

For a remapped input, it calls `0821A490` on the result with the original
second input as the actual 16-bit lookup key. The other forwarded parameters
are the replacement group and replacement low word; `0821A490` ignores the
group, and `0821A454` ignores the forwarded context word. Thus those argument
positions do not establish selector/context behavior beyond this observed
path. There is no result-null guard before the inner header lookup. No
resource-category strings, historic file names, or MGS-equivalent lookup were
established for these constants. Signed switch promotion, mutating the native
16-bit parameters, and an explicit widened third-argument local resolve
branch/prologue/call scheduling. Two bounded searches only improved partial
scores or found no improvement; subsequent independent rewrites matched.

`0821AA1C` and `0821AA50` each decode exactly three successive operands,
store their values and update `02000610` to the final decoder cursor. Neither
checks returned type or early end in this routine. The first stores 32-bit
values; the second truncates to native 16-bit elements. The second is a close
role/control-flow correspondence to MGS `GCL_StrToSV` in `parse.c`, whose
three-value loop also stores shorts and updates the next-string pointer.
B3's already established operand encoding still differs from MGS. Indexed
three-element loops match; pointer-increment loops and explicit stack-pointer
locals introduced extra hoisting/register allocation. Array/struct stack
variants also added a saved register. A 45-second allocation search on a prior
vector candidate did not improve it; the later independent indexed form did.

`0821B20C` selects type from descriptor bits 24–27. Type 9 reads a
little-endian 32-bit value with stride 4; type 8 reads two little-endian bytes
with the same stride 4. Types 1/6 load signed native halfwords with stride 2;
types 2/3 load unsigned bytes. Type 4 adds descriptor bits 16–19 to the signed
index, selects byte at arithmetic `index >> 3`, and returns normalized bit
`1 << (index & 7)` as 0/1. Other types leave the output untouched. The function
does not check memory bounds. `bits != 0` matches and avoids the unnecessary
saved index in the older handwritten normalization. Reversing the equivalent
OR/negation operands also matched, while returning separately from cases did
not. MGS variable roles are useful comparison context, not encoding identity.

### Initialization, actor lookup and remaining negatives

`08219AAC` registers callback `08219A94` on list 0, clears scheduler/input
mode globals, writes 123456 to `03005278`, and fills four halfword samples at
`03005274` with `0x3FF`. The registered callback's existing matched source
calls `08219B24` and `0821A308` alongside two other functions. The value
123456 is observed initialization, not proof of a specific RNG algorithm.
An explicit start pointer/value/advance sequence reproduces the descending
sample-fill schedule; two pointer/index drafts did not.

`08225448` checks the low-byte result of `0822BBA8`, returns it, and on zero
clears 40 bytes if `030053F8` is nonnull, writes halfword 5 at current-memory
`0x12`, then writes 230 at counter-structure offset 4 and zero bytes at 8/9/A.
There is no second null guard before those subsequent stores. Counter and
state-field roles are observed; no exclusive gameplay subsystem name follows.

`082258DC` requires the context pointer at `030025F8`, takes its list at offset
`0x18`, compares each record's 32-bit first word with a zero-extended 16-bit
argument, and follows next at offset `0x44`. It returns the matching record or
null. This strengthens the linked actor/context interpretation near the
already documented callback registry, without proving exclusive ownership or
an identical MGS function correspondence.

`0821B084` remains WIP: it reads timer-3 count at hardware `0400010C`, takes
its low seven bits times eight as an offset into `0203C400`, and establishes
five views at base, base+`B00`, base+`1600`, base+`1A00`, base+`1E00` in
`02000710/0C/08/04/00`. This concretely gives a maximum initial offset of
1016 bytes; it is not a random-generator or encryption identification.
The 88-byte independent draft differs only in the initial commutative-add
operand order after splitting the timer/base expressions. Integer, halfword,
array-struct and expression-order variants did not fix it, and a 60-second
search produced no improvement. No matching C was tracked for this target.
The previous trap draft remains untouched and unmatched. Every candidate and
negative described above remains untracked WIP or ignored build evidence.

Round 4 validation: the complete build printed `build/boktai3.gba: OK` with
all 14 new units before commits. No worker screenshots were requested; the
orchestrator owns combined batch regression. No semantic symbol names were
installed, and no unsupported compiler/whole-engine lineage claim is made.

## Round 5: remaining inventory and rejected search candidates

Round 5 starts at `3c025ef` on `codex/mgs-gcl-round5`, with a ten-active-minute
bound. The assigned ranges have only three remaining unmatched function
starts: `08219B24` (180 bytes), `0821B084` (88 bytes), and `08225624`
(380 bytes). This round produced **zero new matches / zero native bytes**;
only the evidence documentation is committed. Independent valid C drafts and
ignored search output remain available separately. No source-level lineage,
compiler identification or new installed semantic names follow from failures.

### Input update: B3 evidence and MGS pad comparison

The pinned additional source reviewed was
[pad.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgv/pad.c).
Its `GV_UpdatePadSystem` calculates pressed bits from current AND NOT previous,
and released bits from previous AND NOT current, then writes pad state. This
supports a role comparison for B3's same bit formulas; MGS has four pad-state
outputs, packed channel shifts, analog/configuration handling and six-frame
quick-button history that are not present in the reviewed B3 routine.

B3 `08219B24` always reads hardware `REG_KEYINPUT` and writes its raw halfword
to `03005270`. With `03005250` zero, it XORs the raw value with `0x3FF`,
truncates to 16 bits, and updates the record at `03005260`: held at offset 0,
newly pressed at 2, newly released at 4, stride 8. It then clears held/pressed
in the second record, storing that record's previous held value as released.
With `03005250` nonzero, scheduler mask 1 suppresses both record updates;
otherwise two halfword samples beginning at `03005274` supply the values.
Sample `0xFFFF` means held zero; other samples are XORed with `0x3FF`.
Both records get the same previous/current difference formulas. Replay/demo
input is a candidate interpretation of the sampled path, not a verified
exclusive owner of `03005250` or the sample buffer.

Independent struct and integer-width drafts reproduce the control flow but
retain initial hardware-result moves, missing/differently scheduled 16-bit
normalization, and pointer/register choices. Signed raw, scoped assignment,
register keyword, indexed records and a small independent inline update
helper did not match. No externally reconstructed pad code was copied.
A bounded 75-second search scored 1685 then 1640/1570/1475 without a match.
The best generated candidate inserts `if (keys)` before `keys` is initialized.
Even though the branches contain identical assignments, that uninitialized
read is undefined behavior and the candidate is **rejected**. It is not a
valid WIP improvement or a reason to make the original memory volatile.

### Trap comparison and explicit semantic rejections

This round rechecked `TrapCmd` and `NTrapCmd` in the pinned `game/script.c`.
The optional `m`, `b`, `s`, `t`, `p`, and `e` processing in `NTrapCmd` gives
more specific comparative context than MGS `TrapCmd`'s fixed positional
arguments. The layouts still differ: MGS fills a persistent `HZD_BND` entry
and installs a bind array, while the observed B3 path constructs a 44-byte
stack record, gathers two arrays of up to four native halfwords and resolves
through `0821E104` before passing record/top to `0821E190`. MGS `NTrapCmd`
also handles `d/r/i/c` and different flag bits; neither record-field names nor
numeric wildcard values can be transferred from that source merely because
option letters overlap. B3's `b` flag is `0x10`, and its `p` flag is `0x20`.
The B3 `e` branch has priority over `p`; no MGS diagnostics were observed here.

Indexed zeroing, explicit local bounds, an early initialized loop variable,
and moving the second array base initialization later did not match. These
valid drafts emitted 384–396 bytes and changed saved high-register/loop-counter
allocation. A bounded 90-second search on the valid independent base improved
score 2050 through 1985/1860/1683/1570 to 1150; no exact candidate was found.

A semantic audit found two pointer substitutions that are unacceptable:

* The old round-2 score-1110 output substitutes the `out` variable for the
  second-array base, then overwrites it with `&trap.fe`. Its following zeroing
  and `s` reads therefore target the output halfword rather than `trap.s`.
* The round-5 score-1150 output replaces saved `out` with traversal pointer
  `p`, initializes it to `&trap.fe`, then immediately reuses it for vector
  loops. The later `0821E104` call receives that traversal result instead of
  the required pointer at record offset `0x0E`.

Both are **rejected**, regardless of improved assembly scores. Neither is
tracked C, a semantic match, or a candidate for integration. The independent
base retains separate `w`, `s`, traversal `p`, and saved output-field pointers.
Future work must audit every generated source diff for valid pointer lifetimes,
initialized reads and observable behavior before considering its exact score.
No rewrite of the actual data layout or unsafe pointer merge is authorized.

### Timer setup: remaining two-byte operand-order issue

`0821B084` retains round 4's five memory views and timer-derived offset evidence.
Modulo, signed/halfword/integer casts, outer scope, scalar/array/field base,
split arithmetic, final-local assignment, repeated global-base references and
inline offset helper variants did not match. The simplest valid 88-byte draft
still differs only at `0821B090`: it emits `adds r1, r0, r1` where the ROM has
`adds r1, r1, r0`. A new bounded 60-second search remained score 10 without
saving an improved candidate. The ordinary repeated-global version worsened
other register choices. No asm, register binding, extra hardware read or fake
volatile side effect was introduced to force the commutative instruction.

Round 5 validation: the unchanged matching sources completed a full build with
`build/boktai3.gba: OK` before the documentation commit. No new screenshots,
shared-gen regeneration, push, PR or nonmatching tracked C were produced.


## Round 6: adjacent expression, trap and resource relationships

The reserved extension targets were `0821BA5C`, `0821E040`, `0821E190`,
`0821E3C0`, `0821E52C`, `0821E5C4`, `0821E860`, `0821E8C8`,
`0821F15C`, and `082210B8`. Inventory excluded functions already in C in
both this checkout and the current main checkout. The parent confirmed these
ten addresses were excluded from the other worker's ownership. Original
round-5 input, timer and trap-command misses were not revisited.

All source was independently reconstructed from B3 instructions. The pinned
reconstructed MGS source supplies comparative evidence only; no external C
was copied or adapted, and no new semantic symbol names were installed.

### Verified byte-exact functions

| B3 address | Native bytes | Observed role |
| --- | ---: | --- |
| `0821BA5C` | 160 | Expression stack evaluation |
| `0821E040` | 144 | Pack fourteen trap callback arguments |
| `0821E190` | 360 | Insert a trap record and its associated top pointer |
| `0821E3C0` | 276 | Export rectangle limits and centers through nine references |
| `0821E52C` | 104 | Test horizontal rectangle membership |
| `0821E860` | 104 | Export three decoded vector components |
| `082210B8` | 88 | Initialize the random-value table and two cursors |

The total is **7 new matches / 1,236 native bytes**.

### Expression correspondence and encoding differences

The control flow and two-word value/reference slots of B3 `0821BA5C` strongly
correspond to [MGS GCL_Expr in libgcl/expr.c](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/expr.c).
Both save each operand's bytecode pointer, reduce two slots on binary
operations, retain the assigned right-hand value, and clear the reference
after a nonassignment operation. This supports the evaluator role; it does
not establish identical grammar or whole-engine ancestry.

B3 uses one-byte operators selected by `(byte & 0xE0) == 0xA0`, with the
operation in the low five bits. Operation zero terminates, and `0x16` assigns.
MGS uses a `GCL_OP` marker followed by a separate operation byte, with
`OP_SET` equal to 20. B3 advances one byte after an operator; MGS advances
two. B3's stack frame is 72 bytes: eight native eight-byte value/reference
slots and two four-byte decoder outputs. MGS locates its stack in scratchpad
RAM. Neither observed path checks expression-stack height. Malformed scripts
can therefore exceed the B3 local storage; no invented guard was added.

A B3 assignment whose saved reference has high nibble `0x90` writes a
procedure argument through `0821A91C`, indexed by its low nibble. Other
references use `0821B3E4`. Decoder type `0x80` additionally executes a block
through `0821AF2C` and takes the shared return word at VM offset four. These
branches are absent from the compared MGS expression loop. Internal slots
are native 32-bit memory words; bytecode decoding remains delegated to the
already documented B3 decoder, so native slot widths do not imply little-endian
bytecode operands.

The matching C uses integer address arithmetic for the initial cursor below
the array and each eight-byte movement. The stored cursor representation is
not dereferenced until a valid slot is reached. All slot offsets are computed
as integers, avoiding undefined C pointer arithmetic before the first
array element. The block result uses `gUnk_02000610[1]`, preserving the
verified VM base plus four rather than inventing a separate global literal.
Initial integer-cursor and pointer-cursor drafts changed register copies;
separating reference byte, category and saved slot after masking matched the
assignment path without extra reads or side effects.

### Trap callbacks, insertion and rectangle evidence

`0821E040` constructs fourteen native 32-bit arguments in this order:
trigger ID, trap field at offset eight, event value, three signed position
halfwords, four unsigned halfwords at trap offset `0x10`, and four unsigned
halfwords at `0x18`. The event and IDs are truncated to unsigned 16 bits;
position values are sign extended. Its eight-byte argument header has a
16-bit count field and a value-array pointer; the upper 16 bits of the count word are
preserved by the generated field update. Flag `0x20` selects `08224F20`;
otherwise it calls `08224F08`. Those existing wrappers route to procedure
execution `0821AD08` and block execution `0821AFB4`, respectively.
A combined packet struct allowed a halfword store and different saved
registers; separate local value and header declarations matched the original
word-field update and register allocation.

`0821E190` maintains a maximum of 64 records: it returns when the current
unsigned halfword count exceeds 63. Each record is 44 bytes, beginning at
context offset `0x224`; 64 associated top pointers begin at `0xD24`. A
32-bit serial counter at `03005300` is repaired from zero to one, written
into the incoming record, and postincremented. Searching backward by the
record's unsigned ID at offset eight selects insertion immediately after
the last matching ID; otherwise the record appends. It shifts both record
and pointer arrays backward before inserting and increments the count.
The serial wraps naturally as a native unsigned word, with a later insertion
repairing zero. A do-loop draft emitted 336 bytes and missed the peeled
comparison; a while loop plus ID-first equality reproduced all 360 bytes.
The correspondence with MGS trap registration is a subsystem relationship,
not evidence that MGS's HZD_BND layout or flag names apply to this record.

Existing `0821E104` searches the context's offset-`0x0C` table. Its header
contains a byte count and three padding bytes; each 12-byte record consists
of four signed horizontal halfwords, two unsigned vertical bytes, and a
16-bit ID. It returns the first matching record and counts all matching IDs.
`0821E52C` then scans that many consecutive records, using lower-inclusive,
upper-exclusive tests on position components zero and two. It ignores the
vertical limits. Thus this caller assumes matching IDs are contiguous; the
search helper alone does not prove that property. Reversing the for-loop
update order to pointer increment before the 16-bit loop counter matched
its original 104 bytes.

`0821E3C0` requires a nonnull script operand cursor, decodes the ID, and
returns -1 if no rectangle exists. It decodes nine references and stores:
first horizontal limit, first vertical byte shifted left four, second
horizontal limit, corresponding upper limits in the same order, and three
centers. Horizontal centers average signed 16-bit limits with division
truncated toward zero. The vertical center averages the unsigned bytes
then shifts left four. Small independently written inline half/half-and-shift
helpers preserved the original distinct result registers; a shared local
conditional average left three instructions mismatched.

`0821E860` decodes two script values and calls `0821E7EC` with a four-halfword
local vector. Failure returns -1; success decodes three variable references
and writes the first three signed vector halfwords, returning zero. Keeping
a saved vector base for the last two outputs matched the original 104 bytes.
The previously matched helper chain `E7EC -> E748/E76C/E780` supplies its
table-entry and vector relationship; no stronger map/actor name is inferred.

### Random-state initializer and unresolved resource helpers

`082210B8` initializes 1,024 signed halfwords at `0203B400`. Each element
uses two calls to existing `08219C00`: a returned 15-bit value shifted right
by a second returned value masked with three. Two final calls masked with
1023 initialize cursors `03005308` and `03005304`. This is 2,050 generator
calls. Existing `08219C00` advances `03005278` as unsigned
`state * 0x5D588B65 + 1` and returns `state & 0x7FFF`. Consequently the
round-4 initializer's 123456 write to that state is now evidence for a PRNG
seed, replacing the earlier deliberately inconclusive interpretation. This
does not identify encryption or an external historical RNG implementation.
A 60-second search found an ordinary do scope around the element store;
its source diff changes no reads, calls, pointer lifetime or computation.
The distilled scope independently checked exact before promotion.

`0821E5C4` initializes a 12-byte state from the context's offset-`0x10` table:
unsigned halfword table count, four-byte entries containing a byte count and
a halfword offset, and eight-byte selected elements. Invalid entry index or
zero element count returns zero. An oversized element index is clamped to
zero. State bytes zero/one/two receive index, argument and element with
byte truncation; byte three is cleared. The entry pointer goes at four, and
`table + entry.offset + stored_element * 8` at eight. Independent typed and
byte-pointer drafts emitted 80 bytes against the original 84, missing a
saved entry copy. A bounded permuter attempt failed with its AST assertion
`nodes should only appear once in AST`, saving no candidate.

`0821E8C8` tests absolute signed-horizontal differences between position
components zero/two and the first two signed halfwords of state data. Each
absolute difference is compared unsigned against its supplied limit; either
strictly greater returns one, otherwise zero. Original instructions preserve
unused upper bits in two registers before sign-extending the low halfwords.
Independent signed and unsigned 16-bit bitfield drafts read only assigned
fields, but optimized one field into a direct load and emitted 72 bytes
against the original 88. No uninitialized full-word read or fake volatile
operation was introduced to recreate unused upper bits.

`0821F15C` fetches resource key `0xF63B` for a decoded unsigned halfword
argument through `0821A520`, stores the result at context offset `0x14`,
and returns zero. A native pointer/word draft differs by an extra original
result move; an ordinary do scope did not resolve it. A bounded 60-second
search produced byte-exact candidates by duplicating the same store and
return under `if (resource)` and its else branch. This condition is initialized
and the branches are behaviorally equal, unlike the rejected round-5
candidates, but the artificial duplicated control flow is not promoted.
A compact source representation remains WIP. Resource key spelling and
higher-level ownership remain unresolved.

Round 6 validation: all seven new translation units completed a full native
build with `build/boktai3.gba: OK` before one-TU commits. No worker screenshots,
shared-gen regeneration, push, PR or nonmatching tracked C were produced.


## Round 7: bounded final adjacency evidence

The final six-minute pass started at 2026-10-09 04:07:14 UTC on
`codex/mgs-gcl-round7` from `0f8f5b6`. It produced **zero new matches /
zero native bytes**. The parent reserved direct siblings `0821F1C4` and
`0821F204` after checking they remained ASM in both main and this checkout.
The account reached the 35-percent stop floor during the assignment; no new
target, worker, round or permutation search was started afterward.

The retained `0821E5C4` draft was tested with an unsigned-word saved entry
address, converting back only for the offset read and state pointer store.
It remained 80 bytes versus 84, lacking the native saved-entry register copy.
`0821E8C8`'s assigned-field-only bitfield draft remains valid WIP; review
confirmed unused preserved upper bits cannot justify an uninitialized full
word read. The `0821F15C` ordinary scope remains mismatched by its resource
result move. The prior artificial identical branches were not reintroduced,
and no new permutation fishing was performed.

### Symmetric halfword table helper

`0821F1C4` returns zero for equal signed input indices. Otherwise, let
`lo = min(first, second)`, `hi = max(first, second)`, and `n` be the unsigned
16-bit width argument. It reads the native unsigned halfword at table index
`n * lo + hi - ((lo + 1) * (lo + 2) >> 1)`. The formula indexes an upper
triangle without the diagonal, symmetrically for swapped input indices.
The native routine does not check index or width bounds; integer overflow
and invalid negative indices are not given invented semantics by this finding.
This establishes the indexing structure, not the table's gameplay meaning.

Independent separate index/triangle locals, mutation of width and indices,
a small inline triangular-product helper, a signed width with explicit
16-bit normalization, sequential product assignments and integer address
calculation emitted 56, 60 or 64 bytes. None matched the original 64 bytes.
Differences include copying the first input to r0, preserving normalized
width separately, multiplication operand selection and the final load base.
No register binding, asm, volatile side effect or duplicated branch was used.

### Resource rectangle-group membership

`0821F204` reads the same context offset-`0x14` resource stored by `F15C` and
`F180`. Its resource starts with an unsigned halfword group count and a
four-byte-aligned array of native 32-bit relative offsets beginning at four.
Each selected group has an unsigned halfword rectangle count and another
native 32-bit relative offset at group offset eight. Rectangle records are
16 bytes, as supported by already matched `F18C`; their first four bytes
are lower and upper horizontal limits shifted left eight before comparison
against signed halfword position components zero and two. Bounds are
lower inclusive and upper exclusive. Native offset tables are memory layouts,
not evidence of bytecode endianness or MGS format compatibility.

Before scanning groups, F204 arithmetic-shifts signed position halfwords
right eight to obtain horizontal coordinates. Negative or out-of-range
coordinates select cell zero rather than returning immediately. Otherwise
a row halfword at context offset `0x24` plus the first coordinate supplies
the cell index. It calls existing `D64C(cell, 1)`; a returned dynamic node
provides its four-byte cell record at offset four, or the fallback record
is at `context.grid + 12 + cell * 4`. Cell flag two suppresses the group
scan. The first containing rectangle terminates the scan: return one only
if its group index equals the incoming object's unsigned halfword at offset
four, otherwise zero. It does not continue searching later groups after a
hit in a different group. The second incoming register argument is unused.
No null-resource guard or different invalid-coordinate policy was added.

For-loops, explicit success/failure labels, integer group-offset addressing
and explicit loop labels yielded valid independent drafts of 216–232 bytes
against the original 232. Ordinary loop forms introduced an induction pointer
and a stack spill for the group index; label placement could eliminate that
spill, but flag-register selection, loop allocation and branches remained
different. This resource relationship extends the established B3 script
subsystem evidence. A direct MGS function identity or HZD structure name
is **not verified** and was not transferred from the comparative source.

All five valid drafts remain ignored WIP; no nonmatching C was installed.
The unchanged matching tree completed `tools/build.py` with
`build/boktai3.gba: OK` before this documentation-only commit. Parent performs
combined runtime regression; no screenshots, push, PR, shared-gen changes
or new semantic symbols were produced.
