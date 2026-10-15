# Scene range watcher (tentative)

The contiguous `0806F7B0–0806F990` task family is exact over all480 bytes.
The new228-byte initializer closes the family; the other252 bytes were already
matched and now have shared types and corrected script argument construction.

The task watches the signed game-state field876 against an inclusive byte range.
Script keyword `p` supplies entry/exit script IDs, each defaulting to zero;
uppercase `A` supplies two optional arguments, narrowed to16 bits before storage
as words. Missing `A` clears both words. Keyword `u` supplies the minimum and
maximum: when present, each missing value defaults to1; when absent, the bounds
are1 and8. Keyword `f` supplies the interval, default0. Initialization clears
the frame counter and remembered inside/outside state.

Each tick checks the range when interval is zero or the unsigned16-bit frame
counter is divisible by the interval, then increments that counter with its
original halfword wrap. A change in range membership dispatches the applicable
script with three words: the current signed value and the two stored arguments.
The remembered state changes only when that script ID is nonzero; a missing
entry/exit script therefore leaves the previous state intact. Task creation
uses list8 and size2C; destruction is a no-op returning zero.

The old range-check source read an uninitialized whole flags word while setting
the script argument count. It now assigns the established count bitfield in
`ScriptUtilityArguments`, leaving its unused upper half unspecified. This
preserves the original compiler's read/modify/write representation without a
C expression reading an indeterminate scalar. The complete136-byte body remains
exact ordinary C. Initializer depinning reduced twelve hints to three, retaining
only the initialized maximum-field address lifetime in the missing-A branch.
No runtime coverage is inferred from these static contracts.
