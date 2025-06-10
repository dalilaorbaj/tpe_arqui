#include <stdint.h>
#include <idtLoader.h>
#include <defs.h>
#include <interrupts.h>

#pragma pack(push)
#pragma pack (1) 

extern void _lidt(void*);
static void setup_IDT_entry(int index, uint64_t offset);


typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop)

#define IDT_ENTRIES 256
static DESCR_INT idt[IDT_ENTRIES];

#pragma pack(push,1)
typedef struct {
  uint16_t limit;
  uint64_t base;
} IDTR;
#pragma pack(pop)

static IDTR idtr = {
  .limit = sizeof(idt) - 1,
  .base  = (uint64_t)&idt
};

void load_idt() {

  _cli();

  setup_IDT_entry (0x20, (uint64_t)&_irq00Handler); //timer tick
  setup_IDT_entry (0x21, (uint64_t)&_irq01Handler); //keyboard
  setup_IDT_entry (0x00, (uint64_t)&_exception0Handler); //división por cero
  setup_IDT_entry(0x06, (uint64_t)&_exception_invalidOpcodeHandler); // Opcode inválido
  setup_IDT_entry(0x08, (uint64_t)&_exception8Handler); // Doble fault

  idt[0x80].selector  = 0x08;
  idt[0x80].offset_l  = (uint64_t)&_irq80Handler & 0xFFFF;
  idt[0x80].offset_m  = ((uint64_t)&_irq80Handler >> 16) & 0xFFFF;
  idt[0x80].offset_h  = ((uint64_t)&_irq80Handler >> 32) & 0xFFFFFFFF;
  idt[0x80].access    = ACS_SYSCALL;
  idt[0x80].cero      = 0;
  idt[0x80].other_cero= 0;

  _lidt(&idtr);

	//Solo timer tick y keyboard habilitadas
	picMasterMask(0xFC); 
	picSlaveMask(0xFF);
      
	_sti();
}


static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}
