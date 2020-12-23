#pragma option OST
#pragma option ON
#pragma option cri-
#pragma option -CPA
#initallvar 0
#jumptomain FALSE
#code32 TRUE

char   os_name[8]   = {'M','E','N','U','E','T','0','1'};
dword  os_version   = 0x00000001;
dword  start_addr   = #______INIT______;
dword  final_addr   = #______STOP______+32;
dword  alloc_mem    = 0x1000000;
dword  x86esp_reg   = 0x1000000;
dword  I_Param      = 0;
dword  I_Path       = 0;
char param[4096]={0};
char program_path[4096]={0};
dword allocateBuffer = 0;

inline byte IntegerToBoolean(signed int value)
{
  if (value != 0) return 0x01;
  return 0;
}

inline byte IntegerCompareEq(signed int value1, signed int value2)
{
  if (value1 == value2) return 0x01;
  return 0;
}

inline byte IntegerCompareLt(signed int x, signed int y)
{
  if (x < y) return 1;
  return 0;
}

inline byte IntegerCompareRt(signed int x, signed int y)
{
  if (x > y) return 1;
  return 0;
}

inline float brackets_float(float x)
{
  return x;
}

inline dword brackets_string(dword x)
{
  return x;
}

inline int brackets_integer(int x)
{
  return x;
}

inline int prototype_len_string(dword text)
{
  dword begin = 0;
  begin = text;
  while(DSBYTE[text]) text++;
  return text-begin;
}

:unsigned char BUF_ITOA[11];
inline dword itoa(signed long number)
{
        dword ret,p;
        byte cmd;
        long mask,tmp;
        mask = 1000000000;
        cmd = 1;
        p = #BUF_ITOA;
        if(!number){
                ESBYTE[p] = '0';
                ESBYTE[p+1] = 0;
                return p;
        }
        ret = p;
        if(number<0)
        {
                $neg number
                ESBYTE[p] = '-';
                $inc p
        }
        while(mask)
        {
                tmp = number / mask;
                tmp = tmp%10;

                if(cmd){
                        if(tmp){
                                ESBYTE[p] = tmp + '0';
                                $inc p
                                cmd = 0;
                        }
                }
                else {
                        ESBYTE[p] = tmp + '0';
                        $inc p
                }
                mask /= 10;
        }
        ESBYTE[p] = 0;
        return ret;
}

inline fastcall void mem_init()
{
        $mov     eax, 68
        $mov     ebx, 11
        $int     0x40
}

inline dword malloc(dword size)
{
        $push    ebx
        $push    ecx

        $mov     eax, 68
        $mov     ebx, 12
        $mov     ecx, size
        $int     0x40

        $pop     ecx
        $pop     ebx
        return  EAX;
}

inline stdcall dword realloc(dword mptr, size)
{
        $mov     eax, 68
        $mov     ebx, 20
        $mov     ecx, size
        $mov     edx, mptr
        $int     0x40

        return   EAX;
}

inline dword free(dword mptr)
{
        $mov     eax, 68
        $mov     ebx, 13
        $mov     ecx, mptr
        $test    ecx, ecx

        $int     0x40
        return 0;
}

inline byte allocSize(dword size)
{
    byte sizeABS = 0;
    WHILE (size)
    {
        size >>= 1;
        sizeABS++;
    }
    RETURN sizeABS;
}
inline dword malloc2(dword size)
{
    dword allocBytePosition = 0;
    dword key = 0;
    dword alloc = 0;
    word keyCurrent = 0;
    word keyLength = 0;
    byte allocSizePosition = allocSize(size);

    allocBytePosition = allocSizePosition * 4 + allocateBuffer;
    key = DSDWORD[allocBytePosition];
    IF (!key)
    {
        alloc = malloc(1<<allocSizePosition);
        DSBYTE[alloc] = allocSizePosition;
        return alloc+1;
    }

    keyLength = DSWORD[key];
    keyCurrent = DSWORD[key+2];

    IF (keyCurrent < 8)
    {
        alloc = malloc(1<<allocSizePosition);
        DSBYTE[alloc] = allocSizePosition;
        return alloc+1;
    }
    keyCurrent -= 4;
    DSWORD[key+2] = keyCurrent;
    key += keyCurrent;
    return DSDWORD[key]+1;
}

inline dword free2(dword address)
{
    byte allocSizePosition = 0;
    dword key = 0;
    word keyCurrent = 0;
    word keyLength = 0;
    dword len = 0;
    dword temp = 0;
    dword allocBytePosition = 0;
    address--;
    allocSizePosition = DSBYTE[address];
    allocBytePosition = allocSizePosition * 4 + allocateBuffer;
    key = DSDWORD[allocBytePosition];
    IF (!key)
    {
        DSDWORD[allocBytePosition] = malloc(0x1000);
        key = DSDWORD[allocBytePosition];
        DSWORD[key] = 0x1000;
        DSWORD[key+2] = 4;
    }
    keyLength = DSWORD[key];
    keyCurrent = DSWORD[key+2];
    DSWORD[key+2] = keyCurrent+4;

    IF (DSWORD[key+2] > keyLength)
    {
        DSDWORD[allocBytePosition] = realloc(key, keyLength<<1);
        key = DSDWORD[allocBytePosition];
        keyCurrent = DSWORD[key+2];
        DSWORD[key] = keyLength<<1;
    }
    key += keyCurrent;
    DSDWORD[key] = address;
    address++;
    temp = address;
    len = 1<<allocSizePosition-1;
    WHILE (len)
    {
        DSBYTE[temp] = 0;
        temp++;
        len--;
    }
    return address;
}

inline dword realloc2(dword address, dword size)
{
    dword newAddress = 0;
    dword ptr1 = 0;
    dword ptr2 = 0;
    dword len = 0;
    byte allocSizePosition = 0;
    IF (!address) RETURN malloc2(size);
    newAddress = malloc2(size);
    allocSizePosition = DSBYTE[address];
    len = 1<<allocSizePosition-1;
    IF (len > size) len = size;
    ptr1 = newAddress;
    ptr2 = address;
    WHILE (len)
    {
        DSBYTE[ptr1] = DSBYTE[ptr2];
        ptr1++;
        ptr2++;
        len--;
    }
    free2(address);
    return newAddress;
}

inline dword concatString(dword str1, dword str2)
{
    unsigned int len1 = 0;
    unsigned int len2 = 0;
    dword text = 0;
    len1 = strlen(str1);
    len2 = strlen(str2);
    text = malloc2(len1+len2+1);
    strcpy(text, str1);
    strcpy(text+len1, str2);
    return text;
}

inline fastcall unsigned int strlen( EDI)
{
        $xor eax, eax
        $mov ecx, -1
        $REPNE $SCASB
        EAX-=2+ECX;
}

inline fastcall void strcpy( EDI, ESI)
{
    $cld
L2:
    $lodsb
    $stosb
    $test al,al
    $jnz L2
}

void ______INIT______()
{
    mem_init();
    allocateBuffer = malloc(32*4);
    main();
}

______STOP______:
