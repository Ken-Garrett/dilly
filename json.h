#ifndef JSON_H
#define JSON_H

typedef void* JsonT; 

/* */
#define json_ctor(OBJ) \
        _json_ctor(OBJ, __FILE__, __LINE__)
JsonT _json_ctor(const char *, const char *, unsigned);

/* */
#define json_load(OBJ) \
        _json_load(OBJ, __FILE__, __LINE__)
unsigned _json_load(JsonT, const char *, unsigned);

enum json_errors{
JSON_ERROR_SYNTAX = 1,
JSON_ERROR_VALUE,
JSON_ERROR_TYPE,
};


#endif
