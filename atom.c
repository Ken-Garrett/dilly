#include <stdlib.h>
#include <assert.h>
#include "atom.h"

#define MAX_NUM_ATOMS 4096
#define ATOM_DATA_SIZE 65536

struct atomtable {
struct atom **table;
int size;
struct atom *next_free;
struct atomtable *next;
};
#define ATOMTABLE_SZ sizeof(struct atomtable)

struct atom {
struct atom *next;
const char *str;
unsigned len;
unsigned hash;
};
#define ATOM_SZ sizeof(struct atom)

struct atom atoms[MAX_NUM_ATOMS];
char immutable_memory[ATOM_DATA_SIZE];

/* Created by Dan J. Bernstein */
unsigned djb2(const char *str, unsigned len)
{
unsigned long h = 5381;
int c;

if(!len) //zero-terminated str
    while(c = *str++)
        h = ((h << 5) + h) + c;
else //fixed length
    for(int i = 0; i < len; i++)
        h = ((h << 5) + h) + str[i];
return h;
}

struct atomtable *atomtable_init(void *mem, unsigned size)
{
struct atomtable *at;
    at = malloc(ATOMTABLE_SZ);
    at->table = mem;
    at->size = size;
    at->next_free = mem;
    at->next = 0;
    //at->dealloc = (deallocfn)dealloc;
    return at;
}

void atomtable_free(struct atomtable *at)
{
    free(at);
}

void *alloc_atom(unsigned len)
{
static char memory[65536];
static char *next_slot = memory;
void *ptr;

ptr = next_slot;
next_slot += len;
return ptr;
}

/**
** @name atom_addcpy
** @param
** @param
*/
struct atom *atom_addcpy(struct atomtable *at, const char *str, unsigned len)
{
struct atom *atom, *patom;
long index;

    len = len ? len : strlen(str);
    if(!(atom = malloc(ATOM_SZ))) return 0; //create space for the atom
    atom->str = malloc(len); //create space for the atom data
    assert(atom->str);
    memcpy((void*)atom->str, str, len);
    atom->hash = dbj2(str, len);
    index = atom->hash % at->size;
    atom->len = len;
    atom->next = 0;
    if(!at->table[index]) at->table[index] = atom;
    else{
        patom = at->table[index];
        while(patom->next) patom = patom->next;
        patom->next = atom;
    }
    return atom;
}

/**
** @desc Add an string to the atom table
** This function is currently a bottleneck because of malloc
*/
struct atom *atom_add(struct atomtable *at, const char *str, unsigned len)
{
struct atom *atom, *patom;
long index = 0;

    len = len ? len : strlen(str);
    if(!(atom = malloc(ATOM_SZ))) return 0; //create space for the atom
    atom->str = str;
    atom->hash = djb2(str, len);
    index = atom->hash % at->size;
    atom->len = len;
    atom->next = 0;
    if(!at->table[index]) at->table[index] = atom;
    else{
        patom = at->table[index];
        while(patom->next) patom = patom->next;
        patom->next = atom;
    }
    return atom;
}

char *atom_get(struct atom *atom, unsigned *plen)
{
    assert(atom);
    if(plen) *plen = atom->len;
    return atom->str;
}

/**
** @desc
*/
struct atom *atom_find(struct atomtable *at, const char *str, unsigned len)
{
struct atom *atom;
int hash;

    len = len ? len : strlen(str);
    hash = dbj2(str, len);
    for(atom = at->table[hash%at->size]; atom; atom = atom->next)
    {
        if((atom->len == len) && (atom->hash == hash)) break;
    }
    return atom;
}
