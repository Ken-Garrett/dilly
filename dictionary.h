#ifndef DICTIONARY_H
#define DICTIONARY_H

#define MACRO_StrPtr(s) ((s).wide ? (s).unicode : (s).ascii) 

typedef void (*dictns_cb)(struct dictionary*, struct dictns*, char*, int, int);

// Dictionary
struct dictionary;
struct dictval;
struct dictkey;
struct dictns;

/* */
struct dictionary * __cdecl dict_ctor
(void *(__cdecl *alloc)(unsigned),
 void (__cdecl *dealloc)(void*));

/* */
void dict_free(struct dictionary *);

/* */
struct dictns *dict_namespace_add
(struct dictionary*, struct dictns*, const char *str, unsigned len);  

/* */
struct dictkey *dict_key_add
(struct dictionary*, struct dictns*, const char *str, unsigned len); 

/* */
struct dictval *dict_value_add
(struct dictionary*, struct dictkey*, const char *str, unsigned len);

/* */
struct dictns *dict_namespace_byname
(struct dictionary*, struct dictns*, const char *str, unsigned len); 

/* */
struct dictkey *dict_key_byname
(struct dictionary*, struct dictns*, const char *str, unsigned len);

/* */
struct dictns *dict_namespace_byindex
(struct dictionary*, struct dictns*, int index, char **pstr, unsigned *plen); 

/* */
struct dictkey *dict_key_byindex
(struct dictionary*, struct dictns*, int index, char **pstr, unsigned *plen);

/* */
struct dictval *dict_value_byindex
(struct dictkey*, int index, char **pstr, unsigned *plen); 

/* */
struct dictns *dict_namespace_next
(struct dictns*, char **pstr, unsigned *plen); 

/* */
struct dictkey *dict_key_next
(struct dictkey*, char **pstr, unsigned *plen); 

/* */
struct dictval *dict_value_next
(struct dictval*, char **pstr, unsigned *plen); 

/* */
void print_dict(struct dictionary *dict);

/* */
unsigned dict_namespace_iter
(struct dictionary*, struct dictns*, dictns_cb); 


//dict_uint32_add
//dict_uint16_add
//dict_ipv4_add
//dict_ipv6_add

enum value_types{
DICT_TYPE_STRING  = 0,
DICT_TYPE_BOOLEAN,
DICT_TYPE_UBYTE,
DICT_TYPE_SWORD,
DICT_TYPE_SDWORD,
DICT_TYPE_QUADWORD,
};

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
