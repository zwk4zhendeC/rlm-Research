/******************************************************************************

  COPYRIGHT (c) 2007, 2022 by Reprise Software, Inc.
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
 *  Description:    RLM admin API external definitions
 *
 *  M. Christiano
 *  4/25/07
 *
 */

#ifndef RLM_ADMIN_H_INCLUDED
#define RLM_ADMIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

    /*
     *  rlm_admin_xxx(), rlm_userlist_xxx() functions
     */

    typedef struct license_info *RLM_ADMIN_HANDLE;
    typedef struct l_userlist *RLM_USERLIST;
    typedef struct l_rlm_info *RLM_RLMINFO;
    typedef struct l_isv_pool *RLM_ISV_POOL;

    extern RLM_ADMIN_HANDLE rlm_admin_first_server(RLM_HANDLE);
    extern RLM_ADMIN_HANDLE rlm_admin_next_server(RLM_HANDLE, RLM_ADMIN_HANDLE);
    extern RLM_RLMINFO  rlm_admin_rlminfo_read(RLM_HANDLE, RLM_ADMIN_HANDLE);
    extern char *       rlm_admin_first_isv(RLM_RLMINFO, int *instance);
    extern char *       rlm_admin_next_isv(RLM_RLMINFO, int *instance);
    extern int      rlm_admin_connect_isv(RLM_HANDLE, RLM_ADMIN_HANDLE, 
            char *isv, int instance);
    extern void     rlm_admin_free_rlminfo(RLM_RLMINFO);
    extern char *       rlm_admin_hostname(RLM_ADMIN_HANDLE);
    extern RLM_ISV_POOL rlm_admin_poolinfo(RLM_HANDLE, RLM_ADMIN_HANDLE,
            char *prod, char *isv);
    extern int      rlm_admin_port(RLM_ADMIN_HANDLE);

    extern RLM_ISV_POOL rlm_poolinfo_first(RLM_ISV_POOL p);
    extern RLM_ISV_POOL rlm_poolinfo_next(RLM_ISV_POOL p);
    extern void     rlm_poolinfo_free(RLM_ISV_POOL p);
    extern int      rlm_poolinfo_poolnum(RLM_ISV_POOL p);
    extern int      rlm_poolinfo_share(RLM_ISV_POOL p);
    extern char *       rlm_poolinfo_contract(RLM_ISV_POOL p);
    extern char *       rlm_poolinfo_customer(RLM_ISV_POOL p);
    extern char *       rlm_poolinfo_hostid(RLM_ISV_POOL p);
    extern char *       rlm_poolinfo_issuer(RLM_ISV_POOL p);
    extern char *       rlm_poolinfo_options(RLM_ISV_POOL p);
    extern char *       rlm_poolinfo_platforms(RLM_ISV_POOL p);
    extern char *       rlm_poolinfo_start(RLM_ISV_POOL p);

    extern RLM_USERLIST rlm_userlist_read(RLM_HANDLE, RLM_ADMIN_HANDLE h, 
            char *product, char *user, char *host);
    extern RLM_USERLIST rlm_userlist_first(RLM_USERLIST u);
    extern RLM_USERLIST rlm_userlist_next(RLM_USERLIST u);
    extern void     rlm_userlist_free(RLM_USERLIST u);

#include <time.h>

    extern char *       rlm_userlist_auth_ver(RLM_USERLIST);
    extern int      rlm_userlist_client_cache(RLM_USERLIST);
    extern int      rlm_userlist_current_inuse(RLM_USERLIST);
    extern int      rlm_userlist_current_resuse(RLM_USERLIST);
    extern time_t       rlm_userlist_hold(RLM_USERLIST);
    extern char *       rlm_userlist_host(RLM_USERLIST);
    extern char *       rlm_userlist_isv_def(RLM_USERLIST);
    extern time_t       rlm_userlist_min_checkout(RLM_USERLIST);
    extern time_t       rlm_userlist_out_time(RLM_USERLIST);
    extern char *       rlm_userlist_product(RLM_USERLIST);
    extern int      rlm_userlist_queue(RLM_USERLIST);
    extern char *       rlm_userlist_req_prod(RLM_USERLIST);
    extern int      rlm_userlist_server_pool(RLM_USERLIST);
    extern int      rlm_userlist_server_handle(RLM_USERLIST);
    extern char *       rlm_userlist_user(RLM_USERLIST);
    extern char *       rlm_userlist_ver(RLM_USERLIST);

#ifdef __cplusplus
}
#endif

#endif /* RLM_ADMIN_H_INCLUDED */
