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
 *  Description:    Example client for RLM license rehosting/revoking
 *
 *  M. Christiano
 *  11/18/11 - modified from the regular activation example
 *
 *  $Id: rehost_example.c,v 1.22 2022/03/08 17:06:26 matt Exp $
 */

#include "license.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL "hostedactivation.com"
#define ISVNAME "rlmactdemo"
#define PRODUCT "actdemo"

static int checkstat(RLM_HANDLE, RLM_LICENSE, const char *);
static int doactivation(RLM_HANDLE, const char *);
static int retrieve_license(RLM_HANDLE, const char *);
static int dodeactivation(RLM_HANDLE, const char *);
char akey[RLM_MAX_LINE+1];

    int
main(int argc, char *argv[])
{
    RLM_HANDLE rh = (RLM_HANDLE) NULL;
    RLM_LICENSE lic = (RLM_LICENSE) NULL;
    int stat;
    const char *product = PRODUCT;
    int pass;

    akey[0] = '\0';
    for (pass = 1; pass <= 3; pass++)
    {
        if (pass == 3)
        {
            /*
             *      We did not get the license on the first pass, and
             *      activation didn't work.  See if we can figure out what
             *      happened and correct it.
             */
            if (stat == RLM_EH_REHOST_EXISTS) 
            {
                /*
                 *          The rehostable hostid exists for the "actdemo" product.
                 *          Most likely, this is because the license file was deleted
                 *          or misplaced.  In this case, retrieve the rehostable hostid
                 *          and request a "standard" activation with that hostid.
                 */
                printf("The rehostable hostid exists.  We will now\n");
                printf("attempt to re-activate using this hostid.\n\n");
                stat = retrieve_license(rh, product);
                rlm_close(rh);      /* Close the old handle */
                if (stat < 0) exit(2);
            }
        }
        else if (pass == 2)
        {
            /*
             *      We did not get the license on the first pass.  See
             *      if we can activate it now.
             */
            stat = doactivation(rh, product);
            if (stat == RLM_EH_REHOST_EXISTS) continue;
            rlm_close(rh);      /* Close the old handle */
        }
        /*
         *      NOTE that rlm_init() is in this loop.  This is necessary
         *      because if the license is not acquired on the first pass,
         *      the activation request (above) will create a new
         *      license file.  rlm_init() must be called to see this
         *      new license file.
         */
        rh = rlm_init(".", argv[0], (char *) NULL);
        stat = rlm_stat(rh);
        if ((pass == 1) && (stat == RLM_EH_READ_NOLICENSE))
        {
            /*
             *      We didn't find a license.  Checkout won't work, so just
             *      skip to the 2nd pass where we try to activate it.
             */
            continue;
        }
        else if (stat)
        {
            (void) printf("Error %d initializing license system\n", stat);
            exit(1);
        }

        lic = rlm_checkout(rh, product, "1.0", 1);

        stat = checkstat(rh, lic, product);

        if ((pass == 1) && (stat < 0))
        {
            /*
             *      Didn't get the license.  Try a 2nd time to activate it.
             */
            if (lic) rlm_checkin(lic);
            lic = (RLM_LICENSE) NULL;
            continue;
        }
        else if (stat == 0)
        {
            /*
             *          We got the license
             */
            break;
        }
    }

    if (stat == 0)
    {
        /*
         *      We got the license
         */
        if (lic) rlm_checkin(lic);
        printf("License valid.\n");
        stat = dodeactivation(rh, product);
        rlm_close(rh);
    }
    else if (pass == 2)
    {
        /*
         *      checkout and/or activation failed.
         */
        (void) 
            printf("Unable to check out/activate \"actdemo\" license\n");
    }
    return(0);
}

static
    int
checkstat(RLM_HANDLE rh, RLM_LICENSE lic, const char *product)
{
    int stat;
    char errstring[RLM_ERRSTRING_MAX];

    stat = rlm_license_stat(lic);
    if (stat == 0)
        (void) printf("Checkout of %s license OK.\n", product);
    else
    {
        (void) printf("Error checking out %s license\n", product);
        (void) printf("%s\n", rlm_errstring(lic, rh, errstring));
    }
    return(stat);
}

#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#endif

/*
 *  write_license() - write the license file out.
 */
static
    int
write_license(char *license)
{
    char name[100];
    int try;
    int stat=0;
    FILE *f;
    /*
     *  Activation was successful.  Write the license out.  
     *  Note in this example, we try the license file name 
     *  aN.lic, and we only try 100 different names.  You 
     *  should change this to whatever naming convention you 
     *  want to use.
     */
    for (try=0; try<100; try++)
    {
        sprintf(name, "a%d.lic", try);
        f = fopen(name, "r");
        if (f == (FILE *) NULL)
        {
            struct tm *t;
#ifdef _WIN32
            time_t ltime;

            time(&ltime);
            t = localtime(&ltime);
#else
            struct timezone tz;
            struct timeval tv;
            time_t x;

            gettimeofday(&tv, &tz);
            x = tv.tv_sec;
            t = localtime((time_t *) &x);
#endif

            f = fopen(name, "w");
            if (f)
            {
                fprintf(f, "This license created by RLM Internet Activation\n");
                if (t)
                    fprintf(f, "Created on %02d/%02d/%04d at %02d:%02d\n", 
                            t->tm_mon+1, t->tm_mday, 
                            t->tm_year+1900, t->tm_hour, 
                            t->tm_min);

                fprintf(f, "\n%s\n", license);
                fclose(f);
                printf(
                        "Activation successful, license file \"%s\" written\n",
                        name);
                break;
            }
            else
            {
                printf( "Error writing license file \"%s\"\n", name);
                stat = -1;
                break;
            }
        }
    }
    return(stat);
}

/*
 *  display_error() - display the error return from activation.
 */
static
    void
display_error(RLM_HANDLE rh, int stat)
{
    printf("\n");
    switch(stat)
    {
        char err[RLM_ERRSTRING_MAX+1];

        case RLM_EH_CANTCONNECT_URL:
        printf("You were unable to connect to %s.\n", URL);
        printf("Please make sure that this system is able to\n");
        printf("access the internet and try again.\n");
        break;

        case RLM_ACT_NO_KEY_MATCH:
        printf("The activation key you supplied (%s) was \n", akey);
        printf("not found.  Please check the key and ensure\n");
        printf("that you have entered it correctly.\n");
        break;

        case RLM_ACT_KEY_USED:
        printf("The activation key you supplied (%s)\n", akey);
        printf("has already been used to activate a license.\n");
        printf("Please check the key and ensure that you have entered it correctly.\n");
        break;

        case RLM_EH_BAD_HTTP:
        printf("Bad HTTP transaction\n%s\n", rlm_errstring(0, rh, err));
        break;

        case 0:
        break;

        default:
        printf("Error %d requesting activation\n%s\n", stat, 
                RLM_ACT_ERR(stat) ? 
                rlm_act_errstring(stat) : 
                rlm_errstring(0, rh, err));
        break;

    }
    printf("\n");
}

/*
 *  Prompt for the activation key, if not entered already.
 */
static
    int
get_akey(const char *product)
{
    int len;

    if (*akey) return(0);

    (void) printf("\nWould you like to activate this license now? ");
    fgets(akey, RLM_MAX_LINE, stdin);
    if (*akey == 'y' || *akey == 'Y')
    {
        (void) printf("Enter Activation key for \"%s\": ", product);
        fgets(akey, RLM_MAX_LINE, stdin);
        len = ((int) strlen(akey)) - 1;
        if (akey[len] == '\n') akey[len] = '\0';
        return(0);
    }
    return(1);
}

/*
 *  doactivation() - Perform the activation request.
 */

static
    int
doactivation(RLM_HANDLE rh, const char *product)
{
    char license[3 * RLM_MAX_LINE + 1];   /* Allow for HOST, ISV, and LICENSE */
    int stat = RLM_EH_READ_NOLICENSE; /* If they say NO, no license */
    RLM_ACT_HANDLE act_handle;

    if (get_akey(product)) return(stat);
    /*
     *  Request the license.  First make a handle, and tell it we
     *  want a rehostable hostid to activate.
     */
    act_handle = rlm_act_new_handle(rh);
    rlm_act_set_handle(act_handle, RLM_ACT_HANDLE_REHOST, (void *) 1);
    /*
     *  Note - you would normally never need to make this next call, 
     *  but we do it here so that we can connect to the license 
     *  generator on the reprise demo site.
     */
    rlm_act_set_handle(act_handle, RLM_ACT_HANDLE_ISV, ISVNAME);

    stat = rlm_activate(
            rh,         /* RLM handle */
            URL,
            akey,       /* Activation key */
            1,      /* count - # of licenses */
            license,    /* Space for the returned license */
            act_handle  /* Activation handle data */
            );

    rlm_act_destroy_handle(act_handle); /* Done with this */

    if ((stat == 0) || (stat == 1)) stat = write_license(license);
    display_error(rh, stat);
    return(stat);
}

/*
 *  This routine handles the case where the license was activated, but
 *  the license file was lost/misplaced.  Get the hostid and re-activate
 *  using it.
 */

static
    int
retrieve_license(RLM_HANDLE rh, const char *product)
{
    char license[3 * RLM_MAX_LINE + 1];   /* Allow for HOST, ISV, and LICENSE */
    char hostid[RLM_MAX_HOSTID_STRING+1];
    int stat = RLM_EH_READ_NOLICENSE; /* If they say NO, no license */
    RLM_ACT_HANDLE act_handle;

    if (get_akey(product)) return(stat);
    /*
     *  Get the rehostable hostid for this product.
     */
    stat = rlm_get_rehost(rh, PRODUCT, hostid);
    if (stat)
    {
        printf("Rehostable hostid does not exist, error: %d\n", stat);
        return(stat);
    }
    printf("%s product has hostid %s\n", PRODUCT, hostid);
    /*
     *  Re-Request the license using the hostid we just retrieved.  
     */
    act_handle = rlm_act_new_handle(rh);
    rlm_act_set_handle(act_handle, RLM_ACT_HANDLE_HOSTID_LIST, hostid);
    /*
     *  Note - you would normally never need to make this next call, 
     *  but we do it here so that we can connect to the license 
     *  generator on the reprise demo site.
     */
    rlm_act_set_handle(act_handle, RLM_ACT_HANDLE_ISV, ISVNAME);

    stat = rlm_activate(
            rh,         /* RLM handle */
            URL,
            akey,       /* Activation key */
            1,      /* count - # of licenses */
            license,    /* Space for the returned license */
            act_handle  /* Activation handle data */
            );

    rlm_act_destroy_handle(act_handle); /* Done with this */

    if ((stat == 0) || (stat == 1)) stat = write_license(license);
    display_error(rh, stat);
    return(stat);
}

static
    int
dodeactivation(RLM_HANDLE rh, const char *product)
{
    int stat = RLM_EH_READ_NOLICENSE; /* If they say NO, no license */
    char x[100];

    (void) printf("\nWould you like to deactivate the \"%s\" license now? ",
            product);
    fgets(x, RLM_MAX_LINE, stdin);
    if (*x == 'y' || *x == 'Y')
    {
        /*
         *      Request the deactivation.
         */
        stat = rlm_act_revoke(rh, URL, (char *) product);

        if (stat == 0)
        {
            /*
             *              revoke (Deactivation) was successful. 
             */
            printf("License successfully revoked\n");
        }
        else
        {
            char err[RLM_ERRSTRING_MAX+1];

            printf("\n");
            printf("Error %d requesting license revoke\n%s\n", 
                    stat, 
                    RLM_ACT_ERR(stat) ? 
                    rlm_act_errstring(stat) : 
                    rlm_errstring(0, rh, err));
            printf("\n");
        }
    }
    return(stat);
}
