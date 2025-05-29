#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include <video.h>
#include <syscalls.h>

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

int main(){	
	
	load_idt();
	

	
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
	((EntryPoint)shellCodeModuleAddress)();
	// testKeyboard();

	write("[Shell terminada]\n", 18);
	
	return 0;
}

static void testKeyboard() {
    write("Presiona una tecla (test directo):\n", 36);
    uint16_t c;
    
    // Test directo de sys_read
    while(1) {
        int result = sys_read(STDIN, &c, 1);
        if(result > 0) {
            write("Tecla detectada: ", 17);
            char temp[2] = {(char)c, '\0'};
            write(temp, 2);
            write("\n", 2);
            break;
        }
        // Pequeña pausa para no saturar
        for(int i = 0; i < 1000000; i++);
    }
}
