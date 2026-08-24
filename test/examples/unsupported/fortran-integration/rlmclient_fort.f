**********************************************************************
C * Program rlmclient_fort                                             *
C * Fortran test client for RLM license manager                        *
C * (c) 2011-06-30 Petr Koci (petr.koci@vscht.cz)                      *
C * The source code can be freely distributed if authorship preserved. *
C * Tested with ifort and gcc under Linux 32bit.                       *
C * No warranty of functionality on your system.                       *
C **********************************************************************
C RSH modification for Windows, add Interface section for c calls
C     works with Intel Fortran on Linux and Windows 
C **********************************************************************

      Program rlmclient_fort
C ....... Declarations .................................................

      Implicit None

      Integer       handle     ! rlm handle
      Integer       license    ! rlm license
      Integer       status     ! rlm status
      Integer       count      ! number of required licenses
      Integer       argc       ! number of command line arguments
      Integer       arglen     ! actual length of single argument string
      Character*64  product    ! product name
      Character*64  version    ! product version
      Character*64  path       ! license file path
      Character*64  nullstring ! empty string
      Character*512 errstring  ! error message
      Character*64  argbuffer  ! command line arguments reading buffer

!=====================================================================
!
! RSH mods - Set up interface to C routines
!
      INTERFACE
      subroutine rlm_init_fort(p,s1,s2,h)
      !DEC$ ATTRIBUTES C, decorate,alias:
     &       'rlm_init_fort_' :: rlm_init_fort
      integer h
      CHARACTER*(*) p,s1,s2
      !DEC$ ATTRIBUTES REFERENCE::p,s1,s2,h
      end subroutine rlm_init_fort

      subroutine rlm_stat_fort(h,s)
      !DEC$ ATTRIBUTES C, decorate,alias:
     &       'rlm_stat_fort_' :: rlm_stat_fort
      integer h,s
      !DEC$ ATTRIBUTES REFERENCE::h,s
      end subroutine rlm_stat_fort

      subroutine rlm_checkout_fort(h,s1,s2,ic,l1)
      !DEC$ ATTRIBUTES C, decorate,alias:
     &       'rlm_checkout_fort_' :: rlm_checkout_fort
      integer h,l1,l2,ic
      CHARACTER*(*) s1,s2
      !DEC$ ATTRIBUTES REFERENCE::h,s1,s2,ic,l1
      end subroutine rlm_checkout_fort

      subroutine rlm_license_stat_fort(l1,s)
      !DEC$ ATTRIBUTES C, decorate,alias: 
     &      'rlm_license_stat_fort_' :: rlm_license_stat_fort
      integer l1,s
      !DEC$ ATTRIBUTES REFERENCE::l1,s
      end subroutine rlm_license_stat_fort

      subroutine rlm_checkin_fort(l1,s)
      !DEC$ ATTRIBUTES C, decorate,alias: 
     &      'rlm_checkin_fort_' :: rlm_checkin_fort
      integer l1,s
      !DEC$ ATTRIBUTES REFERENCE::l1,s
      end subroutine rlm_checkin_fort

      subroutine rlm_errstring_fort(l1,h,s1)
      !DEC$ ATTRIBUTES C, decorate,alias:
     &      'rlm_errstring_fort_' :: rlm_errstring_fort
      integer l1,h
      character*(*) s1
      !DEC$ ATTRIBUTES REFERENCE::l1,h,s1
      end subroutine rlm_errstring_fort

      subroutine rlm_close_fort(h,s)
      !DEC$ ATTRIBUTES C, decorate,alias: 
     &      'rlm_close_fort_' :: rlm_close_fort
      integer h,s
      !DEC$ ATTRIBUTES REFERENCE::h,s
      end subroutine rlm_close_fort
      END INTERFACE
!=====================================================================

C ....... Default values ...............................................

      product    = 'test1' // Char(0)
      version    = '1.0' // Char(0)
      count      = 1
      path       = '.' // Char(0)
      nullstring = ''  // Char(0)
      errstring  = ''  // Char(0)
      handle     = 0
      license    = 0
      status     = 0

C ....... Command line arguments .......................................

      argc = Iargc()

      If (argc .GE. 1) Then    ! 1st argument contains product name
        Call Getarg(1, argbuffer)
        arglen  = Index(argbuffer, ' ') - 1
        product = argbuffer(1:arglen) // Char(0)
      EndIf

      If (argc .GE. 2) Then    ! 2nd argument contains product version
        Call Getarg(2, argbuffer)
        arglen  = Index(argbuffer, ' ') - 1
        version = argbuffer(1:arglen) // Char(0)
      EndIf

      If (argc .GE. 3) Then    ! 3rd argument contains number of licenses required
        Call Getarg(3, argbuffer)
        Read(argbuffer,*) count
      EndIf

C ....... Initial info to console .....................................      

      Write(*,*)
      Write(*,'(1X,A)')    'RLM licensing test:'
      Write(*,'(1X,A)')    '-------------------'
      Write(*,'(1X,A)')    'Product: ' // product
      Write(*,'(1X,A)')    'Version: ' // version
      Write(*,'(1X,A,I2)') 'Count:  '  ,  count
      Write(*,'(1X,A)')    '-------------------'

C ....... RLM licensing test calls .....................................
      
      Call rlm_init_fort(path, nullstring, nullstring, handle)

      Call rlm_stat_fort(handle, status)

      If (status .EQ. 0) Then

        Call rlm_checkout_fort(handle, product, version, count, license)

        Call rlm_license_stat_fort(license, status)

        If (status .EQ. 0 ) Then

          Write(*,*) 'Checkout succeeded, hit CR to check in back...'

          Pause

          Call rlm_checkin_fort(license, status)
          Write(*,*) 'License checked in back.'
          Call rlm_errstring_fort(license, handle, errstring)
          Write(*,*) errstring

        Else

          Write(*,*) 'Error during license checkout:'
          Call rlm_errstring_fort(license, handle, errstring)
          Write(*,*) errstring

        EndIf

        Call rlm_close_fort(handle, status)
        Write(*,*) 'Closed.'
        Write(*,*)

      Else

        Write(*,*) 'Error during license initialization:'
        Call rlm_errstring_fort(license, handle, errstring)
        Write(*,*) errstring

      EndIf

      STOP
      END

C **********************************************************************
