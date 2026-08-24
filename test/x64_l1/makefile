#
#	RLM Binary Kit makefile - Unix systems.
#
#	You should only have to edit the first definition (ISV) to
#	correspond to your ISV name.  If you have a demo version of
#	RLM, do not change the ISV line below.
#
#	If you have defined an ISV-defined license transfer module,
#	you need to define the object file name here as well.
#

#
#	Beginning of customer-edited definitions
#

ISV = demo

#
#	If you are not using ISV-defined license transfer, the above
#	definitions are the only ones you need to edit.  If you are
#	using ISV-defined transfer, there are more definitions below
#
#
#	Do NOT edit the next 10 lines
#
EXECS = rlc rlmgen rlmclient rlmclient_http rlmsign roam_example \
	$(ISV) $(ISV).strip $(ISV).set rlmstat rlmverify actpro_demo actpro_demo_http

#
#	Targets for building in a cross-dev/target environment
#
ISV_NOSTRIP = $(ISV).nostrip
ISV_STRIP = $(ISV).strip

STEP1EXECS = rlmgenkeys

STEP3EXECS = rlc rlmgen rlmclient rlmclient_http rlmsign roam_example \
		$(ISV_NOSTRIP) rlmstat rlmverify $(ISV_STRIP)

TARGETEXECS = $(ISV).set

SHAREDEXT = so

VER = 1701

SO = librlm$(VER).$(SHAREDEXT)

SO_HTTP = librlm$(VER)_http.$(SHAREDEXT)

all: $(EXECS) $(ISV).hash

#
#	If you are doing cross-development, then:
#
#	1. "make step1" on the cross-dev system
#	2. copy rlmgenkeys to the target; run rlmgenkeys; copy rlm_privkey.c and
#			rlm_pubkey.c back to the cross-dev system into the
#			src directory.
#	3. "make step3" on the cross-dev system
#	4. copy the kit to the target
#	5. "make step5" on the target
#
#	Your kit is now built on the target and ready to use.
#
#	Note: skip steps 1 and 2 if you have a key pair from another
#		rlm platform, and put the keys into the src directory on the
#		cross-dev system; start	from step 3 above.
#
step1:	$(STEP1EXECS)
step3:	$(STEP3EXECS)
step5:	$(TARGETEXECS)

#
#	If you want to support license transfer from another license
#	manager, put the definitions of the interface modules here.
#
#	Put your ISV-defined transfer module object/source/library file names
#	here, plus add any rules to compile/link them.
#	Then, uncomment the $(ISV_XFER_OBJ) rule.
#
#	ISV_XFER_OBJ: the object for your isv transfer definitions.
#	ISV_XFER_SRC: source to the isv transfer defintions
#	ISV_XFER_LIBS: any libraries required for your ISV-transfer LM
#
ISV_XFER_SRC =
ISV_XFER_OBJ =
ISV_XFER_LIBS =
#
#	Uncomment the next line if you are using ISV-defined transfer.
#
#$(ISV_XFER_OBJ):	$(ISV_XFER_SRC)	
#	any special compile rules go here for your ISV-transfer module.

#
#	For example, to build the rlm-rlm example ISV transfer module,
#	set the above definitions as follows:
#
#	ISV_XFER_SRC = ../examples/rlm_transfer.c
#	ISV_XFER_OBJ = rlm_transfer.o
#	ISV_XFER_LIBS =			# No libs needed, since it's all in
#					# the standard rlm libraries.  You
#					# will need the libraries from the
#					# other license manager here.
#	$(ISV_XFER_OBJ):	$(ISV_XFER_SRC)
#		$(CC) -c $(ISV_XFER_SRC) $(CFLAGS)
#

#
#	End of customer-edited definitions
#

CFLAGS = -g -I../src -pthread -Wall -m64 -Dx64_l1 -fPIC -std=gnu99

ARFLAGS = cr

ARXFLAGS = x

CC = cc

LD = $(CC)

LDFLAGS = -lpthread -lstdc++

LIBRARY = rlm.a

NOSSL = rlm_nossl.a

MAINSLIBRARY = rlmmains.a

ALTLIBRARY = librlm.a

XTRALIB = -lpthread -ldl -lrt rlmid1.a

OBJS = rlmclient.o rlm_isv_config.o roam_example.o rehost_example.o

KEYOBJS = rlm_privkey.o rlm_pubkey.o 

WEBOBJS = asp.o balloc.o base64.o cgi.o default.o ejlex.o ejparse.o emfdb.o \
          form.o h.o handler.o main.o mime.o misc.o page.o ringq.o rom.o \
          security.o sock.o sockGen.o sym.o uemf.o um.o umui.o url.o value.o \
          webrom.o webs.o webserver.o websuemf.o

RLMWEBOBJS = rlm_web_form.o rlm_web_options.o rlm_web_platforms.o \
             rlm_web_prompt.o rlm_web_rlminfo.o rlm_web_table.o rlm_web_utils.o \
             rlm_mg_form.o rlm_mg_prompt.o rlm_mg_table.o rlm_mg_utils.o

UTILEXECS = rlmadduser rlmdebug rlmdown rlmhostid rlmnameduser rlmnewlog \
	rlmremove rlmreread rlmstat rlmswitch rlmswitchr

SHAREDFLAG = -shared

SHAREDXTRALIB = 

JAVALD = cc

HASH_CMD := sha256sum

$(LIBRARY):	$(KEYOBJS) rlm_isv_config.o
	@rm -f $(LIBRARY)
	@mkdir _t; cd _t; cp ../lib_client.a $(LIBRARY); ar cr $(LIBRARY) *.o; cp $(LIBRARY) ../;
	@rm -rf _t
	ar $(ARFLAGS) $(LIBRARY) $(KEYOBJS) rlm_isv_config.o

$(NOSSL):	$(LIBRARY)
	@rm -f $(NOSSL)
	@cp lib_client.a $(NOSSL)

../src/rlm_privkey.c:
	@echo ""
	@echo "******************************************************"
	@echo "Your public/private key pair does not exist in ../src"
	@echo "We will generate them for you now..."
	@echo ""
	@echo "If you have previously generated these key files,"
	@echo "place them in ../src now and re-run this make"
	@echo "******************************************************"
	@echo ""
	cd ../src; ../x64_l1/rlmgenkeys
	chmod -w ../src/rlm_pubkey.c ../src/rlm_privkey.c

hosted_activation_demo_keys:
	rm -f ../src/rlm_pubkey.c ../src/rlm_privkey.c
	cp ../examples/hosted_demo_pubkey.c ../src/rlm_pubkey.c
	cp ../examples/hosted_demo_privkey.c ../src/rlm_privkey.c
	chmod -w ../src/rlm_pubkey.c ../src/rlm_privkey.c

rlc:	$(LIBRARY)
	$(LD) $(LDFLAGS) -o rlc rlc.c.o rlm_genlic.a rlmact.c.o libwebs_rom.a rlm_web.a \
						$(LIBRARY) $(XTRALIB) -lcrypto
	
rlmgen:	$(LIBRARY)
	$(LD) $(LDFLAGS) -o rlmgen rlc.c.o rlm_genlic.a rlmgen.c.o libwebs_rom.a rlm_web.a \
						$(LIBRARY) $(XTRALIB) -lcrypto
	
actpro_demo: actpro_demo.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o actpro_demo actpro_demo.o $(LIBRARY) $(XTRALIB) -lcrypto

actpro_demo_http: actpro_demo.o rlm_msgs_http.c.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o actpro_demo_http actpro_demo.o rlm_msgs_http.c.o \
	$(LIBRARY) $(XTRALIB) -lcurl -lcrypto

actpro_demo.o:  ../examples/actpro_demo.c
	$(CC) $(CFLAGS) -c ../examples/actpro_demo.c

rehost_example.o:	../examples/rehost_example.c
	$(CC) $(CFLAGS) -c ../examples/rehost_example.c

rlm_pubkey.o:	../src/rlm_pubkey.c
	$(CC) $(CFLAGS) -c ../src/rlm_pubkey.c

rlm_privkey.o:	../src/rlm_privkey.c
	$(CC) $(CFLAGS) -c ../src/rlm_privkey.c

rlm_isv_config.o:	../src/rlm_isv_config.c ../src/license_to_run.h
	$(CC) $(CFLAGS) -c ../src/rlm_isv_config.c

rlmclient.o:	../examples/sampleclient.c
	$(CC) $(CFLAGS) -o rlmclient.o -c ../examples/sampleclient.c

roam_example.o:	../examples/roam_example.c
	$(CC) $(CFLAGS) -c ../examples/roam_example.c

rehost_example:	rehost_example.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o rehost_example rehost_example.o $(LIBRARY) $(XTRALIB) -lcrypto
	
#
# Note: No dependency on $(LIBRARY) for rlmgenkeys so that we
#	can get the key pair source files created first.
#
rlmgenkeys:
	$(LD) $(LDFLAGS) -o rlmgenkeys rlmgenkeys.o $(LIBRARY) $(XTRALIB) -lcrypto

rlmclient:	rlmclient.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o rlmclient rlmclient.o $(LIBRARY) $(XTRALIB) -lcrypto
	
rlmclient_http:	rlmclient.o rlm_msgs_http.c.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o rlmclient_http rlmclient.o rlm_msgs_http.c.o \
		$(LIBRARY) $(XTRALIB) -lcurl -lcrypto
	
rlmsign:	$(LIBRARY)
	$(LD) $(LDFLAGS) -o rlmsign rlmsign.c.o $(LIBRARY) $(XTRALIB) -lcrypto
	
rlmverify:	$(LIBRARY)
	$(LD) $(LDFLAGS) -o rlmverify rlmverify.c.o $(LIBRARY) $(XTRALIB) -lcrypto

roam_example:	roam_example.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o roam_example roam_example.o $(LIBRARY) $(XTRALIB) -lcrypto
	
$(ISV).set:	$(LIBRARY) rlmsign
	./rlmsign -settings
	
$(ISV):	$(LIBRARY) $(ISV_XFER_OBJ) $(ISV_XFER_LIBS)
	$(LD) $(LDFLAGS) -o $@ isv_main.c.o $(ISV_XFER_OBJ) $(ISV_XFER_LIBS) \
		l_isv.a ls.a $(LIBRARY) $(XTRALIB) -lcrypto

$(ISV).strip: $(ISV)
	strip $(STRIPFLAGS) $(ISV)

$(ISV).hash: $(ISV).strip
	rm -f $@
	$(HASH_CMD) $(ISV) > $@
	echo "# Date created `date -Iseconds`" >> $@
	chmod 444 $@

shared:	$(SO)

#
# NOTE NOTE NOTE!!!
# This target builds the shared object with http client-server transport
# support and works only on Linux and Mac
#
shared_http: $(SO_HTTP)

# The following 2 lines control whether the shared objects contain rlmid1
# dongle support.  The default is to build without rlmid1 support.
#
# To build the shared objects without dongle support:
#       comment out RLMID1LIB and uncomment RLMID1OBJ
#
# To build the shared objects with dongle support:
#       uncomment RLMID1LIB and comment out RLMID1OBJ
#
# One or the other should be commented out - they should
# not both be commented or uncommented.
#
#RLMID1LIB = rlmid1.a
RLMID1OBJ = r/rlm_gethostid_type1.c.o

ARCFLAG_START = -Wl,--whole-archive
ARCFLAG_END = -Wl,--no-whole-archive

$(SO):	$(LIBRARY)
	ar d $(LIBRARY) rlm_privkey.o rlm_sign_license.c.o rlm_sign_lf.c.o $(RLMID1OBJ)
	$(JAVALD) -o $(SO) $(SHAREDFLAG) $(LDFLAGS) $(ARCFLAG_START) $(LIBRARY) $(ARCFLAG_END)  $(RLMID1LIB) -lc -lcrypto $(SHAREDXTRALIB)
	rm $(LIBRARY)

#
# NOTE NOTE NOTE!!!
# This target builds the shared object with http client-server transport
# support and works only on Linux and Mac
#
$(SO_HTTP):   $(LIBRARY)
	ar d $(LIBRARY) rlm_privkey.o rlm_sign_license.c.o rlm_sign_lf.c.o $(RLMID1OBJ) rlm_msgs_http.c.o
	$(JAVALD) -o $(SO_HTTP) $(SHAREDFLAG) $(LDFLAGS) $(ARCFLAG_START) rlm_msgs_http.c.o $(LIBRARY) $(ARCFLAG_END)  $(RLMID1LIB) -lc -lcurl -lcrypto $(SHAREDXTRALIB)
	rm $(LIBRARY)

$(OBJS):	../src/license.h

rlm_pubkey.o:	../src/rlm_pubkey.c

rlm_privkey.o:	../src/rlm_privkey.c

rlmstat:
	make links

links:
	rm -f $(UTILEXECS)
	ln -s rlmutil rlmadduser
	ln -s rlmutil rlmdebug
	ln -s rlmutil rlmdown
	ln -s rlmutil rlmhostid
	ln -s rlmutil rlmnameduser
	ln -s rlmutil rlmnewlog
	ln -s rlmutil rlmremove
	ln -s rlmutil rlmreread
	ln -s rlmutil rlmstat
	ln -s rlmutil rlmswitch
	ln -s rlmutil rlmswitchr

clean:;	rm -f $(OBJS) $(KEYOBJS) $(EXECS) $(ISV).hash $(UTILEXECS) $(SO) $(SO_HTTP) core $(ISV_XFER_OBJ) actpro_demo.o $(LIBRARY) $(ALTLIBRARY)
