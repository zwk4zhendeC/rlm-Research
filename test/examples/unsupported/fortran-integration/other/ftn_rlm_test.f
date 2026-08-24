C      *************************************************************
C      *        Test RLM CALLS (using ftn_calls.f & ftn_rlm.c)     *
C      *  VERSION 1.0            RSH                  08-Dec-2011  *
C      *    Zentech International Ltd.   roger@zentech.co.uk       *
C      *                   (please use freely)                     *
C      *************************************************************
C--------------------------------------------------------------------
      program test
      use FTN_RLM_CALLS
      character*64  path,argv0,licopts,product,version,
     *              issuer,contract,customer,expiry
      character*512 rlmerr
      integer*4     handle,license,count,irr,idbg,iexp,narg

! Set default values
      data argv0,licopts/'',''/
      data path,product,version,count,idbg/'.','test','1.0',1,1/
! Use command line params if present 
      narg = Iargc()
      if(narg.gt.0)call getarg(1,path)
      if(narg.gt.1)call getarg(2,product)
      if(narg.gt.2)call getarg(3,version)
      if(narg.gt.3)then
        call getarg(4,rlmerr)
        read(rlmerr,*,err=7)count
      endif
 7    if(narg.gt.4)then
        call getarg(5,rlmerr)
        read(rlmerr,*,err=8)idbg
      endif
! Show settings
8     write(*,'(a)')'path:    '//trim(path)
      write(*,'(a)')'product: '//trim(product)
      write(*,'(a)')'version: '//trim(version)
      write(*,'(a,i3)')'count:   ',count
      write(*,'(a,i3)')'idbg:    ',idbg
! Convert all strings sent to C to C strings!!
! Failure to send C strings will result in error
      path=trim(path)//char(0)
      argv0=trim(argv0)//char(0)
      licopts=trim(licopts)//char(0)
      product=trim(product)//char(0)
      version=trim(version)//char(0)
! set debug & get max error string length (wrapper calls)
      if(idbg.gt.0)call f_rlm_debug(idbg)
      maxstr=f_rlm_maxstr()
! initialise RLM
      handle=rlm_init(path,argv0,licopts)
      if(rlm_stat(handle).ne.0)goto 97
! checkout
      license=rlm_checkout(handle,product,version,count)
      if(rlm_license_stat(license).ne.0)goto 99
! expiry time
      iexp=rlm_license_exp_days(license)
! details (wrapper calls)
      call f_rlm_license_dtls(license,0,expiry)
      call f_rlm_license_dtls(license,1,contract)
      call f_rlm_license_dtls(license,2,customer)
      call f_rlm_license_dtls(license,3,issuer)
! pause before health check
      write(*,'(a,$)')'any entry to check health, checkin & close'
      read(*,'(a)')
      if(rlm_get_attr_health(license).ne.0)goto 99
! checkin & close up
      call rlm_checkin(license)
      call rlm_close(handle)
      write(*,*)'========= finshed OK ============'
      stop
97    call rlm_errstring(0,handle,rlmerr)
      goto 100
99    call rlm_errstring(license,0,rlmerr)
100   write(*,*)'********* error *****************'
      write(*,*)'error, msg:',irr,trim(rlmerr)
      end
