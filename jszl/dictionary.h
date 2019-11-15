#ifndef DICTIONARY_H_INCLUDE
#define DICTIONARY_H_INCLUDE

typedef void (*dictns_cb)(struct dictionary*, struct dictns*, char*, int, int);

// Dictionary
struct dictionary;
//typedef void * dict_t;
struct dictval;
//typedef void * dictval_t;
struct dictkey;
//typedef void * dictkey_t;
struct dictns;
//typedef void * dictns_t;

/* */
struct dictionary * CDECL dict_ctor();

/* */
void dict_free(struct dictionary *);

/* */
#define dict_namespace_add(DICT, NS, STR, LEN)\
        _dict_namespace_add(DICT, NS, STR, LEN DEBUG_PARAMS)
struct dictns *_dict_namespace_add
(struct dictionary*, struct dictns*,
 const char *str, unsigned len DEBUG_DECL);  

/* */
#define dict_namespace_byname(DICT, NS, STR, LEN)\
        _dict_namespace_byname(DICT, NS, STR, LEN DEBUG_PARAMS)
struct dictns *_dict_namespace_byname
(struct dictionary*, struct dictns*,
 const char *str, unsigned len DEBUG_DECL); 

/* */
#define dict_namespace_byindex(DICT, NS, IDX, PTR, PLEN)\
        _dict_namespace_byindex(DICT, NS, IDX, PTR, PLEN DEBUG_PARAMS)
struct dictns *_dict_namespace_byindex
(struct dictionary*, struct dictns*,
 int index, void *ptr, unsigned *plen DEBUG_DECL); 

/* */
#define dict_namespace_next(NS, PTR, PLEN)\
        _dict_namespace_next(NS, PTR, PLEN DEBUG_PARAMS)
struct dictns *_dict_namespace_next
(struct dictns*, void *ptr, unsigned *plen DEBUG_DECL); 

/* */
#define dict_key_add(DICT, NS, STR, LEN)\
        _dict_key_add(DICT, NS, STR, LEN DEBUG_PARAMS)
struct dictkey *_dict_key_add
(struct dictionary*, struct dictns*,
 const char *str, unsigned len DEBUG_DECL); 

/* */
#define dict_key_byname(DICT, NS, STR, LEN)\
        _dict_key_byname(DICT, NS, STR, LEN DEBUG_PARAMS)
struct dictkey *_dict_key_byname
(struct dictionary*, struct dictns*,
 const char *str, unsigned len DEBUG_DECL);

/* */
#define dict_key_byindex(DICT, NS, IDX, PTR, PLEN)\
        _dict_key_byindex(DICT, NS, IDX, PTR, PLEN DEBUG_PARAMS)
struct dictkey *_dict_key_byindex
(struct dictionary*, struct dictns*,
 int index, void *ptr, unsigned *plen DEBUG_DECL);

/* */
#define dict_key_next(KEY, PTR, PLEN)\
        _dict_key_next(KEY, PTR, PLEN DEBUG_PARAMS)
struct dictkey *_dict_key_next
(struct dictkey*, void *ptr, unsigned *plen DEBUG_DECL); 

/* */
#define dict_value_add(DICT, KEY, STR, LEN, TYPE)\
        _dict_value_add(DICT, KEY, STR, LEN, TYPE DEBUG_PARMS)
struct dictval *_dict_value_add
(struct dictionary*, struct dictkey*,
 const char *str, unsigned len, unsigned type DEBUG_DECL);

/* */
#define dict_value_byindex(KEY, IDX, PTR, PLEN, PTYPE)\
        _dict_value_byindex(KEY, IDX, PTR, PLEN, PTYPE DEBUG_PARAMS)
struct dictval *_dict_value_byindex
(struct dictkey*, int index, void *ptr,
 unsigned *plen, unsigned *ptype DEBUG_DECL); 

/* */
#define dict_value_next(VAL, PTR, PLEN, PTYPE)\
        _dict_value_next(VAL, PTR, PLEN, PTYPE DEBUG_PARAMS)
struct dictval *_dict_value_next
(struct dictval*, void *ptr,
 unsigned *plen, unsigned *ptype DEBUG_DECL); 

/* */
void print_dict(struct dictionary *dict);

/* */
unsigned dict_namespace_iter
(struct dictionary*, struct dictns*, dictns_cb); 


//dict_uint32_add
//dict_uint16_add
//dict_ipv4_add
//dict_ipv6_add

#define DICTERR_NO_DICT "ERROR in %s: Must pass a valid dictionary : %s(%u)\n"
#define DICTERR_NO_GLOBALNS "ERROR in %s: Missing the global namespace : %s(%u)\n"
#define DICTERR_NO_FIRSTNS "ERROR in %s: Missing first ns : %s(%u)\n"

/*
void *ns, *key, *val;
char *nsstr, *keystr, *valstr;
int nslen, keylen, vallen;

for(ns = dict_namespace_byindex(dict, 0, 0, &nsstr, &nslen);
    ns; ns = dict_namespace_next(ns, &nsstr, &nslen))
    {
    for(key = dict_key_byindex(dict, ns, 0, &keystr, &keylen); 
    key; key = dict_key_next(key, &keystr, &keylen))
        {
	for(val = dict_value_byindex(key, 0, &valstr, &vallen);
        val; val = dict_value_next(val, &valstr, &vallen))
            {
            }
        }
    }
}
*/
 

#endif
