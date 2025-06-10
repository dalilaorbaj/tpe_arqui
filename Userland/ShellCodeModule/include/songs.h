#ifndef SONGS_H
#define SONGS_H
#include <library.h>


#define MS_PER_TICK 55
typedef struct {
    uint32_t frequency; // frecuencia en Hz
    int duration;       // duracion en milisegundos
} Note;

typedef struct {
    uint64_t note_count; // cantidad de notas
    Note* notes;         // puntero al arreglo de notas
    char* name;          // nombre de la canción
} Song;

extern Note mortal_kombat_notes[];

extern Note victory_sound_notes[];

extern Note cockroach_notes[];

extern Note got_theme_notes[];

void play_mortal_kombat_theme();

void play_victory_sound();

void play_song(const Song *song);

void play_cockroach_song();

void play_got_theme();

#endif