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
  "15-apr-2026 \
  sig=\"c2N252Zm+P9eB*Q1GD4uiIu2EHET4KRD0igtz4xmodd9+WQymBxrPgFV4JD5hN\""
 
#define RLM_ISV_NAME "demo"
 
  /* License Key Checksum: 00006e  */
  /*  End of License keys for RLM v17 for ISV demo  */

