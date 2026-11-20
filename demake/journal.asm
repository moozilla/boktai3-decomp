; Fixed-bank page directory entries are mapper bank, address low, address high.
; Each ROM page carries its own 2bpp dictionary and dense 20x18 tile map.
open_map:
    ld a,4
    ld [mode],a
    xor a
    ld [note_index],a
    jp draw_journal

journal_tick:
    call input
    ld a,[pressed]
    bit 3,a
    jp nz,journal_close
    ld a,[mode]
    cp 6
    jr nz,.which
    ld a,[pressed]
    and 3
    jr nz,.map
    jr .idle
.which:
    cp 4
    jr nz,.text
    ld a,[pressed]
    bit 2,a
    jr nz,.save
    bit 1,a
    jp nz,journal_close
    bit 0,a
    jr z,.idle
    ld a,[note_count]
    and a
    jr z,.idle
    ld a,5
    ld [mode],a
    jr .draw
.save:
    call save_store
    call lcd_off
    ld a,6
    ld [mode],a
    ld hl,saved_page
    call show_journal_page
    jp loop
.text:
    ld a,[pressed]
    bit 1,a
    jr nz,.map
    bit 2,a
    jr z,.page
    ld a,[note_lang]
    xor 1
    ld [note_lang],a
    xor a
    ld [note_index],a
    jr .draw
.page:
    bit 0,a
    jr nz,.next
    ld a,[pressed_dir]
    bit 0,a
    jr nz,.next
    bit 1,a
    jr z,.idle
    ld hl,note_index
    ld a,[hl]
    and a
    jr z,.idle
    dec [hl]
    jr .draw
.next:
    ld hl,note_index
    inc [hl]
    ld a,[note_count]
    cp [hl]
    jr nz,.draw
.map:
    call open_map
    jp loop
.draw:
    call draw_journal
    jp loop
.idle:
    call vblank
    jp loop
journal_close:
    call restore_field
    jp loop

draw_journal:
    xor a
    ldh [0x25],a
    call lcd_off
    ld a,[bank]
    ld [0x2000],a
    ld a,[header+18]
    ld e,a
    ld a,[header+19]
    ld d,a
    ld hl,journal_ptrs
    add hl,de
    ld a,[header+20]
    ld [note_count],a
    ld a,[mode]
    cp 4
    jr z,.map
    ld de,3
    add hl,de
    ld a,[note_lang]
    and a
    jr z,.index
    ld a,[header+21]
    and a
    jr z,.index
    ld [note_count],a
    ld a,[header+20]
    ld e,a
    ld d,0
    add hl,de
    add hl,de
    add hl,de
.index:
    ld a,[note_index]
    ld e,a
    ld d,0
    add hl,de
    add hl,de
    add hl,de
    jp show_journal_page
.map:
    push hl
    ld hl,0xc900
    ld bc,160
    call clear
    ld a,[player_z]
    ld l,a
    ld h,0x73
    ld a,[hl]
    ld [0xc900],a
    ld a,[player_x]
    ld l,a
    ld h,0x72
    ld a,[hl]
    ld [0xc901],a
    ld a,118
    ld [0xc902],a
    ld a,4
    ld [dma_count],a
    call 0xff80
    pop hl
show_journal_page:
    ld a,[hl+]
    ld [0x2000],a
    ld a,[hl+]
    ld h,[hl]
    ld l,a
    ld a,[hl+]
    ld c,a
    ld a,[hl+]
    ld b,a
    ld de,0x8800
    call copy
    ld de,0x9800
    ld b,18
.row:
    push bc
    ld bc,20
    call copy
    ld a,e
    add 12
    ld e,a
    jr nc,.next
    inc d
.next:
    pop bc
    dec b
    jr nz,.row
    xor a
    ldh [0x42],a
    ldh [0x43],a
    ld a,[mode]
    cp 4
    ld a,0x81
    jr nz,.show
    ld a,0x87
.show:
    ldh [0x40],a
    ret
