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
 *  Usage:      roam_example [ roam_value [product [ count [ ver ] ] ] ]
 *
 *  Return:     None
 *
 *  M. Christiano
 *  1/13/09
 *
 *  $Id: roam_example.c,v 1.17 2022/03/08 17:06:25 matt Exp $
 */

#include "license.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#define PUTENV putenv
#else
#define PUTENV _putenv
#endif /* _WIN32 */

static void printstat(RLM_HANDLE, RLM_LICENSE, const char *);

    int
main(int argc, char *argv[])
{
    RLM_HANDLE rh;
    RLM_LICENSE lic;
    int x, stat;
    const char *product = "test1";
    int count = 1;
    const char *ver = "1.0";
    char roamstring[100];
    int roamval;
#define REPSIZE 20
    char reply[REPSIZE+1];

    rh = rlm_init(".", argv[0], (char *) NULL);
    stat = rlm_stat(rh);
    if (stat)
    {
        char errstring[RLM_ERRSTRING_MAX];

        (void) printf("Error initializing license system\n");
        (void) printf("%s\n", rlm_errstring((RLM_LICENSE) NULL, rh, 
                    errstring));
        exit(1);
    }

    if (argc > 2) product = argv[2];
    if (argc > 3) count = atoi(argv[3]);
    if (argc > 4) ver = argv[4];
    if (argc > 1) 
    {
        roamval = atoi(argv[1]);
    }
    else
    {
        printf("Enter the value for the RLM_ROAM environment variable\n\n");
        printf("   A value < 0 will cause any roaming license for %s\n",
                product);
        printf("   to be returned to the server\n\n");
        printf("   A value > 0 will cause this program to use an \n");
        printf("   existing roaming license for %s, or set up a\n",
                product);
        printf("   roaming license if one does not exist.\n\n");
        printf("   A value of 0 (or <CR>) will cause RLM_ROAM to\n");
        printf("   remain unset which will cause the program to use\n");
        printf("   a floating license if it exists, or a roamed\n");
        printf("   license if no floating license is available.\n");

        (void) printf("\nEnter RLM_ROAM value: ");

        reply[0] = '\0';
        (void) fgets(reply, REPSIZE, stdin);
        if (!reply[0])  roamval = 0;
        else        roamval = atoi(reply);
        printf("\n");
    }
    /*
     *  Set the RLM_ROAM value in the environment
     */
    if (roamval)
    {
        sprintf(roamstring, "RLM_ROAM=%d", roamval);
        PUTENV(roamstring);
    }

    /*
     *  Depending on the setting of RLM_ROAM, here is what is going to happen:
     *
     *  - if RLM_ROAM is set to a positive number, and there is currently
     *    no roaming license on this computer, the checkout call
     *    will check out a floating license which will at the same time 
     *    create the roaming license.
     *
     *  - if RLM_ROAM is 0 (unset), the checkout will get a floating license 
     *    if one is available, otherwise it will use a roaming license if
     *    there is one.
     *
     *  - if RLM_ROAM is positive and there is currently a roaming license 
     *    on this computer, the checkout will get the roaming license.
     *
     *  - if RLM_ROAM is negative, any roaming license on this computer will
     *    be returned.
     *
     *  Note that in all cases where a roaming license is to be used,
     *  this client must be able to check out an "rlm_roam" license.
     *  The example license file has this license, so if you disconnect
     *  this computer from the network after you have created the roaming
     *  license on it, you must be sure that the example license file is
     *  accessible so that the rlm_roam license can be checked out.
     *  Another way to test this is to simply stop the license server
     *  and the checkout will succeed if you have previously caused a
     *  license to roam.
     */
    lic = rlm_checkout(rh, product, ver, count);
    printstat(rh, lic, product);

    (void) printf("Enter <CR> to continue: ");
    x = fgetc(stdin);
    stat = x;   /* To remove "x unused" warning */

    if (lic) rlm_checkin(lic);
    rlm_close(rh);
    return(0);
}

static
    void
printstat(RLM_HANDLE rh, RLM_LICENSE lic, const char *name)
{
    int stat;
    char errstring[RLM_ERRSTRING_MAX];

    stat = rlm_license_stat(lic);
    if (stat == 0)
    {
        int isroam = rlm_license_roaming(lic);

        (void) printf("Checkout of %s OK.\n", name);
        (void) printf("You have checked out a %s license.\n",
                isroam ? "ROAMING" : "FLOATING");
    }
    else if (stat == RLM_EL_INQUEUE)
    {
        (void) printf("Queued for %s license\n", name);
    }
    else
    {
        (void) printf("Error checking out %s license\n", name);
        (void) printf("%s\n", rlm_errstring(lic, rh, errstring));
    }
}
