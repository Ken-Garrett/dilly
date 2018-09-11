#ifndef ATOM_H
#define ATOM_H
// Atom
struct atomtable;
struct atom;

/* */
struct atomtable *atomtable_init(void *mem, unsigned size, void*, void*);

/* */
void atomtable_free(struct atomtable *table);

/* */
struct atom *atom_add
(struct atomtable *t, const char *str, unsigned len);

/* */
struct atom *atom_find
(struct atomtable *t, const char *str, unsigned len);

/* */
char *atom_get(struct atom *, unsigned *plen);

#endif
