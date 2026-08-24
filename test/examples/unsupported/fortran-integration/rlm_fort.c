/**********************************************************************
 * Interface subroutines for RLM calls from Fortran                   *
 * (c) 2011-06-30 Petr Koci (petr.koci@vscht.cz)                      *
 * The source code can be freely distributed if authorship preserved. *
 * Tested with ifort and gcc under Linux 32bit.                       *
 * No warranty of functionality on your system.                       *
 **********************************************************************/

#include "license.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <strings.h>
#endif /* _WIN32 */

    void
rlm_init_fort_(char *path, char *argv0, char *licstrings, RLM_HANDLE *handle, int lenpath, int lenargv0, int lenlicstrings)
{
    /*
       printf("> rlm_init_fort_:\n");
       printf("path    %s %i \n", path, lenpath);
       printf("argv0   %s %i \n", argv0, lenargv0);
       printf("licstr  %s %i \n", licstrings, lenlicstrings);
       printf("handle  %i \n", *handle);
       printf("calling rlm...\n");
       */
    *handle = rlm_init(path, argv0, licstrings);
    /*
       printf("handle  %i \n", *handle);
       */
}

    void
rlm_stat_fort_(RLM_HANDLE *handle, int *status)
{
    /*
       printf("> rlm_stat_fort_:\n");
       printf("handle  %i \n", *handle);
       printf("status  %i \n", *status);
       printf("calling rlm...\n");
       */
    *status = rlm_stat(*handle);
    /*
       printf("status  %i \n", *status);
       */
}

    void
rlm_checkout_fort_(RLM_HANDLE *handle, char *product, char *version, int *count, RLM_LICENSE *license, int lenproduct, int lenversion)
{
    /*
       printf("> rlm_checkout_fort_:\n");
       printf("handle  %i \n", *handle);
       printf("product %s %i \n", product, lenproduct);
       printf("version %s %i \n", version, lenversion);
       printf("count   %i \n", *count);
       printf("license %i \n", *license);
       printf("calling rlm...\n");
       */
    *license = rlm_checkout(*handle, product, version, *count);
    /*
       printf("license %i \n", *license);
       */

}

    void
rlm_license_stat_fort_(RLM_LICENSE *license, int *status)
{
    /*
       printf("> rlm_stat_fort_:\n");
       printf("license %i \n", *license);
       printf("status  %i \n", *status);
       printf("calling rlm...\n");
       */
    *status = rlm_license_stat(*license); 
    /*
       printf("status  %i \n", *status);
       */
}


    void
rlm_errstring_fort_(RLM_LICENSE *license, RLM_HANDLE *handle, char *errstring, int lenerrstring)
{
    char string[RLM_ERRSTRING_MAX];
    int  i = 0;
    /*
       printf("> rlm_errstring_fort_:\n");
       printf("license %i \n", *license);
       printf("handle  %i \n", *handle);
       printf("maxlenc %i \n", RLM_ERRSTRING_MAX);
       printf("maxlenf %i \n", lenerrstring);
       printf("stringf %s \n", errstring);
       printf("calling rlm...\n");
       */
    rlm_errstring(*license, *handle, string);

    while( i < lenerrstring && i < RLM_ERRSTRING_MAX && string[i] != 0)
    {
        errstring[i] = string[i];
        i++;
    }
    while( i < lenerrstring )
    {
        errstring[i] = ' ';
        i++;
    }
    /*
       printf("stringc %s \n", string);
       printf("stringf %s \n", errstring);
       */

}

    void
rlm_checkin_fort_(RLM_LICENSE *license, int *status)
{
    /*
       printf("> rlm_checkin_fort_:\n");
       printf("license %i \n", *license);
       printf("status  %i \n", *status);
       printf("calling rlm...\n");
       */
    *status = rlm_checkin(*license);
    /*
       printf("status  %i \n", *status);
       */
}

    void
rlm_close_fort_(RLM_HANDLE *handle, int *status)
{
    /*
       printf("> rlm_close_fort_:\n");
       printf("handle  %i \n", *handle);
       printf("status  %i \n", *status);
       printf("calling rlm...\n");
       */
    *status = rlm_close(*handle);
    /*
       printf("status  %i \n", *status);
       */
}
