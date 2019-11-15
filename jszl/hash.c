
/* Created by Dan J. Bernstein */
unsigned __cdecl djb2(const char *str, unsigned len)
{
unsigned long h = 5381;
int c;

    if(!len) //zero-terminated str
        while(c = *str++)
            h = ((h << 5) + h) + CHAR_TOLOWER(c);
    else //fixed length
        for(int i = 0, c = str[i]; i < len; i++)
            h = ((h << 5) + h) + CHAR_TOLOWER(str[i]);
    return h;
}
