//******************************************************************************
// Archivo: idtLoader.h
//******************************************************************************

#ifndef _IDTLOADER_H_
#define _IDTLOADER_H_

//******************************************************************************
// DECLARACIÓN DE PROTOTIPOS
//******************************************************************************

static void setup_IDT_entry(int index, uint64_t offset);
void load_idt();

#define ACS_INT       0x8E    // P=1, DPL=0, type=14 (interrupt gate)
#define ACS_SYSCALL   0xEE    // P=1, DPL=3, type=14 (interrupt gate)

#endif // _IDTLOADER_H_
