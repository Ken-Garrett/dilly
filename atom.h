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
#define atom_add(ATOMTBL, STR, LEN) \
  _atom_add(ATOMTBL, STR, LEN, __FILE__, __LINE__)
struct atom *_atom_add
(struct atomtable *t, const char *str, unsigned len, const char*, unsigned);

/* */
#define atom_find(ATOMTBL, STR, LEN) \
  _atom_find(ATOMTBL, STR, LEN, __FILE__, __LINE__)
struct atom *_atom_find
(struct atomtable *t, const char *str, unsigned len, const char *, unsigned);

/* */
#define atom_get(ATOM, PTR, PLEN) \
  _atom_get(ATOM, PTR, PLEN, __FILE__, __LINE__)
unsigned _atom_get(struct atom *, void *ptr,
  unsigned *plen, const char *, unsigned);

#endif
