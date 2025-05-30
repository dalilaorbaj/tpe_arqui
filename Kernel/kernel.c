#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include <video.h>
#include <syscalls.h>
#include <sound.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

/* Dirección para la shell */
static void * const shellCodeModuleAddress = (void*)0x400000;
static void * const shellDataModuleAddress = (void*)0x500000;

static void print_reg(const char *name, uint64_t value);


typedef int (*EntryPoint)();



void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		shellCodeModuleAddress,
		shellDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

static void testKeyboard();

//dsp borrar
static void print_reg(const char *name, uint64_t value) {
    char buffer[40];
    int i = 0, j;
    // Copiar el nombre
    while (name[i] != 0) {
        buffer[i] = name[i];
        i++;
    }
    buffer[i++] = ':';
    buffer[i++] = ' ';
    buffer[i++] = '0';
    buffer[i++] = 'x';

    // Convertir a hexadecimal (16 dígitos)
    for (j = 0; j < 16; j++) {
        int shift = (15 - j) * 4;
        uint8_t digit = (value >> shift) & 0xF;
        buffer[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
    }
    buffer[i++] = '\n';
    write(buffer, i);
}

int main(){	
	
	load_idt();
	
	//para probar snapshot en Kernel
	RegsSnapshot regsSnapshot;
	sys_get_registers(&regsSnapshot);
	write("SNAPSHOT:\n", 15);

    print_reg("rax", regsSnapshot.rax);
    print_reg("rbx", regsSnapshot.rbx);
    print_reg("rcx", regsSnapshot.rcx);
    print_reg("rdx", regsSnapshot.rdx);
    print_reg("rsi", regsSnapshot.rsi);
    print_reg("rdi", regsSnapshot.rdi);
    print_reg("rbp", regsSnapshot.rbp);
    print_reg("rsp", regsSnapshot.rsp);
    print_reg("r8",  regsSnapshot.r8);
    print_reg("r9",  regsSnapshot.r9);
    print_reg("r10", regsSnapshot.r10);
    print_reg("r11", regsSnapshot.r11);
    print_reg("r12", regsSnapshot.r12);
    print_reg("r13", regsSnapshot.r13);
    print_reg("r14", regsSnapshot.r14);
    print_reg("r15", regsSnapshot.r15);
    print_reg("rflags", regsSnapshot.rflags);
	

	write("[Cargando la shell]\n", 20);
/*
	ncPrint("[Cargando la shell]");
	ncNewline();

	ncClear();

	//Llamada a la shell
	((EntryPoint)shellCodeModuleAddress)();
		
	ncPrint("[Shell terminada]");
	ncNewline();

	ncPrint("[Finished]");
*/
	//((EntryPoint)shellCodeModuleAddress)();

	//write("[Shell terminada]\n", 18);
	
	return 0;
}

