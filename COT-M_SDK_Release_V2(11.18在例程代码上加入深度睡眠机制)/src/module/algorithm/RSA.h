/**
 * \file RSA.h
 */
#ifndef _RSA_H
#define _RSA_H

#include <stdint.h>
//#include "bignum.h"


/************************************************************************************************
                                        �궨��                                                 
*************************************************************************************************/
#define MAX_RSA_MODULUS_BITS                2048
#define MIN_RSA_MODULUS_BITS                508
#define MAX_RSA_PRIME_BITS                  ((MAX_RSA_MODULUS_BITS +1)/2)   // 1024
#define MAX_RSA_MODULUS_LEN                 ((MAX_RSA_MODULUS_BITS +7)/8)   // 256
#define MAX_RSA_PRIME_LEN                   ((MAX_RSA_PRIME_BITS +7)/8) // 128 
//#define RSA_PUBEXPONENT_LEN             ((MAX_RSA_MODULUS_BITS +7)/8)   // 256
#define EM_rsa_EXP_3                       0x03
#define EM_rsa_EXP_10001                   0x10001
/*
 * RSA Error codes
 */
#define EM_err_RSA_BAD_INPUT_DATA       (uint8_t)0x8B  /* ��Ч���� */
#define EM_err_RSA_KEY_GEN_FAILED       (uint8_t)0x81  /* ���ɹ�˽Կ��ʧ�� */
#define EM_err_RSA_PUBLIC_FAILED        (uint8_t)0x83  /* ��Կ����ʧ�� */
#define EM_err_RSA_PRIVATE_FAILED       (uint8_t)0x85  /* ˽Կ����ʧ�� */

#define EM_ERRPARAM                     (uint8_t)0x86 


#define EM_SUCCESS                      (uint8_t)0x0 
#define EM_ERROR                        (uint8_t)0x1 
#define EM_NULL                         NULL



/*************************************************************************************
                                 ���ݽṹ
**************************************************************************************/
// ˽Կ�ṹ
typedef struct{
    uint32_t bits;
    unsigned char modulus[MAX_RSA_MODULUS_LEN];
    unsigned char publicExponent[MAX_RSA_MODULUS_LEN];
    unsigned char exponent[MAX_RSA_MODULUS_LEN];
    unsigned char prime[2][MAX_RSA_PRIME_LEN];
    unsigned char primeExponent[2][MAX_RSA_PRIME_LEN];
    unsigned char coefficient[MAX_RSA_PRIME_LEN];
}R_RSA_PRIVATE_KEY;

// ��Կ�ṹ
typedef struct{
    uint32_t bits;
    unsigned char modulus[MAX_RSA_MODULUS_LEN];
    unsigned char exponent[MAX_RSA_MODULUS_LEN];
}R_RSA_PUBLIC_KEY;

/**************************************************************************************
                                  ����ԭ��        
***************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int rsa_gen_key( uint32_t nbits, uint32_t exponent, R_RSA_PRIVATE_KEY *privateKey );

int rsa_public( uint32_t inputLen, const unsigned char *input, 
                uint32_t *outputLen, unsigned char *output,                 
                R_RSA_PUBLIC_KEY *publicKey );

int rsa_private(uint32_t inputLen, const unsigned char *input, 
                uint32_t *outputLen, unsigned char *output, 
                R_RSA_PRIVATE_KEY *privateKey );

/**
 * New RSA �ӿں���
 */
 
/* ��˽Կ�����ɺ��� �ⲿ�ӿ�*/
uint8_t EA_ucGenerateRSAKeyPair(uint32_t uiModLen, uint32_t uiExp, R_RSA_PRIVATE_KEY *ptPrivateKey);

/* ��Կ���ܺ��� �ⲿ�ӿ�*/
uint8_t EA_ucRSAPublicBlock(uint32_t uiLenIn,
						     void *pvDataIn,
						     uint32_t *puiOutLen,
						     void *pvDataOut,
						     R_RSA_PUBLIC_KEY *tPublicKey
						     );
/* ˽Կ���ܺ��� �ⲿ�ӿ�*/
uint8_t EA_ucRSAPrivateBlock(uint32_t uiLenIn,
						    void *pvDataIn,
						    uint32_t *puiOutLen,
						    void *pvDataOut,
						    R_RSA_PRIVATE_KEY *tPrivateKey								
						    );

#ifdef __cplusplus
}
#endif

#endif /* rsa.h */

