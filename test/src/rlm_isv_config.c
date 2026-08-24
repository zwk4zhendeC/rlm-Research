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
 *  Description:    rlm_isv_config.c - configuration data for ISV 
 *
 *  M. Christiano
 *  11/25/05
 *
 */

#include "license.h"
#include "license_to_run.h"

/*
 *  Define "INCLUDE_RLMID1" to include support for RLMID1 dongles.
 *  Comment out to remove aladdin dongle support.
 *
 *  Note: The RLMID1 dongle code is always included in 
 *  your license server.  This setting is only for your applications, and 
 *  only needs to be set if you are issuing licenses that are nodelocked 
 *  to a dongle.
 *
 *  Including the RLMID1 dongle code increases the size of
 *  your applications by approx 900Kb on 32-bit windows, plus involves
 *  a small delay at application startup time, even if you are not using
 *  a dongle.
 *
 *  If you are not planning to issue licenses which are node-locked to
 *  rlmid devices, Reprise Software recommends leaving these options turned
 *  off (ie, leave the "#if 0" on the next line).
 */

#if 0
#define INCLUDE_RLMID1
#endif

#ifdef INCLUDE_RLMID1
extern void _rlm_gethostid_type1(RLM_HANDLE, L_HOSTID);
#endif

    void
rlm_isv_config(RLM_HANDLE handle)
{

    /* 
     *  Set ISV name 
     *
     *  NOTE: IF you are evaluating RLM, DO NOT change the ISV
     *        name, or your license keys will no longer work.
     *        For eval kits, the name on the next line MUST
     *        be "demo".
     *
     *  NOTE: Your ISV name is, in general, case-insensitive.
     *        The ONLY exception to this is when it is used as
     *        a lockfile name using a FLEXlm-compatible lockfile.
     *        In this case (and this case only), the case of the
     *        name you enter here is important.  Note that even in
     *        this case, ONLY THE LOCKFILE NAME uses the exact case
     *        you enter - every other place in RLM uses a lowercase
     *        version of this name.
     *
     *  Beginning in RLM v7.0, your ISV name is contained in
     *  "license_to_run.h".  If you need to alter the case of the
     *  name for a compatible FLEXlm lockfile, you should do it there
     *  and leave the next line as it is.
     */
    rlm_isv_cfg_set_name(handle, RLM_ISV_NAME);

    /* 
     *  Set RLM license - do not modify this line 
     */
    rlm_isv_cfg_set_license(handle, RLM_LICENSE_TO_RUN);

    /* 
     *  Set oldest allowed server version.
     *
     *  The next setting controls the oldest RLM license server
     *  version with which your application will work.
     *
     *  The 3 parameters are rlm version, revision, and build (in
     *  that order).
     *
     *  If you leave this set to 0, 0, 0 your application will 
     *  attempt to work with the oldest available RLM server.
     *
     *  You should only set this if you are concerned with an older
     *  server in the field which has been hacked, otherwise, you should
     *  leave it set to 0, 0, 0.
     *
     *      (Note: Do not set this to anything between 0,0,0, and
     *       9,0,0).  Servers older than v9.0 will appear to be v0.0)
     *
     */
    rlm_isv_cfg_set_oldest_server(handle, 0, 0, 0);

    /* 
     *  Set ISV server settings file compatibility 
     *
     *  The next setting controls what versions of RLM your
     *  ISV server settings file will work with.   You can enable
     *  it for all earlier versions (> v6), or later versions or both.
     *  The 2nd parameter enables earlier versions if non-zero, the
     *  3rd parameter enables later versions if non-zero.  Note that
     *  "earlier" and "later" are relative to the version of your
     *  settings file.  So, if you create the settings file with RLM v8,
     *  "earlier" means v6 and v7, while "later" means v9 and above.
     *
     *  default is: rlm_isv_cfg_set_compat(handle, 0, 1); - sets compatibility
     *          with later versions, but not earlier ones.
     */
    rlm_isv_cfg_set_compat(handle, 0, 1);
    /* 
     *  
     *  Setup virtual machine enable/disable.
     *
     *  By default (if you do not modify the following call), RLM
     *  will refuse to run a license server on a virtual machine.  
     *
     *      You can always enable a particular virtual machine by issuing
     *      an "rlm_server_enable_vm" license for that machine.
     *
     *  If you want license servers to run on all virtual machines, set
     *  the 2nd parameter of the next call to a non-zero value.
     *
     */
    rlm_isv_cfg_set_enable_vm(handle, 0);

    /*
     *      Beginning in RLM v10.0, roaming is disabled for servers that
     *      use transient hostids (ie, dongles, or ISV-defined transient hostids).
     *      If you want to enable roaming on these servers, set the 2nd
     *      parameter of the next call to 1.
     */
    rlm_isv_cfg_set_enable_roam_transient(handle, 0);

    /*
     *      Beginning in RLM v10.0,  you have the option of turning ROAMED
     *      licenses into "single" licenses.  Prior to RLM v10.0, all ROAMED
     *      licenses were nodelocked, uncounted.
     *      If you want your roamed licenses to be "single" licenses, set the
     *      second parameter of the next call to 1.
     */
    rlm_isv_cfg_set_roam_single(handle, 0);

    /*
     *  Beginning in RLM v10.0 it is possible to disable the clock windback
     *      check.  In previous versions it was always enabled.  Passing a 1 in
     *      the second argument of the following function call disables the 
     *  windback check; passing 0 leaves it enabled (the default).
     */
    rlm_isv_cfg_disable_clock_windback_check(handle, 0);

    /* 
     *  FLEXlm(R)-style lockfile compatibility.
     * 
     *  Set to non-zero to use a FLEXlm-style lockfile.  For windows
     *  systems, a value of 1 uses the 'C' drive always, whereas a
     *  value > 1 will use the system drive.  FLEXlm (up to version
     *  10.3, at least) puts the lockfile on the 'C' drive.
     *
     *  Reprise Software recommends setting this to 1 if you want to
     *  use FLEXlm-compatible lockfiles.
     */
    rlm_isv_cfg_set_use_flexlm_lockfile(handle, 0);

    /*  
     *  The Windows disk serial number hostid code can return hostids
     *  that are usable only by processes running with admin rights if
     *  running with admin privileges.  If an application is installed
     *  and a license activated by an admin user, it's possible that
     *  a non-admin user will not be able to use the application because
     *  it can't read the disk serial number.  Beginning in RLM v10.0,
     *  you can disable the use of disk serial number hostids that are 
     *  usable by admins only.  If you want to do so, change the second 
     *  parameter of the next function to 0.
     */
#ifdef _WIN32
    rlm_isv_cfg_set_use_admin_disksns(handle, 1);
#endif

    /* 
     *  Beginning in RLM v10.0,  RLM's license transfer capability also
     *  allows for disconnected operation on the destination server.
     *  This capability only requires that an "rlm_roam" license be
     *  present on the destination server.  You can ship an rlm_roam
     *  license to your customer and have them install it on every
     *  destination server, or you can simply put it into the next
     *  call, in which case, no separate license file will be needed
     *  on the destination license server.
     *  
     *  To enable this, set the 2nd parameter of the next call to a valid,
     *  signed rlm_roam license (enclosed in "<>") in place of the 
     *  last argument.  This license should be a static string 
     *  which is available for the lifetime of the server.
     *
     *      This license MUST have the following parameters:
     *              version: "1.0"
     *              exp: "permanent"
     *              count: "uncounted"
     *              hostid: "any"
     *              NO other parameters
     *
     *  for example:
     *
     *  rlm_isv_cfg_set_server_roam(handle, "<LICENSE your-isvname rlm_roam 1.0 uncounted hostid=any sig=xxxxxxx>"); 
     */
    rlm_isv_cfg_set_server_roam(handle, (char *) 0);

    /*
     *      Beginning in RLM v10.0,  RLM can broadcast to find a license
     *      server as a last resort, if all the normal methods to find
     *      the server fail.  This capability is enabled by default.
     *
     *      To disable this, set the 2nd parameter of the next call to 1.
     */
    rlm_isv_cfg_disable_broadcast(handle, 0);

    /* 
     *  Beginning in RLM v11.0, the client can specify that
     *  it will not use a generic license server (i.e., rlm + a 
     *  settings file).
     *  If you want to disable generic servers, set the 2nd
     *  parameter of the next call to 1.
     *  If you disable generic servers and your application
     *  attempts to connect to a generic server, it will
     *  receive an RLM_EH_SERVER_REJECT error upon connection
     *  or an RLM_EL_SERVER_REJECT upon license checkout.
     *  The text error message is "Server rejected client".
     *
     *  Pre-v11 clients will get a "Communications error with
     *  license server (-17), Connection refused at server (-111)"
     *  error in this case.
     *
     *  Reprise Software does not recommend disabling the generic server.
     *
     */
    rlm_isv_cfg_disable_generic_server(handle, 0);

    /*
     *  Beginning in RLM v10.1, licenses can be cached on the client
     *  side with the use of the "client_cache" license attribute.
     *  This capability must be enabled with the following call.
     *  If the 2nd parameter is 1, client caching is enabled, if 0,
     *  caching is disabled.
     *      Note: this call has no effect on HP systems.
     */
    rlm_isv_cfg_enable_client_cache(handle, 1);

    /*
     *  Beginning in RLM v10.1, license servers can return one
     *  valid license to the application which is then verified on
     *  the client side.  This check helps ensure that the license
     *  server hasn't been modified.  To enable this checking set
     *  the second parameter of the next call to 1.  If you enable
     *  this, please read the section titled "Server Integrity Checking"
     *  in the "Securing Your Application" section of the Reference
     *  Manual so that you understand the errors which can be generated
     *  as a result of this call and how you should proceed.
     */
    rlm_isv_cfg_enable_check_license(handle, 0);

    /*
     *      Beginning in RLM v11.0, you can specify which types of
     *      hostids that Activation Pro will accept from an activation
     *      request.   Prior to v11.0, the only 6 types of acceptable
     *      hostids were: rehostable, isv-defined, rlmid, ethernet,
     *      disk serial numbers and native 32-bit hostids.
     *      In the following call, you can set the default hostids that
     *      your Actpro server will accept.  To get the pre-v11 behavior,
     *      (plus alternate server hostids) set the 2nd parameter as shown.  
     *  Hostid type definitions are in license.h
     *
     *  Note:   RLM_ACTPRO_ALLOW_ISV is the old ISV-defined hostid.
     *      RLM_ACTPRO_ALLOW_ISVDEF is the isv-defined string as a hostid.
     *      RLM_ACTPRO_ALLOW_ASH allows both Alternate Server Hostids as
     *          well as Alternate Nodelock Hostids.
     *
     *  The default for any RLM version going forward is defined
     *  in license.h/RLM_ACTPRO_ALLOW_DEFAULT
     *
     */
#if 0
    {
        int allowed_types =   RLM_ACTPRO_ALLOW_REHOST | RLM_ACTPRO_ALLOW_ISV |
            RLM_ACTPRO_ALLOW_ISVDEF | RLM_ACTPRO_ALLOW_RLMID | 
            RLM_ACTPRO_ALLOW_ETHER | RLM_ACTPRO_ALLOW_DISKSN | 
            RLM_ACTPRO_ALLOW_32 | RLM_ACTPRO_ALLOW_UUID |
            RLM_ACTPRO_ALLOW_ASH;
        rlm_isv_cfg_actpro_allowed_hostids(handle, allowed_types);
    }
#endif

    /*
     *      Beginning in RLM v11.2, license servers can utilize
     *      Alternate Server Hostids.  These hostids are activated
     *      from Activation Pro by the ISV server, which needs to
     *      know the URL of the activation server.
     *      If you use Reprise's hosted activation service, the default
     *      (hostedactivation.com) is correct.  For all others, set your
     *      activation server url here.  Note that this URL pointer must
     *  remain valid as long as the RLM_HANDLE is in use.
     */
    /*** rlm_isv_cfg_set_url(handle, "hostedactivation.com"); ***/

    /*
     *  Rehostable hostids do two checks at verification time which 
     *  fail on certain systems.   These checks are:
     *     - checking the file ID of each file in the rehostable hierarchy, and
     *     - checking the native hostid of the system
     *
     *  The file ID check fails on Windows systems if drives are added or
     *  removed from the controller.
     *  We have seen the native hostid change on Centos systems when the
     *  network cable is unplugged.
     *
     *  Beginning in RLM v12.3, you can disable one or both of these
     *  checks by setting the second parameter of the two following
     *  calls to 1.  The default behavior remains the same as in
     *  previous versions of RLM.  If you use a server settings file and
     *  you disable either of these checks, you must re-generate the
     *      settings file with your v12.3 kit.

*/
    rlm_isv_cfg_disable_windows_fileid_check(handle, 0);    
    /* 0 -> check, <>0 -> no check */
    rlm_isv_cfg_disable_reference_hostid_check(handle, 0);  
    /* 0 -> check, <>0 -> no check */
    /*
     *      Roam extension is a new feature in RLM v12.3, and it is disabled
     *      by default.  If you enable it, be aware that the max_roam setting
     *      from your rlm_roam license will NOT be honored for a roam extension,
     *      only the max_roam setting of the license which is roaming.  This means
     *      that if you use max_roam on the rlm_roam license to limit roaming
     *      duration on your licenses, it will not be effective for any roam
     *      extension.  The default max_roam on any license is 30 days, so this
     *      may or may not be an issue for you.
     *
     *      To enable roam extensions, set the 2nd parameter of the next call
     *      to 1.  If you use a server settings file, you must re-generate the
     *      settings file with your v12.3 kit, otherwise, the roam extension will
     *      not appear in the RLM web interface.
     */
    rlm_isv_cfg_enable_roam_extend(handle, 0);

    /*
     *  New in v12.4, the RLM web services API (used with RLMCloud) has an 
     *  isv-defined server handshake function, and also
     *  new in v14.0, the https transport optionally used for RLMCloud
     *  server connumications uses the same handshake function.  
     *
     *  To use this, specify the 2 parameters to the server-side of the 
     *  algorighm here.  P1 and P2 are any 32-bit non-zero positive numbers, 
     *  ie, > 0 and <= 0x7fffffff  Avoid long sequences of 0's or 1's in P1.
     *
     *  This handshake algorithm is separate from the normal RLM client-
     *  server handshake, and these parameters will not affect non-web-
     *  services handshakes.
     *
     *  These values are encoded in your settings file (<isv>.set)
     *
     *  NOTE: CHANGE THE DEFINTIONS of P1/P2 that appear here.
     */
#define P1 0x39a74d25
#define P2 0xa9d75
    rlm_isv_cfg_set_isv_handshake(handle, P1, P2);

    /*
     *  Prior to RLM v12.4, if you enabled the check for server licenses
     *  by calling rlm_isv_cfg_enable_check_license(handle, 1), connections
     *  to the license server would fail if the license either contained new
     *  keywords or was invalid.  In v12.4 and later, you can cause the
     *  connection to succeed and retrieve the status later.  To do this,
     *  set the 2nd parameter of the next call to a non-zero value.   After
     *  connecting, you can call rlm_get_attr_lic_check() on the handle.
     *  rlm_get_attr_lic_check will return 0 for success or either
     *  RLM_NO_SERVER_LIC, RLM_LIC_NEW_KEYWORDS or RLM_LIC_BAD. 
     */
    rlm_isv_cfg_no_server_license_fail(handle, 0);  
    /* 0 -> check, <>0 -> no check */
    /*
     *  In RLM v14.0, transport to the license server can be over HTTPS.
     *  If you are enabling this capability (available on RLMCloud only),
     *  you need to set the "promise" value that is used on the RLMCloud
     *  helper which connects to the license server.  You can make this
     *  call here, and anywhere in your application.  The default is
     *  set to 10 minutes here, which means you need to send heartbeats
     *  to the server every 5-7 minutes to avoid the license being
     *  reclaimed automatically.  The promise value is in minutes.
     */
    rlm_isv_cfg_set_http_promise(handle, 10);
    /*
     *      Beginning in RLM v14.0, you can disable certain RLM hostid types.
     *      In the following call, you can set the hostids that your product
     *      (or license server) will NOT accept.  If the 2nd parameter of this
     *      call is 0, all RLM hostids are allowed.
     *
     *      OR the values of the hostids you want to disable into a single
     *      integer and pass this as the 2nd argument. For example:
     *
     *              int disable = RLM_DISABLE_H_USER | RLM_DISABLE_H_HOST;
     *                      rlm_isv_cfg_disable_hostids(handle, disable);
     *
     *  The bitmasks for Hostid Types that can be disabled are (in license.h also):
     *              RLM_DISABLE_H_32BIT   RLM_DISABLE_H_STRING   RLM_DISABLE_H_ETHER
     *              RLM_DISABLE_H_USER    RLM_DISABLE_H_HOST     RLM_DISABLE_H_IP
     *              RLM_DISABLE_H_ANY     RLM_DISABLE_H_DEMO     RLM_DISABLE_H_SN
     *              RLM_DISABLE_H_RLMID1  RLM_DISABLE_H_RLMID2   RLM_DISABLE_H_GC
     *              RLM_DISABLE_H_DISKSN  RLM_DISABLE_H_IPV6     RLM_DISABLE_H_UUID
     */
    {
        int disable = 0;      /* Disable nothing, ie, pre-v14 behavior */
        rlm_isv_cfg_disable_hostids(handle, disable);
    }

    /*
     *  Beginning in RLM v14.1, when you change your ISV name, you can
     *  use this call to tell RLM about the old name, so that your
     *  ISV servers will lock out both new and old ISV servers.
     *  Don't call this unless you are transitioning ISV names.
     *  In v14.2, 2 old ISV names can be specified.
     */
#if 0
    rlm_isv_cfg_set_old_isv_name(handle, "old_name");
    rlm_isv_cfg_set_old_isv_name2(handle, "old_name2");
#endif

    /*
     *  Beginning in RLM v14.1, for ASH licenses, you have the 
     *  option of allowing a server startup if contact with the
     *  activation server has been successful within the
     *  # of minutes specified below.  Default is 0, ie the server
     *  must be able to contact the activation server for "daily"
     *  and "6hour" check types.
     *  If you want to use this capability, a good value would be
     *  one or 2 days, ie, 60*24 or 2*60*24.
     */
#define DAY 60*24
    rlm_isv_cfg_set_ash_start_tolerance(handle, DAY*2);

    /*
     *  If you want to add ISV-defined hostids to the ISV server,
     *  use code similar to the following for each new hostid type
     *  you would like to add.
     *
     *  Note: beginning in RLM v11.3, Reprise Software recommends
     *  using the isv-defined string as a hostid rather than integrating
     *  ISV-defined hostid code into your application and license server.
     *  This preserves your ability to use the ISV server settings file
     *  and the Activaiton Pro license generator settings file rather than
     *  building custom binaries for these two programs.  You should note,
     *  however, that the isv-defined string when used as a hostid only
     *  works on the client-side, ie, only for nodelocked licenses.  It
     *  does not work for the license server's hostid.
     */
#if 0
    stat = rlm_add_isv_hostid
        (
         handle,     /* RLM_HANDLE passed in */
         "keyword here", /* Hostid keyword you chose */
         YOU_DEFINE_HOSTID_TYPE, /* Your hostid type (int) 
                                    > RLM_ISV_HID_TYPE_MIN */
         transient,  /* (int) == 0 if hostid does not change.
                        Non-zero if it does change, e.g., if
                        your hostid is a dongle, it can
                        change if someone unplugs it, so
                        you should set transient non-zero */
         get_type_hostid /* Your function to determine the
                            hostid value */
        );
    if (stat)
    {
        printf("ERROR: add hostid type returns %d\n", stat);
    }
#endif

    /* 
     *  To include RLMID1 dongle code, be sure INCLUDE_RLMID1 is defined above.
     */

#ifdef INCLUDE_RLMID1
    rlm_isv_cfg_set_use_hostid(handle, RLM_HOSTID_RLMID1, 
            _rlm_gethostid_type1, 0);
#endif
}
