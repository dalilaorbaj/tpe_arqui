//Fuente: https://wiki.osdev.org/PC_Speaker
#include <stdint.h>
#include <sound.h>

//Play sound using built-in speaker
 static void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	out(0x43, 0xb6);
 	out(0x42, (uint8_t) (Div) );
 	out(0x42, (uint8_t) (Div >> 8));
 
        //And play the sound using the PC speaker
 	tmp = in(0x61);
  	if (tmp != (tmp | 3)) {
 		out(0x61, tmp | 3);
 	}
 }
 
 //make it shut up
 static void nosound() {
 	uint8_t tmp = in(0x61) & 0xFC;
     
 	out(0x61, tmp);
 }
 
 //Make a beep
 void beep(uint64_t freq, uint64_t time) {
 	 play_sound(freq);
 	 sleepTicks(time);
 	//for (volatile uint64_t i = 0; i < time * 100000; i++); // Espera simple

	 nosound();
	 return;
}
