C      *************************************************************
C      *  Fortran Interfaces to call RLM using ftn_rlm.c wrapper   *
C      *             (see also ftn_rlm_test.f)                     *
C      *  VERSION 1.0            RSH                  08-Dec-2011  *
C      *    Zentech International Ltd.   roger@zentech.co.uk       *
C      *                   (please use freely)                     *
C      *************************************************************
C ---------------------------------------------------------------------
C  Most of the RLM API routines can be called directly from these routines.
C  Exceptions are functions returning a character string - these are
C  called via the wrapper in ftn_rlm.c.
C  Tested on Windows with Intel Visual Fortran 11.1 + MVS 2005, 
C  and on Linux x64 with gcc
C ---------------------------------------------------------------------
!
!!!! NB: all character strings sent to C must be C strings, e.g.:
!        trim(string)//char(0)
!
	Module FTN_RLM_CALLS
!
      integer*4 MAXSTR,MAXDTLS,DEBUG
      DATA MAXSTR,MAXDTLS,DEBUG/512,64,0/
!      
! Set up interfaces to RLM C routines directly
!
      INTERFACE
! initialise RLM
      integer*4 function rlm_init(s1,s2,s3)
      !DEC$ ATTRIBUTES C :: rlm_init
      CHARACTER*(*) s1,s2,s3
      !DEC$ ATTRIBUTES REFERENCE::s1,s2,s3
      END
! rlm status
      integer*4 function rlm_stat(i1)
      !DEC$ ATTRIBUTES C :: rlm_stat
      integer*4  i1
      !DEC$ ATTRIBUTES VALUE::i1
      END
! checkout
      integer*4 function rlm_checkout(i1,s1,s2,i2)
      !DEC$ ATTRIBUTES C :: rlm_checkout
      integer*4 i1,i2
      CHARACTER*(*) s1,s2
      !DEC$ ATTRIBUTES REFERENCE::s1,s2
      !DEC$ ATTRIBUTES VALUE::i1,i2
      END
! license status
      integer*4 function rlm_license_stat(i1)
      !DEC$ ATTRIBUTES C :: rlm_license_stat
      integer*4 i1
      !DEC$ ATTRIBUTES VALUE::i1
      END
! error message
      subroutine rlm_errstring(i1,i2,s1)
      !DEC$ ATTRIBUTES C :: rlm_errstring
      CHARACTER*(*) s1
      integer*4  i1,i2
      !DEC$ ATTRIBUTES REFERENCE::s1
      !DEC$ ATTRIBUTES VALUE::i1,i2
      END
! checkin
      subroutine rlm_checkin(i1)
      !DEC$ ATTRIBUTES C :: rlm_checkin
      integer*4 i1
      !DEC$ ATTRIBUTES VALUE::i1
      END
! close RLM
      subroutine rlm_close(i1)
      !DEC$ ATTRIBUTES C :: rlm_close
      integer*4 i1
      !DEC$ ATTRIBUTES VALUE::i1
      END
! get attr health
      integer*4 function rlm_get_attr_health(i1)
      !DEC$ ATTRIBUTES C :: rlm_get_attr_health
      integer*4 i1
      !DEC$ ATTRIBUTES VALUE::i1
      END
! get days to expiry
      integer*4 function rlm_license_exp_days(i1)
      !DEC$ ATTRIBUTES C :: rlm_license_exp_days
      integer*4 i1
      !DEC$ ATTRIBUTES VALUE::i1
      END
! 
! Set up interfaces to RLM C routines via ftn_rlm.c wrapper
! set debug for RLM wrapper
      subroutine rlm_debug_c(i1)
      !DEC$ ATTRIBUTES C :: rlm_debug_c
      integer*4 i1
      !DEC$ ATTRIBUTES REFERENCE::i1
      END
! get RLM_ERRSTRING_MAX length
      integer*4 function rlm_maxstr_c()
      !DEC$ ATTRIBUTES C :: rlm_maxstr_c
      END
! get license details
      subroutine rlm_license_dtls_c(i1,i2,s1)
      !DEC$ ATTRIBUTES C :: rlm_license_dtls_c
      CHARACTER*(*) s1
      integer*4 i1,i2
      !DEC$ ATTRIBUTES REFERENCE::i2,s1
      !DEC$ ATTRIBUTES VALUE::i1
      END
!
      END INTERFACE
!
      CONTAINS
!
!     Fortran routines to be called for c wrapper routines
!
! set debug for RLM
      subroutine f_rlm_debug(n)
!     n = debug level to set:
!         1 debug fortran
!         2   -      -    & c wrapper
      integer*4 n
      call rlm_debug_c(n)
      debug=n
      return
      end subroutine f_rlm_debug
! get RLM_ERRSTRING_MAX length
      integer*4 function f_rlm_maxstr()
!     return the RLM_MAXSTR value
      f_rlm_maxstr=rlm_maxstr_c()
      if(debug.gt.0)write(*,'(a,t26,i8)')'<f_rlm_maxstr:',f_rlm_maxstr
      return
      END FUNCTION f_rlm_maxstr
! get license details
      subroutine f_rlm_license_dtls(i1,i2,s1)
!     i2 - 0 returns expiry date string
!          1    -    contract
!          2    -    customer
!          3    -    issuer
      integer*4 i1,i2
      character*(*) s1
      s1=''
      if(len(s1).ge.64)then
       call rlm_license_dtls_c(i1,i2,s1)
       if(debug.gt.0)write(*,'(a,i2,a,t26,a)')'<f_rlm_license_dtls',i2,
     *                           ':',trim(s1)
      else
       write(*,'(1x,2(a,i3))')
     *    '<f_rlm_license_dtls: s1 length ',len(s1),
     *    ' is < MAXDTLS of ',maxdtls
      endif
      return
      END SUBROUTINE f_rlm_license_dtls
!
	END MODULE FTN_RLM_CALLS
