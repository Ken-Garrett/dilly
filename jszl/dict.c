#include "dictionary.h"

struct atom *atom_table[256];

struct dictionary{
struct dictns *global;
struct atomtable  *table;
void *(__cdecl *alloc)(unsigned);
void (__cdecl *dealloc)(void*);
struct dictionary *next;
};

struct dictval{
struct dictval *next;
short type;
struct atom *atom;
};

struct dictkey{
struct dictkey *next;
struct dictval *first_val;
struct atom *atom;
};

struct dictns{
struct dictns *next;
struct dictns *first_ns;
struct dictkey *first_key;
struct atom *atom;
};


void ReportError(const char *msg, const char* fn, const char* file, unsigned line)
{
fprintf((__acrt_iob_func(2)), msg, fn, file, line);
ExitProcess(1);
}

static unsigned _dict_namespace_iter
(struct dictionary *dict, struct dictns *ns, dictns_cb cb, int level);

struct dictionary *CDECL dict_ctor()
{
struct dictionary *dict;
struct dictns    *ns;

    dict = MEM_ALLOC(sizeof(struct dictionary));
    assert(dict);
    ns = MEM_ALLOC(sizeof(struct dictns));
    assert(ns);
    ns->next = 0; 
    ns->first_ns = 0;
    ns->first_key = 0;
    dict->global = ns;
    dict->table = atomtable_init(atom_table, 256);
    assert(dict->table);
    return dict;
}

void dict_free(struct dictionary *dict)
{
    atomtable_free(dict->table);
}

void dict_namespace_delete
(struct dictionary *dict, struct dictns **ns)
{
    dict->dealloc(*ns);
    *ns = 0;
}

struct dictns *_dict_namespace_add
(struct dictionary *dict,
 struct dictns *ns,
 const char *str, unsigned len DEBUG_DECL)
{
    ns = ns ? ns : dict->global;
    if(ns->first_ns){
        ns = ns->first_ns;
        while(ns->next) ns = ns->next;
        ns->next = dict->alloc(sizeof(struct dictns));
        assert(ns->next);
        ns = ns->next;
    }
    else{
        ns->first_ns = dict->alloc(sizeof(struct dictns));
        assert(ns->first_ns);
        ns = ns->first_ns;
    }
    ns->next      = 0;
    ns->first_ns  = 0;
    ns->first_key = 0;
    ns->atom      = str ? _atom_add(dict->table, str, len DEBUG_PARAMS) : 0;
    if(str) assert(ns->atom);
    return ns; 
}


struct dictns *_dict_namespace_byname
(struct dictionary *dict, struct dictns *ns,
 const char *str, unsigned len DEBUG_DECL)
{
struct atom *atom;
    if(!dict); //ReportError("ERROR in %s: Must pass a valid dictionary : %s(%u)\n", __FUNCTION__+1, __file__, __line__);
    if(!dict->global);
        //ReportError("ERROR in %s: Missing the global namespace : %s(%u)\n", __FUNCTION__+1, __file__, __line__);

    ns = ns ? ns: dict->global->first_ns;
    if(!ns) return 0;
    if(!(atom = _atom_find(dict->table, str, len DEBUG_PARAMS))) return 0;
    while(ns && ns->atom != atom) ns = ns->next;
    return ns;
}


struct dictns *_dict_namespace_byindex
(struct dictionary *dict, struct dictns *ns,
 int idx, void *ptr, unsigned *plen DEBUG_DECL)
{
    if(!dict) return 0;
    ns = ns ? ns : dict->global->first_ns;
    while(idx--) if(!(ns = ns->next)) return 0;
    if(ns) _atom_get(ns->atom, ptr, plen DEBUG_PARAMS);
    return ns;
}

struct dictns *_dict_namespace_next
(struct dictns *ns, void *ptr, unsigned *plen DEBUG_DECL)
{
    assert(ns);
    ns = ns->next;
    if(ns) _atom_get(ns->atom, ptr, plen DEBUG_PARAMS);
    return ns;
}

struct dictkey *_dict_key_add
(struct dictionary *dict, struct dictns *ns,
 const char *str, unsigned len DEBUG_DECL)
{
struct dictkey *key = 0;
    assert(dict);
    ns = ns ? ns : dict->global;
    if(ns->first_key){
        key = ns->first_key;
        while(key->next) key = key->next;
        key->next = dict->alloc(sizeof(struct dictkey));
        assert(key->next);
        key = key->next;
    }
    else{
        key = dict->alloc(sizeof(struct dictkey));
        assert(key);
        ns->first_key = key;
    }
    key->next      = 0;
    key->first_val = 0;
    key->atom      = str ? _atom_add(dict->table, str, len DEBUG_PARAMS) : 0;
    if(str)  assert(key->atom);
    return key;
}

struct dictkey *_dict_key_byname
(struct dictionary *dict, struct dictns *ns,
 const char *str, unsigned len DEBUG_DECL)
{
struct dictkey *key;
struct atom *atom;

    if(!dict); //ReportError("ERROR in %s: Must pass a valid dictionary : %s(%u)\n", __FUNCTION__+1, __file__, __line__);
    ns = ns ? ns : dict->global;
    assert(ns);
    if(!(atom = _atom_find(dict->table, str, len DEBUG_PARAMS))) return 0;
    key = ns->first_key;
    while(key && key->atom != atom) key = key->next;
    return key;
}

struct dictkey *_dict_key_byindex
(struct dictionary *dict, struct dictns* ns,
 int idx, void *ptr, unsigned *plen DEBUG_DECL)
{
struct dictkey *key;

    if(!dict) return 0;
    ns = ns ? ns : dict->global;
    if(!(key = ns->first_key)) return 0;
    while(idx--) if(!(key = key->next)) return 0;
    _atom_get(key->atom, ptr, plen DEBUG_PARAMS);
    return key;
}

struct dictkey *_dict_key_next
(struct dictkey *key, void *ptr,
 unsigned *plen DEBUG_DECL)
{
    if(!key) return 0;
    key = key->next;
    if(key) _atom_get(key->atom, ptr, plen DEBUG_PARAMS);
    return key;
}


struct dictval *_dict_value_add
(struct dictionary *dict, struct dictkey *key,
 const char *str, unsigned len, unsigned type DEBUG_DECL)
{
struct dictval *val;
    if(key->first_val){
        val = key->first_val;
        while(val->next) val = val->next;
        val->next = dict->alloc(sizeof(struct dictval));
        assert(val->next);
        val = val->next;
    }
    else{
        val = dict->alloc(sizeof(struct dictval));
        assert(val);
        key->first_val = val;
    }
    val->next = 0;
    val->type = type;
    val->atom = _atom_add(dict->table, str, len DEBUG_PARAMS);
    assert(val->atom);
    return val;
}

struct dictval *_dict_value_byindex
(struct dictkey *key, int idx, void *ptr,
 unsigned *plen, unsigned *ptype DEBUG_DECL)
{
struct dictval *val;
    assert(key);
    if(!(val = key->first_val)) return 0;
    while(idx--) if(!(val = val->next)) return 0;
    
    _atom_get(val->atom, ptr, plen DEBUG_PARAMS);
    if(ptype) *ptype = val->type;
    return val;
}

struct dictval *_dict_value_next
(struct dictval *val, void *ptr, unsigned *plen,
 unsigned *ptype DEBUG_DECL)
{
    if(!val) return 0;
    val = val->next;
    if(val) _atom_get(val->atom, ptr, plen DEBUG_PARAMS);
    if(ptype) *ptype = val->type;
    return val;
}


unsigned dict_namespace_iter
(struct dictionary *dict,
 struct dictns *ns,
 dictns_cb cb
)
{
    _dict_namespace_iter(dict, ns, cb, 0);
    return 1;
}


static unsigned _dict_namespace_iter
(struct dictionary *dict,
 struct dictns *ns,
 dictns_cb cb,
 int level)
{
int len = 0;
char *str;
    ns = ns ? ns : dict->global;
    for(ns = ns->first_ns; ns; ns = ns->next)
    {
        _atom_get(ns->atom, &str, &len DEBUG_PARAMS);
        cb(dict, ns, str, len, level);
        _dict_namespace_iter(dict, ns, cb, level+1);
    }
    return 0;
}

