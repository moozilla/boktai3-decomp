# Wireless connection controller

52 new exact functions complete every named inventory function in
08034084–08036CA4. Coverage rises from 20.406% to 20.704%, a gain of
0.298 percentage points (7,188 reported code-span bytes). Total: 5,678 exact
functions. The existing unnamed assembly tails remain assembly; this does not
claim that those tails were rewritten in C.

## What the unit does

The controller allocates a 0x38C-byte task, configures a wireless game identity,
searches for peers and exchanges IDs/names. Separate parent and child event
callbacks react to RFU search, connection, disconnect, transfer and error
notifications. Its update task selects a connection, UNI or NI session updater
and dispatches the current operation through gUnk_086056EC. Shutdown unlinks
and frees the task. Function roles in symbols/proposed/subsystems.csv are
provisional descriptive names; linker symbols remain stable.

UNI packets carry sequence counters and input history. The parent receives
one packet per connected child, tracks duplicate/missed sequences, and builds
a combined packet. The child accepts the next sequence and counts participants.
The update path publishes five input entries to gUnk_03005274, using bit 0x2000
as a validity flag and extracting the lower twelve bits. Missing entries become
0xFFFF. NI transfer routines send/receive twelve-byte blocks, invoke completion
or failure callbacks, and record slots lost during transmission.

## Useful layout and entry points

The following names are tentative interpretations of observed offsets:

| Controller offset | Role |
|---|---|
| 0x20 / 0x21 | Parent send slot / connected parent slot |
| 0x22 | Handshake/send flags |
| 0x30 | Active session flag |
| 0x31 / 0x32 | Input ready / entering a new operation |
| 0x33 / 0x34 | Error/status / session mode (connection, UNI, NI) |
| 0x35 / 0x36 | Lost slots / send-ready flag |
| 0x38 | Operation dispatched through the callback table |
| 0x44 / 0x48 | Operation timer / overall frame counter |
| 0x70 / 0x80 / 0x90 | Parent outbound / child inbound / child outbound UNI records |
| 0xB8 / 0xC4 | Twelve-byte NI send / receive buffers |
| 0xD0–0xDF | Per-peer sequence and duplicate/missed counters |
| 0xE6–0xE9 | Received/acknowledged slot masks and pending disconnect state |
| 0xEA / 0xEC / 0xF0 | Previous aggregate input / current aggregate / five-entry history |
| 0x134 | Peer handshake and participant data |
| 0x174 | Larger exchange packet passed to packet encode/decode helpers |
| 0x378–0x384 | Send/receive completion and failure callbacks |

Useful entry points: Wireless_CreateController (08036910),
Wireless_UpdateControllerTask (080367B4), Wireless_SetTransferCallbacks
(08036B2C), and Wireless_QueueNiPacket (08036B64). See the function-name CSV
for state-handler names. The global gUnk_02000484 owns the singleton;
gUnk_020000EC synchronizes with wireless callbacks. Busy waits use volatile
reads so asynchronous updates remain visible.

RFU SDK types come from include/rfu_compat.h. The game API occasionally needs
wide local prototypes even where the SDK callee narrows arguments. Register
pinning and empty compiler constraints retain the original allocation and
instruction order. Range switches retain strict comparison boundaries that
ordinary C comparisons normalized by one. There are no handwritten instruction
bodies. Layout consolidation and concession removal can follow later.

## Validation

Every added translation unit passed its reviewed-extent exact byte check after
formatting. One combined ROM build printed `build/boktai3.gba: OK`. No new tooling
or broad regression work was added.
