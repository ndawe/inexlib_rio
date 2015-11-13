// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file bush.license for terms.

#ifndef bush_visual_unistd_h
#define bush_visual_unistd_h

//#include <windows.h>

#ifdef __cplusplus

extern "C" {
  __declspec(dllimport) void __stdcall Sleep(unsigned long);
}

inline void sleep(unsigned int a_secs) {
  ::Sleep(a_secs*1000);
}

#endif

#endif
