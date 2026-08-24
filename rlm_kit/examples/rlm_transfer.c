/******************************************************************************

  COPYRIGHT (c) 2009, 2022 by Reprise Software, Inc.
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
 *  Function:   Example ISV-defined server transfer definition code
 *
 *          This code performs an RLM-RLM license transfer.
 *
 *          While this is not useful (since the functionality is
 *          already built into RLM), it serves as an example for 
 *          how to integrate license transfer from another license 
 *          manager.
 *
 *      Extern function in this module:
 *
 *          rlm_ix_setup(void *ls) - sets up all your callbacks.
 *
 *      Support functions (these are the functions you would write
 *          to support another license manager.  This example
 *          treats RLM as a separate license manager):
 *
 *          _check_auth_rlm(void *ls, char *product, 
 *                      void *lm_handle)
 *              - checks that a checked-out license is
 *                valid for a transfer.  Several things
 *                could make it invalid - e.g. if it is
 *                a user- or host- based license, we don't
 *                want to transfer it.   Returns 0 if the
 *                license is OK, or -1 if it is invalid.
 *                If you return -1, your checkin and close
 *                function will be called by RLM (_checkin_rlm
 *                in this example).
 *
 *          _fillin_auth_rlm(void *ls, void *lm_handle, 
 *                  void *license_handle, void *auth, 
 *                  void *product, int count)
 *              - fills in a license auth from the checked-out 
 *                license.  License parameters in "lm_handle",
 *                and/or "license_handle".
 *                Some other license managers (e.g. FLEXlm)
 *                don't have a license handle but use the
 *                lm handle along with the product name. 
 *
 *          _update_status_rlm(void *ls, char *server, int status, 
 *                    void *lm_handle, int *passes_to_check)
 *              - update status if checkout fails
 *                Returns 0 if the checkout can be attempted
 *                again, or 1 if it will never succeed.
 *
 *                Also, passes_to_check should be updated
 *                with the number of passes before another
 *                checkout should be attempted.  Each pass
 *                is one minute later.  So, for example, if
 *                the license server is down, we wait 10
 *                passes (10 minutes) before attempting the
 *                checkout again.
 *
 *          _open_rlm(void *ls, char *lmname, char *host, 
 *                  void **lm_handle, int *unavailable)
 *              - creates the RLM handle that will be used
 *                  for the license checkout, and sets
 *                  the various attributes.  Returns the
 *                  RLM_HANDLE in the "lm_handle" parameter.
 *
 *          _checkout_rlm(void *ls, void *lm_handle, char *product, 
 *                      char *ver, int count, 
 *                      void **license_handle)
 *              - performs the RLM license checkout.  Returns
 *                  the license handle in "license_handle"
 *
 *          _hb_rlm(void *lm_handle, void *license_handle)
 *              - does a heartbeat on the handle; returns status
 *              returns 0 for good status, LM error for error.
 *
*          _checkin_rlm(void *lm_handle, void *license_handle)
    *              - performs the RLM checkin and 
    *                  frees the license_handle.
    *
*          _close_rlm(void *lm_handle)
    *              - performs the RLM handle close and
    *                  frees the handle.
    *
    *
    *      There are 2 RLM support functions used:
    *
*          rlm_ix_log(void *ls, char *errtxt)
    *
    *              - logs a message to the ISV server debug log.
    *              pass "ls", the RLM license server handle, from 
    *              the calling parameters, and a string to log.
    *
    *          rlm_ix_update_auth(void *ls, void *auth, int what, 
            *                          int ival, char *sval);
*
*              - updates the server's license "auth" data
*              with parameters from the transferred license.
*              "what" is which parameter to update (see
        *              license.h for definitions), if the value of 
*              "what" is an integer, put the value into ival, 
    *              if it is a string, put the pointer into sval.
    *          
    *
    *  Parameters: (void *) ls - RLM license server handle
*               (used only to pass through to other fcns)
    *          (char *) product - product name transferred
    *          (char *) ver - version of product to transfer
    *          (void *) lm_handle - the RLM_HANDLE
    *          (void *) auth - RLM license auth handle
*               (used only to pass through to other fcns)
    *          int count - # licenses transferred
    *          (char *) server - server where transfer originated
    *          int status - checked-out license status on update
    *          (void *) license_handle - license handle
    *  
    *  Return:     int *passes_to_check - set to the # of passes before
    *              the license checkout will be attempted again
    *          int *unavailable - set to non-zero if this license is
    *              unavailable (never checked again)
    *          (license_handle returned from checkout entry point)
*          (lm_handle returnd from open entry point)
    *
    *  M. Christiano
    *  6/29/09
    *
    *  $Id: rlm_transfer.c,v 1.18 2022/03/08 17:06:25 matt Exp $
    */

#include "license.h"

    /*
     *  Include the other LM headers here.   Not needed for RLM.
     */


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

    /*
     *  Check that an acquired license is OK for use
     *
     *  For RLM licenses, we make sure the license is not a user-based,
     *  host-based, named-user, or token-based license.
     *
     *  For other license managers, other types of license may be
     *  ineligible for transfer.  You can decide what is, and what
     *  is not transferred by rejecting or accepting it here.
     *
     *  product: product name
     *  lm_handle: lm handle  (RLM_HANDLE for rlm, cast to void *)
     *  license_handle: license handle (RLM_LICENSE for rlm, cast to void *)
     *
     *  Return: 0 for OK, -1 to reject the transfer.
     */

    static
    int
_check_auth_rlm(void *ls, char *product, void *lm_handle, void *license_handle)
{
    RLM_LICENSE lic = (RLM_LICENSE) license_handle;
    char errtxt[200];

    /*
     *  Note: the "lm_handle" parameter is unused for RLM, but may be used in
     *  other license managers (like FLEXlm, for example).
     */

    if (rlm_license_named_user_count(lic)) 
    {
        sprintf(errtxt, 
                "ERROR: Named User licenses (%s) cannot be transfered\n",
                product);
        rlm_ix_log(ls, errtxt);
        return(-1);
    }
#ifdef _WIN32
    else if (stricmp(product, rlm_license_product(lic))) 
#else
    else if (strcasecmp(product, rlm_license_product(lic))) 
#endif
    {
        sprintf(errtxt, 
                "ERROR: Token-based licenses (%s) cannot be transfered\n",
                product);
        rlm_ix_log(ls, errtxt);
        return(-1);
    }
    else if (rlm_license_user_based(lic)) 
    {
        sprintf(errtxt, 
                "ERROR: User-based licenses (%s) cannot be transfered\n",
                product);
        rlm_ix_log(ls, errtxt);
        return(-1);
    }
    else if (rlm_license_host_based(lic)) 
    {
        sprintf(errtxt, 
                "ERROR: Host-based licenses (%s) cannot be transfered\n",
                product);
        rlm_ix_log(ls, errtxt);
        return(-1);
    }
    else
    {
        return(0);
    }
}

/*
 *  fillin_auth_rlm - this is where the mapping from the other LM's
 *      license attributes to the RLM license attributes happens.
 */

static
    void
_fillin_auth_rlm(void *ls, void *lm_handle, void *license_handle, void *auth, 
        char *prod, int count)
{
    RLM_LICENSE lic = (RLM_LICENSE) license_handle;

    /*
     *  Call rlm_ix_update_auth() for each parameter you want to set.  
     *  If the parameter is an integer, put it's value in the 4th parameter.  
     *  If it's a string, put a pointer into the 5th parameter.
     *
     *  Use "ls", and "auth" as the first 2 parameters to these calls.
     */
    rlm_ix_update_auth(ls, auth, RLM_IX_PRODUCT, 0, 
            rlm_license_product(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_VER, 0, rlm_license_ver(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_EXP, 0, rlm_license_exp(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_ISSUED, 0, rlm_license_issued(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_HOLD, rlm_license_hold(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_MIN_CHECKOUT, 
            rlm_license_min_checkout(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_CONTRACT, 0, 
            rlm_license_contract(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_CUSTOMER, 0, 
            rlm_license_customer(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_ISSUER, 0, rlm_license_issuer(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_LIC_OPTS, 0, 
            rlm_license_options(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_PLATFORMS, 0, 
            rlm_license_platforms(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_TYPE, rlm_license_type(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_SOFT_LIMIT, 
            rlm_license_soft_limit(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_MAX_SHARE, 
            rlm_license_max_share(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_SHARE, rlm_license_share(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_START, 0, rlm_license_start(lic));
    rlm_ix_update_auth(ls, auth, RLM_IX_MIN_TIMEOUT, 
            rlm_license_min_timeout(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_COUNT, count, "");
    rlm_ix_update_auth(ls, auth, RLM_IX_TIMEZONES, rlm_license_tz(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_MIN_REMOVE, 
            rlm_license_min_remove(lic), "");
    rlm_ix_update_auth(ls, auth, RLM_IX_HOSTID_STRING, 0, 
            rlm_license_hostid(lic));
}

/*
 *  Update the transfer status if we didn't get an RLM license, or
 *  if we lose it.
 */

static
    int
_update_status_rlm(void *ls, char *server, int status, void *lm_handle, 
        int *passes_to_check)
{
    char string[RLM_ERRSTRING_MAX];
    int unavailable = 0;
    RLM_HANDLE rh = (RLM_HANDLE) lm_handle;

    switch (status)
    {
        case RLM_EL_COMM_ERROR:
        case RLM_EL_SERVER_DOWN:
            sprintf(string, "  license server (%s) down (%d)\n", 
                    server, status);
            rlm_ix_log(ls, string);
            *passes_to_check = 10;
            break;

        case RLM_EL_ALLINUSE:
            sprintf(string, "  Licenses all in use on server %s (%d)\n",
                    server, status);
            rlm_ix_log(ls, string);
            *passes_to_check = 2;
            break;

        case RLM_EL_NOPRODUCT:
        case RLM_EL_TOOMANY:
        case RLM_EL_EXPIRED:
        case RLM_EL_NOTTHISHOST:
        case RLM_EL_ON_EXC_ALL:
        case RLM_EL_ON_EXC:
        case RLM_EL_NOT_INC_ALL:
        case RLM_EL_NOT_INC:
        case RLM_EL_OVER_MAX:
        case RLM_EL_SERVER_BADRESP:
        case RLM_EL_WRONG_PLATFORM:
        case RLM_EL_WRONG_TZ:
        case RLM_EL_NOT_STARTED:
        case RLM_EL_OVERSOFT:
            sprintf(string, "  Licenses unavailable from server %s (%d)\n",
                    server, status);
            rlm_ix_log(ls, string);
            unavailable = 1;
            break;

        default:
            sprintf(string, "%s\n", rlm_errstring(NULL, rh, string));
            rlm_ix_log(ls, string);
            unavailable = 1;
            break;
    }
    return(unavailable);
}

static
    int
_open_rlm(void *ls, char *lmname, int port, char *host, int port2, char *host2,
        void **lm_handle, int *unavailable)
{
    int status = 0;
    char portathost[200];

    /*
     *  host2/port2 not used for RLM
     */
    sprintf(portathost, "%d@%s", port, host);
    *lm_handle = (void *) rlm_init(portathost, "", "");

    status = rlm_stat((RLM_HANDLE) *lm_handle);
    if (status)
    {
        char string[RLM_ERRSTRING_MAX];

        rlm_ix_log(ls, "License transfer: error initializing RLM\n");
        rlm_ix_log(ls, rlm_errstring((RLM_LICENSE) NULL, 
                    (RLM_HANDLE) *lm_handle, string));
    }
    else
    {
        /*
         *      We never want to use broadcasts to find the license
         *      server.  Turn that off now. (RLM v10.0 and later only).
         */
        rlm_isv_cfg_disable_broadcast((RLM_HANDLE) *lm_handle, 1);
        /*
         *      Note - this username will appear in the source server's
         *      log files as the user who did the checkout for the transfer.
         */
        rlm_set_environ((RLM_HANDLE) *lm_handle, "RLM-EXT-Transfer", 
                (char *) NULL, (char *) NULL);
    }
    return(status);
}

/*
 *  _checkout_rlm() - check out the specified license
 *
 *  product/ver/count - the license to check out
 *  lm_handle - the license manager's handle (RLM_HANDLE cast to void *)
 *  license_handle - this function returns the license handle
 */ 

static
    int
_checkout_rlm(void *ls, void *lm_handle, char *prod, char *ver, int count, 
        void **license_handle)
{
    int status = 0;
    RLM_LICENSE *lic = (RLM_LICENSE *)license_handle;

    *lic = rlm_checkout((RLM_HANDLE) lm_handle, prod, ver, count);
    status = rlm_license_stat(*lic);
    return(status);
}

/*
 *  hb_rlm() - perform the license check heartbeat.
 *
 *  lm_handle - the license manager handle (unused in RLM)
 *  license_handle - the RLM license handle (cast to void *)
 */

static
    int
_hb_rlm(void *lm_handle, void *license_handle)
{
    return(rlm_get_attr_health((RLM_LICENSE) license_handle));
}

/*
 *  _checkin_rlm() - check the license in, and close the license handle
 */

static
    int
_checkin_rlm(void *lm_handle, void *license_handle)
{
    rlm_checkin((RLM_LICENSE) license_handle);
    return(0);
}

/*
 *  _close_rlm() - close the license handle
 */

static
    int
_close_rlm(void *lm_handle)
{
    rlm_close((RLM_HANDLE) lm_handle);
    return(0);
}

/*
 *  rlm_ix_setup() - sets up for an external license transfer.
 *
 *  Do not change the name of this function.  The ISV server calls
 *  this function at initialization time to register your functions
 *  for an external license transfer.  There is an rlm_ix_setup() stub
 *  in the RLM library, so when you compile this module, you MUST 
 *  include this object file before the rlm library in your link
 *  command.
 */

    void
rlm_ix_setup(void *ls)
{
    /*
     *  These next 3 lines will be logged in the license server before
     *  any of the license file and product support logging.
     */
    rlm_ix_log(ls, "**********************************************\n");
    rlm_ix_log(ls, "Setting up (ISV-defined) RLM license transfers\n");
    rlm_ix_log(ls, "**********************************************\n");

    /*
     *  This call registers all the callbacks to do the license
     *  transfers for our ISV-defined transfer module. 
     *
     *  The 2nd and 3rd parameters are here to cause an optional
     *  RLM module that handles the license transfer extensions
     *  in this file to be linked into your ISV server - don't 
     *  change them.
     *
     *  All the other entry points are written by you, and will
     *  generally be in this file.
     *
     *  Link this object file with your ISV server, before the RLM library
     *  in the link command.
     */
    rlm_ix_enable(  "RLM-external",     /* License Manager name */
            rlm_ix_transfer,    /* DON'T CHANGE THIS */
            rlm_ix_done_xfer,   /* DON'T CHANGE THIS */
            _open_rlm,      /* Your handle open fcn */
            _checkout_rlm,      /* Your checkout fcn */
            _fillin_auth_rlm,   /* Your auth fillin fcn */
            _check_auth_rlm,    /* Your license check fcn */
            _update_status_rlm, /* Your status update fcn */
            _hb_rlm,        /* Your heartbeat fcn */
            _checkin_rlm,       /* Your checkin fcn */
            _close_rlm);        /* Your handle close fcn */
}
