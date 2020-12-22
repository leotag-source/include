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
dword  alloc_mem    = 100000;
dword  x86esp_reg   = 100000;
dword  I_Param      = 0;
dword  I_Path       = 0;
char param[4096]={0};
char program_path[4096]={0};

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

inline fastcall void mem_init()
{
        $mov     eax, 68
        $mov     ebx, 11
        $int     0x40
}

:dword malloc(dword size)
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

:stdcall dword realloc(dword mptr, size)
{
        $push    ebx
        $push    ecx
        $push    edx

        $mov     eax, 68
        $mov     ebx, 20
        $mov     ecx, size
        $mov     edx, mptr
        $int     0x40

        $pop     edx
        $pop     ecx
        $pop     ebx
        return   EAX;
}

:dword free(dword mptr)
{
        $push    eax
        $push    ebx
        $push    ecx

        $mov     eax, 68
        $mov     ebx, 13
        $mov     ecx, mptr
        $test    ecx, ecx
        $jz      end0
        $int     0x40
   @end0:
        $pop     ecx
        $pop     ebx
        $pop     eax
        return 0;
}


dword allocateBuffer[32] = {0};


byte allocSize(dword size)
{
    byte sizeABS = 0;
    while (size)
    {
        size >>= 1;
        sizeABS++;
    }
    return sizeABS;
}
dword malloc2(dword size)
{
    dword allocBytePosition = 0;
    dword key = 0;
    dword alloc = 0;
    word keyCurrent = 0;
    word keyLength = 0;
    byte allocSizePosition = allocSize(size);
    allocBytePosition = allocSizePosition * 4 + #allocateBuffer;
    key = DSDWORD[allocBytePosition];
    if (!key)
    {
        alloc = malloc(1<<allocSizePosition);
        DSBYTE[alloc] = allocSizePosition;
        return alloc+1;
    }

    keyLength = DSWORD[key];
    keyCurrent = DSWORD[key+2];

    if (keyCurrent < 8)
    {
        alloc = malloc(1<<allocSizePosition);
        DSBYTE[alloc] = allocSizePosition;
        return alloc+1;
    }

    DSWORD[key+2] = DSWORD[key+2] - 4;
    keyCurrent = DSWORD[key+2];

    return DSDWORD[key+keyCurrent]+1;
}

dword free2(dword address)
{
    byte allocSizePosition = 0;
    dword key = 0;
    word keyCurrent = 0;
    word keyLength = 0;
    dword allocBytePosition = 0;
    allocSizePosition = DSBYTE[address];
    allocBytePosition = allocSizePosition * 4 + #allocateBuffer;
    key = DSDWORD[allocBytePosition];
    if (!key)
    {
        DSDWORD[allocBytePosition] = malloc(0x1000);
        key = DSDWORD[allocBytePosition];
        DSWORD[key] = 0x1000;
        DSWORD[key+2] = 4;
    }
    keyLength = DSWORD[key];
    keyCurrent = DSWORD[key+2];
    DSWORD[key+2] = keyCurrent+4;
    if (DSWORD[key+2] > keyLength)
    {
        DSDWORD[allocBytePosition] = realloc(key, keyLength<<1);
        key = DSDWORD[allocBytePosition];
        keyCurrent = DSWORD[key+2];
        DSWORD[key] = keyLength<<1;
    }
    DSDWORD[keyCurrent+4] = address-1;
    return address;
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
    main();
}

______STOP______:
