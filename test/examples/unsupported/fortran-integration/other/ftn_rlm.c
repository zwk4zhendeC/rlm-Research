// *************************************************************
// *     C wrapper to RL routines for calls from Fortran      *
// *     (see also ftn_rlm_calls.f & ftn_rlm_test.f)           *
// *  VERSION 1.0            RSH                 08-Dec-2011   *
// *    Zentech International Ltd.   roger@zentech.co.uk       *
// *                   (please use freely)                     *
// *************************************************************
//---------------------------------------------------------------------
// ost of the RLM API routines can be called directly from ftn_rlm_calls.f
// Exceptions are functions returning a character string - these are 
// called via this wrapper.
// Tested on Windows with Intel Visual Fortran 11.1 + VS 2005, 
// and on Linux x64 with gcc
//---------------------------------------------------------------------
// misc:
// rlm_debug_c            - sets debug level
// rlm_maxstr_c           - returns RL_ERRSTRING_MAX length
//
// wrappers               > RL API
// rlm_license_dtls_c     > rlm_license_exp
//                        > rlm_license_contract
//                        > rlm_license_customer
//                        > rlm_license_issuer
//
#include "license.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <strings.h>
#endif /* _WIN32 */
//
int debug=0;
//
void rlm_debug_c(int *val)
{
    debug = *val;
}
//
int rlm_maxstr_c(void)
{
    if(debug>1)
    {
        printf("> rlm_maxstr_c:\n");
    }
    return RL_ERRSTRING_MAX;
}
//
void rlm_license_dtls_c(RL_LICENSE lic, int *opt, char *str)
{
    int i=0;
    char *tmp;
    if(debug>1)
    {
        printf("> rlm_license_dtls_c:\n");
        printf("lic     %i  \n", lic);
        printf("opt     %i  \n", *opt);
    }
    switch (*opt)
    {
        case 0: tmp=rlm_license_exp(lic);break;
        case 1: tmp=rlm_license_contract(lic);break;
        case 2: tmp=rlm_license_customer(lic);break;
        case 3: tmp=rlm_license_issuer(lic);break;
    }
    while (i<64 && tmp[i] != 0){ str[i] = tmp[i]; i++;}
}
//
