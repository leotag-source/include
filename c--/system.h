#pragma option OST
#pragma option ON
#pragma option cri-
#pragma option -CPA
#initallvar 0
#jumptomain FALSE
#startaddress 0
#code32 TRUE

char   os_name[8]   = {'M','E','N','U','E','T','0','1'};
dword  os_version   = 0x00000001;
dword  start_addr   = #______INIT______;
dword  final_addr   = #______STOP______+32;
dword  alloc_mem    = 100000;
dword  x86esp_reg   = 100000;
dword  I_Param      = #param;
dword  I_Path       = #program_path;
char param[4096];
char program_path[4096];

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

void ______INIT______()
{
  main();
}

______STOP______:
