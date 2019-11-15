#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\core.h"
#include "dictionary.h"

void cb(struct dictionary *dict, struct dictns* ns,
char *str, int len, int level)
{
for(int i = level; i--;) printf("\t");
printf("%.*s\n", len, str);
}

struct dictionary *create_dict()
{



void main(int argc, char **argv)
{
struct dictionary *dict;

    dict = create_dict();
    //dict_namespace_iter(dict, 0, cb);
}
