
unsigned atouint(const char *str, unsigned len)
{
unsigned c;
unsigned n = 0;
    for(c = *str; len--; c = *++str)
        n = ((n<<3) + (n<<1)) + (c - '0'); //n*10 + c-32
    return n;
}

unsigned hextoint(const char *str, unsigned len)
{
unsigned c = *str;
unsigned n = 0;
    for(c = *str; len--; c = *++str)
        n = (n<<4) + (c > '9' ? CHAR_TOUPPER(c) - 7 : c) - '0'; 
    return n;
}


/**
 * @param
 * @return
**/
static unsigned validate_phonenumber(const char *str)
{
return 0;
}

/**
 * @param
 * @return
**/
static unsigned validate_string(const char *str)
{
unsigned i = 0;
    while(str[i] != '"'){
        if(!str[i]) return 0; 
        i++;
    }
    return ++i;
}

/**
 * @param
 * @return
**/
static unsigned validate_uuid(const char *start)
{
const char *str = start;
short i;
    if(*str == '{') str++;
    for(i = 8; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 4; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 4; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 4; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str != '-') return 0;
    str++;
    for(i = 12; i; i--){
        if(!IS_HEX_DIGIT(*str)) return 0;
        str++;
    }
    if(*str == '}') str++;
return str - start;
}

/**
 * @param
 * @param ipv6 returns the ipv6 address in binary network byte order
 * @return
 * Leading zeros in an octet wil cause an error
 * TODO Detect invalid on trailing digits, right now it confirms as valid addr
 * TODO detect leading zeroes in a hextet
 * TODO refactor to remove duplication
 * TODO handle multiple long running zero hextets.
 * TODO optimize memset to set 1 less zero since first zero is already set?
**/

#define IS_IPV6_DIGIT(c) (IS_DEC_DIGIT(c) || _BETWEEN_('a', c, 'f'))
static unsigned validate_ipv6(const char *start, unsigned *err, union ipv6 *ipv6)
{
#define IPV6_TRUNCATED 0x01
#define IPV6_BRACKETED 0x02
const char *str = start;
char i, j;
short z, y = 0;
unsigned flags = 0;
    //if(*str == '[') { flags &= IPV6_BRACKETED; str++; }
    for(i = 0; i < 7; i++)
    {
        for(j = 0; j < 4; j++){
            if(!IS_IPV6_DIGIT(*str)) break;
            //if(j == 4 && *str == '0' && IS_IPV6_DIGIT(str[1])) return 0;
            str++;
        }
        z = hextoint(str-j, j);
        if(__LITTLE_ENDIAN__) z = ENDIAN16(z);
        *(((short*)ipv6)+i) = z;
        if(*str != ':'){ //should be colon
            if(flags & IPV6_TRUNCATED){
                memmove( (((short*)ipv6)+(8-(i-y))),
                         (((short*)ipv6)+(y+1)),
                         sizeof(short)*(i-y) );  
                memset( (((short*)ipv6)+y), 0, sizeof(short)*(8-i) );
                return str - start;
            }
            else return 0;
        }
        else if(*(str-1) == ':'){
            if(flags & IPV6_TRUNCATED) return 0;
            else{
                flags |= IPV6_TRUNCATED;
                y = i; //mark truncated hextet
            }
        }
        str++;
    }
    for(j = 0; j < 4; j++)
    {
        if(!IS_IPV6_DIGIT(*str)) break;
        str++;
    }
    z = hextoint(str-j, j);
    if(__LITTLE_ENDIAN__) z = ENDIAN16(z);
    *(((short*)ipv6)+i) = z;
    return str - start;
}

/**
 * @param
 * @param addr returns the 32 bit ip address in big-endian format
 * @return
 * TODO refactor to remove duplication
 * TODO cleanup port handling. do we even want it?
 * TODO detect leading zeroes in an octet
**/
static unsigned validate_ipv4(const char *start, unsigned *err, unsigned *addr)
{
const char *str = start;
short i, j;
unsigned x = 0;

    //printf("Validating IPv4 Address\n");
    for(i = 3; i; i--){
        for(j = 0; j < 3; j++){
            if(!IS_DEC_DIGIT(*str)){
                if(j == 0){
                    if(err) *err = 1;
                    return 0; //need at least 1 digit
                }
                break;
            }
            str++;
        }
        if(*str != '.') return 0; //should be decimal
            j = atouint(str-j, j);
        if(!_BETWEEN_(0, j, 255)) return 0;
            x |= j << (i<<3);
        str++;
    }
    for(j = 0; j < 3; j++){
        if(!IS_DEC_DIGIT(*str)){
            if(j == 0) return 0;
            break;
        }
        str++;
    }
    j = atouint(str-j, j);
    if(!_BETWEEN_(0, j, 255)) return 0;
    x |= j << (i<<3);
    *addr = x;

    //port handling
    if(*str == ':'){
        str++;
        for(j = 5; j; j--){
            if(!IS_DEC_DIGIT(*str)){
                if(j == 5) return 0;
                break;
            }
        str++;
        }
    }
    return str - start;
}

/**
 * @param
 * @return
 * TODO fix incomplete exponent validation
**/
static unsigned validate_number(const char *start)
{
const char *str = start;
char flags = 0;

    //printf("Validating Number\n");
    if(*str == '-'){
        flags |= 0x01;
        str++;
    }
    if(str[0] == '0'){
        if(str[1] == 'x' || str[1] == 'X'){ //hex
            str+=2;
            while(IS_DEC_DIGIT(*str) ||
            _BETWEEN_('a', *str, 'f') ||
            _BETWEEN_('A', *str, 'F'))
                str++;
        }
        else if(BETWEEN_('0', str[1], '7')){ //octal
            str+=2;
            while(_BETWEEN_('0', *str, '7')) str++;
        }
        else return 0;
    }
    else if(BETWEEN_('0', *str, '9')){ //decimal
        while(IS_DEC_DIGIT(*str)) str++;
        if(*str == '.'){ //floating point
            str++;
            while(IS_DEC_DIGIT(*str)) str++;
            if(*str == 'e' || *str == 'E'){
                str++;
                if(*str == '+'){
                    str++;
                }
                else if(*str == '-'){
                    str++;	
                }
                if(!IS_DEC_DIGIT(*str)) return 0;
                while(IS_DEC_DIGIT(*str)) str++;
            }
        }
        else if(*str == 'e' || *str == 'E'){ //exponent
            str++;
            if(*str == '+'){
                str++;
            }
            if(*str == '-'){
                str++; 
            }
            if(!IS_DEC_DIGIT(*str)) return 0;
            while(IS_DEC_DIGIT(*str)) str++;
        }
    }
    else return 0;
    return str - start;
}

#define DATE_MMDDYYYY

/**
 *  @param
 *  @return
**/
static unsigned validate_date(const char *start)
{
return 0;
}
