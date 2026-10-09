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
source remains untracked under `wip/`, alongside the independent base. This
is a retained candidate, not a byte-exact match or evidence for a new compiler.

All eleven round-2 additions passed per-function checks, and the complete
worker ROM printed `build/boktai3.gba: OK` before separate per-TU commits.
The parent orchestrator handles the combined 35 screenshot comparisons.
