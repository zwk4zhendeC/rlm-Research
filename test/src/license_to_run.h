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
 *  Description:    License to use RLM
 *
 *  Replace the RLM license on the four lines after:
 *
 *      #define RLM_LICENSE_TO_RUN  \
 *
 *  with the license you received from Reprise Software.
 *
 *  M. Christiano
 *  4/20/07
 *
 */

#ifdef RLM_LICENSE_TO_RUN
#undef RLM_LICENSE_TO_RUN
#endif

/* ISV:  demo */

#define RLM_LICENSE_TO_RUN \
  "16-may-2025 \
  sig=\"c2N253QKHgBQvcMI4vykjVNb6njhoyvZ0igA~cIie0hPxfBzwKhGDba+4peatN\""
#define RLM_ISV_NAME "demo"

