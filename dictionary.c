/* Author: Ken Garrett */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hash.h"
#include "atom.h"
#include "dictionary.h"

#define STRING(s) ((s).multi ? (s).unicode : (s).ascii) 

#define ATOM_TABLE_SIZE 256 
struct atom *atom_table[ATOM_TABLE_SIZE];

//Dictionary
struct dictionary{
struct dictns *global;
struct atomtable  *table;
void *(__cdecl *alloc)(unsigned);
void (__cdecl *dealloc)(void*);
struct dictionary *next;
};
#define DICT_SZ sizeof(struct dictionary)

struct dictval{
struct dictval *next;
short type;
struct atom *atom;
};
#define DICTVAL_SZ sizeof(struct dictval)

struct dictkey{
struct dictkey *next;
struct dictval *first_val;
struct atom *atom;
};
#define DICTKEY_SZ sizeof(struct dictkey)

struct dictns{
struct dictns *next;
struct dictns *first_ns;
struct dictkey *first_key;
struct atom *atom;
};
#define DICTNS_SZ sizeof(struct dictns)

static unsigned _dict_namespace_iter
(struct dictionary *dict, struct dictns *ns, dictns_cb cb, int level);

//dict_add_string;
//dict_add_uint8;
//dict_add_uint16;
//dict_add_uint32;
//dict_add_sint8

/**
** @name dict_ctor
** @desc Initialize a dictionary object
** @param
** @param
*/
struct dictionary * __cdecl dict_ctor
(void *(__cdecl *alloc)(unsigned),
 void (__cdecl *dealloc)(void*))
{
struct dictionary *dict;
struct dictns    *ns;

    dict = alloc(DICT_SZ);
    assert(dict);
    ns = alloc(DICTNS_SZ);
    assert(ns);
    ns->next = 0; /* the top level namespace can never have a sibling */
    ns->first_ns = 0;
    ns->first_key = 0;
    dict->global = ns;
    dict->alloc = alloc;
    dict->dealloc = dealloc;
    dict->table = atomtable_init(atom_table, ATOM_TABLE_SIZE, hash_djb2, malloc);
    assert(dict->table);
    return dict;
}

void dict_free(struct dictionary *dict)
{
    atomtable_free(dict->table);
}


/**
** @name dict_namespace_add
** @desc Add a namespace to a dictionary, If NULL is passed as the namespace
         then the global namespace will be used
** @param dict
** @param ns
** @param str
** @param len
*/
struct dictns *dict_namespace_add
(struct dictionary *dict,
 struct dictns *ns,
 const char *str, unsigned len)
{
    ns = ns ? ns : dict->global;
    if(ns->first_ns){
        ns = ns->first_ns;
        while(ns->next) ns = ns->next;
        ns->next = dict->alloc(DICTNS_SZ);
        assert(ns->next);
        ns = ns->next;
    }
    else{
        ns->first_ns = dict->alloc(DICTNS_SZ);
        assert(ns->first_ns);
        ns = ns->first_ns;
    }
    ns->next      = 0;
    ns->first_ns  = 0;
    ns->first_key = 0;
    ns->atom      = atom_add(dict->table, str, len);
    assert(ns->atom);
    return ns; 
}

void dict_namespace_delete
(struct dictionary *dict, struct dictns **ns)
{
    dict->dealloc(*ns);
    *ns = 0;
}

/**
** @name dict_key_add
** @desc Adds a key to a dictionary. If NULL is passed as the namespace
**       then the global namespace will be used
** @param dict Dictionary object
** @param ns
** @param str
** @param len
**
*/
struct dictkey *dict_key_add
(struct dictionary *dict,
 struct dictns *ns,
 const char *str, unsigned len)
{
struct dictkey *key = 0;
    assert(dict);
    ns = ns ? ns : dict->global;
    if(ns->first_key){
        key = ns->first_key;
        while(key->next) key = key->next;
        key->next = dict->alloc(DICTKEY_SZ);
        assert(key->next);
        key = key->next;
    }
    else{
        key = dict->alloc(DICTKEY_SZ);
        assert(key);
        ns->first_key = key;
    }
    key->next = 0;
    key->first_val = 0;
    key->atom = atom_add(dict->table, str, len);
    assert(key->atom);
    return key;
}


/**
** @name dict_value_add
** @desc Adds a value to a key in a dictionary
** @param
** @param
** @param
** @return
*/
struct dictval *dict_value_add
(struct dictionary *dict, struct dictkey *key, const char *str, unsigned len)
{
struct dictval *val;
    if(key->first_val){
        val = key->first_val;
        while(val->next) val = val->next;
        val->next = dict->alloc(DICTVAL_SZ);
        assert(val->next);
        val = val->next;
    }
    else{
        val = dict->alloc(DICTVAL_SZ);
        assert(val);
        key->first_val = val;
    }
    val->next = 0;
    val->atom = atom_add(dict->table, str, len);
    return val;
}


/**
** @name dict_namespace_byname
** @desc Lookup a namespace by name
** @return a pointer to a dictionary namespace
**
*/
struct dictns *dict_namespace_byname
(struct dictionary *dict,
 struct dictns *ns,
 const char *str, unsigned len)
{
struct atom *atom;
    assert(dict && dict->global && dict->global->first_ns);
    ns = ns ? ns: dict->global->first_ns;
    if(!(atom = atom_find(dict->table, str, len))) return 0;
    while(ns && ns->atom != atom) ns = ns->next;
    return ns;
}


/**
** @name dict_key_byname
** @desc Lookup a key by name
** @param dict
** @param
** @param
*/
struct dictkey *dict_key_byname
(struct dictionary *dict,
 struct dictns *ns,
 const char *str, unsigned len)
{
struct dictkey *key;
struct atom *atom;

    ns = ns ? ns : dict->global;
    if(!(atom = atom_find(dict->table, str, len))) return 0;
    key = ns->first_key;
    while(key && key->atom != atom) key = key->next;
    return key;
}


/**
** @name dict_namespace_byindex
** @desc Lookup a namespace by index
** @param
** @param
*/
struct dictns *dict_namespace_byindex
(struct dictionary *dict, struct dictns *ns,
 int idx, char **pstr, unsigned *plen)
{
    if(!dict) return 0;
    ns = ns ? ns : dict->global->first_ns;
    while(idx--) if(!(ns = ns->next)) return 0;
    if(ns && pstr) *pstr = atom_get(ns->atom, plen);
    return ns;
}


/**
** @name dict_key_byindex
** @desc Lookup a key by index
** @return
*/
struct dictkey *dict_key_byindex
(struct dictionary *dict, struct dictns* ns,
 int idx, char **pstr, unsigned *plen)
{
struct dictkey *key;

    if(!dict) return 0;
    ns = ns ? ns : dict->global;
    key = ns->first_key;
    while(idx--) if(!(key = key->next)) return 0;
    if(key && pstr) *pstr = atom_get(key->atom, plen);
    return key;
}


/**
** @desc Get the next namespace in the list
*/
struct dictns *dict_namespace_next
(struct dictns *ns, char **pstr, unsigned *plen)
{
    assert(ns);
    //if(!ns) return 0;
    ns = ns->next;
    if(ns && pstr) *pstr = atom_get(ns->atom, plen);
    return ns;
}


/**
** @desc Get the next key in the list
*/
struct dictkey *dict_key_next
(struct dictkey *key, char **pstr, unsigned *plen)
{
    if(!key) return 0;
    key = key->next;
    if(key && pstr) *pstr = atom_get(key->atom, plen);
    return key;
}


/**
** @desc Lookup a value in an array by index
*/
struct dictval *dict_value_byindex
(struct dictkey *key, int idx, char **pstr, unsigned *plen)
{
struct dictval *val;
    if(!key) return 0;
    val = key->first_val;
    while(idx--) if(!(val = val->next)) return 0;
    if(val && pstr) *pstr = atom_get(val->atom, plen);
    return val;
}


/**
** @desc Get the next value in the array list
*/
struct dictval *dict_value_next
(struct dictval *val, char **pstr, unsigned *plen)
{
    if(!val) return 0;
    val = val->next;
    if(val && pstr) *pstr = atom_get(val->atom, plen);
    return val;
}







/**** UNDER CONSTRUCTION ****/


unsigned dict_namespace_iter
(struct dictionary *dict,
 struct dictns *ns,
 dictns_cb cb
)
{
    _dict_namespace_iter(dict, ns, cb, 0);
    return 1;
}


/* print dictionary contents, used for debugging */
void print_dict(struct dictionary *dict)
{
struct dictns *ns, *ns2, *ns3;
struct dictkey *key;
struct dictval *val;
struct atom *atom;
char keybuf[128], valbuf[128];
const char *str;
int len;

ns  = dict->global;
//ns  = ns->first_ns; /* get the first object of the top level object */
while(ns){
    key = ns->first_key;
    atom = ns->atom;
    //GetAtomName(ns->hash, keybuf, 256);
    str = atom_get(atom, &len);
    printf("%.*s:\n", len, str);
    while(key){
        //GetAtomNameA(key->hash, keybuf, 256);
	atom = key->atom;
        str = atom_get(atom, &len);
        printf("  %.*s:\n", len, str); 
	val = key->first_val;
	while(val){
            //GetAtomNameA(val->hash_key, valbuf, 128);
	    atom = val->atom;
            str = atom_get(atom, &len);
            printf("    %.*s\n", len, str);
            val = val->next;
            }
	key = key->next;
        }
    ns2 = ns->first_ns;
    while(ns2){
        atom = ns2->atom;
	//GetAtomName(ns2->hash, keybuf, 256);
        str = atom_get(atom, &len);
	printf("  %.*s:\n", len,  str);
	ns3 = ns2->first_ns;
	while(ns3){
            atom = ns3->atom;
            //GetAtomName(ns3->hash, keybuf, 256);
            str = atom_get(atom, &len);
	    printf("    %.*s:\n", len, str);
	    ns3 = ns3->next;
	    }
        ns2 = ns2->next;
        }
    ns = ns->next;
    printf("\n");
    }
}

/* PRIVATE FUNCTIONS */

static unsigned _dict_namespace_iter
(struct dictionary *dict,
 struct dictns *ns,
 dictns_cb cb,
 int level)
{
int len;
char *str;
    ns = ns ? ns : dict->global;
    for(ns = ns->first_ns; ns; ns = ns->next)
    {
        str = atom_get(ns->atom, &len);
        cb(dict, ns, str, len, level);
        _dict_namespace_iter(dict, ns, cb, level+1);
    }
    return 0;
}

