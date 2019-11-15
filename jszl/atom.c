/**
** @desc
*/
static struct atom *atom_find(
  struct atom * atom_table[]
 ,unsigned atom_table_size
 ,unsigned hash
 ,unsigned len
){
struct atom *atom = 0;
    for(atom = atom_table[hash % atom_table_size]; atom; atom = atom->next)
        if((atom->length == len) && (atom->hash == hash)) break;
    return atom;
}

/**
** @desc Add an string to the atom table
** This function is currently a bottleneck because of malloc
*/
static inline struct atom *atom_add(
  struct atom * atom_table[]
 ,unsigned atom_table_size
 ,struct atom * atom_to_add
 ,unsigned hash
 ,unsigned len
 ,const void *data
 ,void *pb
){
struct atom *patom, *atom = 0;
unsigned index = 0;
    atom = atom_find(atom_table, atom_table_size, hash, len);
    if(atom) return atom;
    atom = atom_to_add;
    assert(atom);
    atom->next = 0;
    atom->length = len;
    atom->data = data;
    atom->hash = hash;
    index = hash % atom_table_size;
    if(!atom_table[index]){
        atom_table[index] = atom;
    }
    else{
        patom = atom_table[index];
        while(patom->next) patom = patom->next;
        patom->next = atom;
    }
    return atom;
}
