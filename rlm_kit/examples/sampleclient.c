/******************************************************************************

  COPYRIGHT (c) 2005, 2022 by Reprise Software, Inc.
  This software has been provided pursuant to a License Agreement
  containing restrictions on its use.  This software contains
  valuable trade secrets and proprietary information of 
  Reprise Software Inc and is protected by law.  It may not be 
  copied or distributed in any form or medium, disclosed to third 
  parties, reverse engineered or used in any manner not provided 
  for in said License Agreement except with the prior written 
  authorization from Reprise Software Inc.

 *****************************************************************************/
/*  
 *  Description:    Test client for LM system
 *
 *  Usage:      % sampleclient [product [count [version]]]
 *
 *  Return:     None
 *
 *  M. Christiano
 *  11/27/05
 *
 *  $Id: sampleclient.c,v 1.41 2022/03/08 17:06:27 matt Exp $
 */

#include "license.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <strings.h>
#endif /* _WIN32 */

static void printstat(RLM_HANDLE, RLM_LICENSE, const char *);

#ifdef x64_v1 /* No main on vxworks downloadable kernel modules, or argc/argv */
int sampleclient(char *product) 
{ 
    char *argv[1];
    argv[0] = product;
    return main(1, argv); 
}
#endif

    int
main(int argc, char *argv[])
{
    RLM_HANDLE rh;
    RLM_LICENSE lic = (RLM_LICENSE) NULL;
    int unused, stat;
    char *product, p[RLM_MAX_PRODUCT+1];
    int count = 1;
    const char *ver = "1.0";

    rh = rlm_init(".", argv[0], (char *) NULL);
    stat = rlm_stat(rh);
    if (stat)
    {
        char errstring[RLM_ERRSTRING_MAX];

        (void) printf("Error initializing license system\n");
        (void) printf("%s\n", rlm_errstring((RLM_LICENSE) NULL, rh, 
                    errstring));
    }
    else
    {

        /*
         *      Use the program name as the license name
         */
        if      ((product = strrchr(argv[0], (int) '/')))  product++;
        else if ((product = strrchr(argv[0], (int) '\\'))) product++;
        else                       product = argv[0];
        if (!strcmp(product, "rlmclient_http")) product = "rlmclient";
        strncpy(p, product, RLM_MAX_PRODUCT);
        p[RLM_MAX_PRODUCT] = '\0';
        /* 
         *      Don't want .exe 
         */
        if ((product = strchr(p, '.'))) *product = '\0';    
        product = p;
        /*
         *      If product name specified, override program name
         */
        if (argc > 1) product = argv[1];
        if (argc > 2) count = atoi(argv[2]);
        if (argc > 3) ver = argv[3];
        lic = rlm_checkout(rh, product, ver, count);
        printstat(rh, lic, product);
    }

    (void) printf("Enter <CR> to continue: ");
    unused = fgetc(stdin);

    if (lic) 
    {
#if 0
        /*
         *      rlm_checkin() isn't necessary if you aren't going
         *      to do anything else on the handle (other than check
         *      in licenses).   If you are using a handle created
         *      with rlm_init(), then rlm_checkin() doesn't hurt
         *      anything.  But if you use a handle created with
         *      rlm_init_disconn(), then rlm_checkin() causes an extra,
         *      unnecessary network connection to the license server.
         */
        rlm_checkin(lic);
#endif
        rlm_close(rh);
    }
    return(stat);
}

static
    void
printstat(RLM_HANDLE rh, RLM_LICENSE lic, const char *name)
{
    int stat;
    char errstring[RLM_ERRSTRING_MAX];

    stat = rlm_license_stat(lic);
    if (stat == 0)
        (void) printf("Checkout of %s OK.\n", name);
    else if (stat == RLM_EL_INQUEUE)
        (void) printf("Queued for %s license\n", name);
    else
    {
        (void) printf("Error checking out %s license\n", name);
        (void) printf("%s\n", rlm_errstring(lic, rh, errstring));
    }
}
