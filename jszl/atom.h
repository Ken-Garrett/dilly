#ifndef ATOM_H_INCLUDED
#define ATOM_H_INCLUDED
// Atom
struct atomtable;
struct atom;


/* */
struct atomtable *atomtable_init(void *mem, unsigned size);

/* */
void atomtable_free(struct atomtable *table);

/* */
#define atom_add(ATOMTBL, DATA, LEN, HASH) \
  _atom_add(ATOMTBL, DATA, LEN, HASH DEBUG_PARAMS)
struct atom *_atom_add
(struct atomtable *t, void *data, unsigned len, unsigned long hash DEBUG_DECL);

/* */
#define atom_find(ATOMTBL, HASH, LEN) \
  _atom_find(ATOMTBL, HASH, LEN DEBUG_PARAMS)
struct atom *_atom_find
(struct atomtable *t, unsigned long hash, unsigned len DEBUG_DECL);

/* */
#define atom_get(ATOM, PTR, PLEN) \
  _atom_get(ATOM, PTR, PLEN DEBUG_PARAMS)
unsigned _atom_get(struct atom *, void *ptr,
  unsigned *plen DEBUG_DECL);

#endif
