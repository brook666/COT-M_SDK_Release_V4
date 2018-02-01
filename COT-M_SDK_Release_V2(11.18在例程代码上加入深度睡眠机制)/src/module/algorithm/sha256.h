//------------------------------------------------------------------------
// $Id
//
// Copyright (C), 2015-, DS IOT Team Information Co.,Ltd,All Rights Reserved
//
// FileName: sha256.h
//
// Author: zhangjianhua
//
// Version: 1.0
//
// Date: 2015-11-20
//
// Description: sha256Ëã·¨Ä£¿é
//
//
// Function List:
//
// History:
//--------------------------------------------------------------------------

#ifndef _SHA256_H
#define _SHA256_H

#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif


typedef unsigned __int64 uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

#define SHA256_HASH_SIZE 32

/* Hash size in 32-bit words */
#define SHA256_HASH_WORDS 8

struct _SHA256Context {
  uint64_t totalLength;
  uint32_t hash[SHA256_HASH_WORDS];
  uint32_t bufferLength;
  union {
    uint32_t words[16];
    uint8_t bytes[64];
  } buffer;
#ifdef RUNTIME_ENDIAN
  int littleEndian;
#endif /* RUNTIME_ENDIAN */
};

typedef struct _SHA256Context SHA256Context;


void SHA256Init (SHA256Context *sc);
void SHA256Update (SHA256Context *sc, const void *data, uint32_t len);
void SHA256Final (SHA256Context *sc, uint8_t hash[SHA256_HASH_SIZE]);

int sha256_test ( void );

#ifdef __cplusplus
}
#endif

#endif /* !_SHA256_H */
