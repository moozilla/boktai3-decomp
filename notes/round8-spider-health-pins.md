# Spider pursuit and health restore entry

Tags: #round8 #matching #compiler

Spider_BeginPlayerPursuit at 080A3A30 matches all 464 bytes with stock agbcc.
Capturing the pursuit mask after the navigation test keeps it live over terrain
resolution. Grouping the context flag update and timer reset keeps the context
in r8 and the mask in r9.

The health restore entry at 08234DC8 matches all 260 bytes with stock agbcc.
The sprite scale uses the word-sized increment while the 16-frame bound tests
the stored halfword. Capturing both values at those widths preserves the copy
before the test and the addition of 34 in the sliding-sprite path.
