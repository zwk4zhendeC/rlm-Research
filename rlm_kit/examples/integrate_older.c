/******************************************************************************

  COPYRIGHT (c) 2006, 2022 by Reprise Software, Inc.
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
 *  Description:    Example of RLM integration with an older license mgr
 *
 *      This example illustrates how to integrate RLM in code
 *      that uses a different license manager.  The idea is that
 *      both license managers can be used, at the ISV's choice for
 *      a particular customer.
 *
 *      The strategy is:
 *
 *          1. request the RLM license.  
 *          2. If the request succeeds or results in a license in 
 *             the queue, assume RLM licenses are in use, and
 *             proceed with the application.  If the license is
 *             queued, there should be some indication to the user.
 *          3. All other errors are divided into errors that
 *             indicate RLM is being used, and those that indicate
 *             that the older license manager is in use.
 *             In the first case, report the RLM error, in the
 *             second case, no RLM error should be displayed; 
 *             instead revert to the old behavior.
 *          4. If using RLM licenses, call the appropriate
 *             RLM functions (checking status, checkin, etc) where
 *             the calls are placed to the older license manager.
 *             (Note: item #4 is not illustrated in this source
 *              file).
 *
 *  M. Christiano
 *
 *  $Id: integrate_older.c,v 1.26 2022/03/08 17:06:27 matt Exp $
 */

#include "license.h"
#include <stdio.h>

    int
main(int argc, char *argv[])
{
    RLM_HANDLE rh;
    RLM_LICENSE lic;
    char *product = "test1";
    int stat = 0, count = 1;
    char *ver = "1.0";
    char errstring[RLM_ERRSTRING_MAX];

    rh = rlm_init(".", argv[0], (char *) NULL);
    if ((stat = rlm_stat(rh)))
    {
        /*
         *      Since one error from rlm_init() indicates that there
         *      is no RLM license file or RLM_LICENSE environment
         *      variable, this is an indication that your customer
         *      is using the older license manager.
         */
        (void) printf(
                "No RLM license - old LM checkout code goes here\n");
        (void) printf("RLM initialization error %d\n", stat);
        (void) printf("%s\n", rlm_errstring((RLM_LICENSE) NULL, rh, 
                    errstring));
        return(1);
    }

    if (stat == 0)
    {
        lic = rlm_checkout(rh, product, ver, count);
        stat = rlm_license_stat(lic);
    }
    switch (stat)
    {
        case 0:
            (void) printf("RLM license acquired\n");
            break;

        case RLM_EL_INQUEUE:
            (void) printf("In queue for license from RLM\n");
            break;

            /*
             *      These errors indicate that RLM licenses are in use,
             *      and the result should be reported to the user.
             */
        case RLM_EL_ALLINUSE:
        case RLM_EL_APP_INACTIVE:
        case RLM_EL_BAD_PASSWORD:
        case RLM_EL_CANTRECONNECT:
        case RLM_EL_DEMOEXP:
        case RLM_EL_FAILED_BACK_UP:
        case RLM_EL_METER_NO_SERVER:
        case RLM_EL_METER_NOCOUNT:
        case RLM_EL_NONE_CANROAM:
        case RLM_EL_NOROAM_FAILOVER:
        case RLM_EL_NOROAM_TRANSIENT:
        case RLM_EL_NOT_INC_ALL:
        case RLM_EL_NOT_INC:
        case RLM_EL_NOT_INC_ROAM:
        case RLM_EL_NOT_NAMED_USER:
        case RLM_EL_NOTTHISHOST:
        case RLM_EL_ON_EXC_ALL:
        case RLM_EL_ON_EXC:
        case RLM_EL_ON_EXC_ROAM:
        case RLM_EL_OVER_MAX:
        case RLM_EL_OVERSOFT:
        case RLM_EL_PORTABLE_REMOVED:
        case RLM_EL_ROAM_TOOLONG:
        case RLM_EL_ROAMFILEERR:
        case RLM_EL_RLM_ROAM_ERR:
        case RLM_EL_SERVER_LOST_XFER:
        case RLM_EL_SERVER_TOO_OLD:
        case RLM_EL_TOOMANY:
        case RLM_EL_TOOMANY_ROAMING:
        case RLM_EL_TS_DISABLED:
        case RLM_EL_VM_DISABLED:
        case RLM_EL_WILL_EXPIRE:
        case RLM_EL_WRONG_PLATFORM:
        case RLM_EL_WRONG_TZ:

            (void) printf(
                    "RLM licenses in use but cannot acquire license.\n");
            (void) printf("%s\n", rlm_errstring(lic, rh, errstring));
            break;

            /*
             *      These errors indicate that the older license manager
             *      should be used.
             */
        default:
        case RLM_EL_BADHOST:
        case RLM_EL_NOPRODUCT:
        case RLM_EL_NOTME:
        case RLM_EL_EXPIRED:
        case RLM_EL_BADKEY:
        case RLM_EL_BADVER:
        case RLM_EL_BADDATE:
        case RLM_EL_NOAUTH:
        case RLM_EL_REMOVED:
        case RLM_EL_SERVER_BADRESP:
        case RLM_EL_COMM_ERROR:
        case RLM_EL_NO_SERV_SUPP:
        case RLM_EL_NOHANDLE:
        case RLM_EL_SERVER_DOWN:
        case RLM_EL_NO_HEARTBEAT:
        case RLM_EL_NOHOSTID:
        case RLM_EL_TIMEDOUT:
        case RLM_EL_SYNTAX:
        case RLM_EL_NO_SERV_HANDLE:
        case RLM_EL_NOT_STARTED:
        case RLM_EL_CANT_GET_DATE:
        case RLM_EL_WINDBACK:
        case RLM_EL_BADPARAM:
            /*
             *      RLM licenses not in use.
             */
            (void) printf(
                    "No RLM license - old LM checkout code goes here\n");
            (void) printf("RLM error %d\n", stat);
            (void) printf("%s\n", rlm_errstring(lic, rh, errstring));
            break;
    }
    return(0);
}
