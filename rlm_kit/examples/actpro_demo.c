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
 *  Description:    Demo client for RLM Activation Pro
 *
 *  M. Christiano
 *  2/12/11
 *  Derived from act_api_example.c
 *
 *  $Id: actpro_demo.c,v 1.21 2022/03/08 17:06:25 matt Exp $
 */

#include "license.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int checkstat(RLM_HANDLE, RLM_LICENSE, const char *);
static int doactivation(RLM_HANDLE, const char *);

/*
 *  Configuration  -  Change the URL definition below.
 */

#define PRODUCT "actdemo"       /* Use the "actdemo" product */
#define ISV "demo"          /* For Demo on kit */
#define URL "http://demoN.hostedactivation.com"

/*  If SIMPLE_ACTIVATION is defined, this test program does a simple 
 *  one-product activation.  If not defined, we activate with an 
 *  Alternate Nodelock Hostid.  
 *
 *  You need the 2 activation keys set up properly to do this, and 
 *  we recommend you do the simple activation and become familiar
 *  with it first. 
 */

#if 1
#define SIMPLE_ACTIVATION   
#endif

    int
main(int argc, char *argv[])
{
    RLM_HANDLE rh = (RLM_HANDLE) NULL;
    RLM_LICENSE lic = (RLM_LICENSE) NULL;
    int x, stat;
    const char *product = PRODUCT;
    int pass;

    for (pass = 1; pass <= 2; pass++)
    {
        if (pass == 2)
        {
            /*
             *      We did not get the license on the first pass.  See
             *      if we can activate it now.
             */
            stat = doactivation(rh, product);
            rlm_close(rh);      /* Close the old handle */
            if (stat < 0) exit(2);
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
        (void) printf("Enter <CR> to continue: ");
        x = fgetc(stdin);
        stat=x; stat=0; /* To remove "x unused" warning */
        if (lic) rlm_checkin(lic);
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
checkstat(RLM_HANDLE rh, RLM_LICENSE lic, const char *name)
{
    int stat;
    char errstring[RLM_ERRSTRING_MAX];

    stat = rlm_license_stat(lic);
    if (stat == 0)
        (void) printf("Checkout of %s license OK.\n", name);
    else
    {
        (void) printf("Error checking out %s license\n", name);
        (void) printf("%s\n", rlm_errstring(lic, rh, errstring));
    }
    return(stat);
}

#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#endif

/*
 *  Write out a license file.
 */

static 
    int
write_lf(char *prefix, char *license)
{
    /*
     *  Activation was successful.  Write the license out.  
     *  Note in this example, we try the license file name 
     *  <prefix>N.lic, and we only try 100 different names.  You 
     *  should change this to whatever naming convention you 
     *  want to use.
     */
    char name[100];
    int attempt;
    FILE *f;
    int stat = 0;

    for (attempt=0; attempt<100; attempt++)
    {
        sprintf(name, "%s%d.lic", prefix, attempt);
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
                fprintf(f, "This license created by RLM Activation Pro\n");
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
 *  Print an activation error
 */

    static void
print_error(RLM_HANDLE rh, int stat, char *akey, char *url)
{
    printf("\n");
    switch(stat)
    {
        char err[RLM_ERRSTRING_MAX+1];

        case RLM_EH_CANTCONNECT_URL:
        printf("You were unable to connect to %s.\n", url);
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

        default:
        printf("Error %d requesting activation\n%s\n", stat, 
                RLM_ACT_ERR(stat) ? 
                rlm_act_errstring(stat) : 
                rlm_errstring(0, rh, err));
        break;

    }
    printf("\n");
}

#ifdef SIMPLE_ACTIVATION        /* Standard, simple activation */

static
    int
doactivation(RLM_HANDLE rh, const char *name)
{
    char license[3 * RLM_MAX_LINE + 1];   /* Allow for HOST, ISV, and LICENSE */
    char akey[RLM_MAX_LINE+1];
    int stat = RLM_EH_READ_NOLICENSE; /* If they say NO, no license */
    int len;

    (void) printf("\nWould you like to activate this license now? ");
    fgets(akey, RLM_MAX_LINE, stdin);
    if (*akey == 'y' || *akey == 'Y')
    {
        (void) printf("Activation will be attempted for ISV %s at %s\n",
                ISV, URL);
        (void) printf("Enter Activation key for \"%s\": ", name);
        fgets(akey, RLM_MAX_LINE, stdin);
        len = ((int) strlen(akey)) - 1;
        if (akey[len] == '\n') akey[len] = '\0';
        /*
         *      Request the license
         */
        stat = rlm_act_request(
                rh,                   /* RLM handle */
                URL,                  /* URL */
                ISV,                  /* ISV */
                akey,                 /* Activation key */
                "",       /* hostid list - empty for local machine */
                "",       /* hostname - empty for local machine */
                1,    /* count - # of licenses */
                "",       /* extra license parameters */
                license); /* Space for the returned license */

        if ((stat == 0) || (stat == 1))
        {
            stat = write_lf("a", license);
        }
        else
        {
            print_error(rh, stat, akey, URL);
        }
    }
    return(stat);
}

#else   

/*  
 *  Activation using an Alternate Nodelock Hostid hostid.
 *  For this to work, you need 2 activation keys, the first one
 *  should be an Alternate Nodelock Hostid type, and the 2nd one
 *  should only allow an "Alternate Server Hostid" as it's hostid
 *
 *  In addition, this example assumes the alternate nodelock hostid 
 *  is locked to a rehostable hostid.
 *
 */

static
    int
doactivation(RLM_HANDLE rh, const char *name)
{
    char license[3 * RLM_MAX_LINE + 1];   /* Allow for HOST, ISV, and LICENSE */
    char akey[RLM_MAX_LINE+1];
    int stat = RLM_EH_READ_NOLICENSE; /* If they say NO, no license */
    int len;
    RLM_ACT_HANDLE act_handle;
    RLM_HANDLE rh2;
    RLM_LICENSE l;
    char hostid[RLM_MAX_HOSTID_STRING+1];

    (void) printf("\nWould you like to activate this license now? ");
    fgets(akey, RLM_MAX_LINE, stdin);
    if (*akey == 'y' || *akey == 'Y')
    {
        (void) printf("Activation will be attempted for ISV %s at %s\n",
                ISV, URL);
        /*
         *      First, revoke any rehostable for "rlm_nodelock" to clean up
         *      from prior runs.  
         */
        rlm_act_revoke(rh, URL, "rlm_nodelock");

        (void) printf("Enter ANH Activation key: ");
        fgets(akey, RLM_MAX_LINE, stdin);
        len = ((int) strlen(akey)) - 1;
        if (akey[len] == '\n') akey[len] = '\0';
        /*
         *      Request the license
         */
        act_handle = rlm_act_new_handle(rh);
        rlm_act_set_handle(act_handle, RLM_ACT_HANDLE_REHOST, (void *) 1);
        stat = rlm_activate(
                rh,         /* RLM handle */
                URL,        /* URL */
                akey,       /* Activation key */
                1,      /* count - # of licenses */
                license,    /* Space for the returned license */
                act_handle); 
        rlm_act_destroy_handle(act_handle);

        if ((stat == 0) || (stat == 1))
        {
            stat = write_lf("anh", license);
        }
        else
        {
            print_error(rh, stat, akey, URL);
            return(stat);
        }
        /*
         *      OK, now activate the real license
         */
        {
            hostid[0] = '\0';
            /*
             *      get the hostid. This involves a checkout and getting the options
             *      string.
             */
            rh2 = rlm_init(".", 0, 0);
            l = rlm_checkout(rh2, "rlm_nodelock", "1.0", 1);

            if (l) 
            {
                char *o;

                o = rlm_license_options(l);
                if (o)
                {
                    sprintf(hostid, "license=%s", o);
                    rlm_checkin(l);
                }
                else 
                {
                    printf("rlm_license_options() returns NULL\n");
                    return(-1);
                }
            }
            else printf("could not checkout rlm_nodelock license\n");
            printf("ANH hostid is %s\n", hostid);

            (void) printf("Enter product Activation key for \"%s\": ",name);
            fgets(akey, RLM_MAX_LINE, stdin);
            len = ((int) strlen(akey)) - 1;
            if (akey[len] == '\n') akey[len] = '\0';
            /*
             *          Request the license
             */
            act_handle = rlm_act_new_handle(rh);
            rlm_act_set_handle(act_handle, RLM_ACT_HANDLE_HOSTID_LIST, 
                    hostid);
            stat = rlm_activate(
                    rh2,        /* RLM handle */
                    URL,        /* URL */
                    akey,       /* Activation key */
                    1,      /* count - # of licenses */
                    license,    /* Space for the returned license */
                    act_handle); 
            rlm_act_destroy_handle(act_handle);

            if ((stat == 0) || (stat == 1))
            {
                stat = write_lf("a", license);
            }
            else
            {
                print_error(rh2, stat, akey, URL);
                return(stat);
            }
        }
    }
    return(stat);
}
#endif
