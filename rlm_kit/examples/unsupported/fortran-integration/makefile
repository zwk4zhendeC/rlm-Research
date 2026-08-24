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
EXECS = rlmgenkeys rlc rlmgen rlmclient rlmsign roam_example $(ISV) $(ISV).set \
	$(ISV)_mklic rlmstat rlmverify actdemo

SHAREDEXT = so

VER = 913

SO = librlm$(VER).$(SHAREDEXT)

all: $(EXECS) 

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

CFLAGS = -g -I../src -pthread -Wall -Dx86_l2 -fPIC

ARFLAGS = cr

ARXFLAGS = x

STRIPFLAGS = 

CC = cc

LD = $(CC)

LDFLAGS = 

LIBRARY = rlm.a

NOSSL = rlm_nossl.a

MAINSLIBRARY = rlmmains.a

ALTLIBRARY = librlm.a

XTRALIB = -lpthread

OBJS = rlmclient.o rlm_isv_config.o roam_example.o

KEYOBJS = rlm_privkey.o rlm_pubkey.o 

WEBOBJS = asp.o balloc.o base64.o cgi.o default.o ejlex.o ejparse.o emfdb.o \
          form.o h.o handler.o main.o mime.o misc.o page.o ringq.o rom.o \
          security.o sock.o sockGen.o sym.o uemf.o um.o umui.o url.o value.o \
          webrom.o webs.o webserver.o websuemf.o

RLMWEBOBJS = rlm_web_form.o rlm_web_options.o rlm_web_platforms.o \
             rlm_web_prompt.o rlm_web_rlminfo.o rlm_web_table.o rlm_web_utils.o

UTILEXECS = rlmdebug rlmdown rlmhostid rlmnewlog rlmremove rlmreread rlmstat rlmswitch \
	    rlmswitchr rlmrefresh

SHAREDFLAG = -shared

SHAREDXTRALIB = 

JAVALD = cc

$(LIBRARY):	$(KEYOBJS) rlm_isv_config.o
	ar $(ARFLAGS) $(LIBRARY) $(KEYOBJS) rlm_isv_config.o
	@rm -f $(ALTLIBRARY)
	@ln -s $(LIBRARY) $(ALTLIBRARY)

$(NOSSL):	$(LIBRARY)
	mkdir _t
	cd _t; ar x ../$(LIBRARY); mkdir _t2; cp rlm*.o _t2; cd _t2; ar $(ARFLAGS) $(NOSSL) *.o; mv $(NOSSL) ../..
	ranlib $(NOSSL)
	rm -rf _t

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
	cd ../src; ../x86_l2/rlmgenkeys
	chmod -w ../src/rlm_pubkey.c ../src/rlm_privkey.c

hosted_activation_demo_keys:
	rm -f ../src/rlm_pubkey.c ../src/rlm_privkey.c
	cp ../examples/hosted_demo_pubkey.c ../src/rlm_pubkey.c
	cp ../examples/hosted_demo_privkey.c ../src/rlm_privkey.c
	chmod -w ../src/rlm_pubkey.c ../src/rlm_privkey.c

rlc:	$(LIBRARY)
	ar $(ARXFLAGS) $(MAINSLIBRARY) rlc.o
	ar $(ARXFLAGS) $(LIBRARY) rlm_genlic.o
	ar $(ARXFLAGS) $(LIBRARY) rlmact.o
	$(LD) $(LDFLAGS) -o rlc rlc.o rlm_genlic.o rlmact.o \
						$(LIBRARY) $(XTRALIB)
	rm -f rlc.o rlm_genlic.o rlmact.o
	
rlmgen:	$(LIBRARY)
	ar $(ARXFLAGS) $(MAINSLIBRARY) rlc.o
	ar $(ARXFLAGS) $(LIBRARY) rlm_genlic.o
	ar $(ARXFLAGS) $(LIBRARY) rlmgen.o
	$(LD) $(LDFLAGS) -o rlmgen rlc.o rlm_genlic.o rlmgen.o \
						$(LIBRARY) $(XTRALIB)
	rm -f rlc.o rlm_genlic.o rlmgen.o
	
actdemo:  $(LIBRARY) $(MAINSLIBRARY)
	ar $(ARXFLAGS) $(MAINSLIBRARY) actdemo.o
	$(LD) $(LDFLAGS) -o actdemo actdemo.o $(LIBRARY) $(XTRALIB)
	rm -f actdemo.o
	
rlm_pubkey.o:	../src/rlm_pubkey.c
	$(CC) $(CFLAGS) -c ../src/rlm_pubkey.c

rlm_privkey.o:	../src/rlm_privkey.c
	$(CC) $(CFLAGS) -c ../src/rlm_privkey.c

rlm_isv_config.o:	../src/rlm_isv_config.c ../src/license_to_run.h
	$(CC) $(CFLAGS) -c ../src/rlm_isv_config.c

rlmclient.o:	../examples/rlmclient.c
	$(CC) $(CFLAGS) -c ../examples/rlmclient.c

rlmclient_fort.o:	../examples/rlmclient_fort.f
	ifort -c ../examples/rlmclient_fort.f

rlm_fort.o:	../examples/rlm_fort.c
	$(CC) $(CFLAGS) -c ../examples/rlm_fort.c

roam_example.o:	../examples/roam_example.c
	$(CC) $(CFLAGS) -c ../examples/roam_example.c

#
# Note: No dependency on $(LIBRARY) for rlmgenkeys so that we
#	can get the key pair source files created first.
#
rlmgenkeys:
	ar $(ARXFLAGS) $(MAINSLIBRARY) rlmgenkeys.o
	$(LD) $(LDFLAGS) -o rlmgenkeys rlmgenkeys.o $(LIBRARY) $(XTRALIB)
	rm -f rlmgenkeys.o

rlmclient:	rlmclient.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o rlmclient rlmclient.o $(LIBRARY) $(XTRALIB)

rlmclient_fort:	rlmclient_fort.o rlm_fort.o $(LIBRARY)
	ifort -o rlmclient_fort rlmclient_fort.o rlm_fort.o $(LIBRARY) $(XTRALIB)
	
rlmsign:	$(LIBRARY) $(MAINSLIBRARY)
	ar $(ARXFLAGS) $(MAINSLIBRARY) rlmsign.o
	$(LD) $(LDFLAGS) -o rlmsign rlmsign.o $(LIBRARY) $(XTRALIB)
	rm -f rlmsign.o
	
rlmverify:	$(LIBRARY) $(MAINSLIBRARY)
	ar $(ARXFLAGS) $(MAINSLIBRARY) rlmverify.o
	$(LD) $(LDFLAGS) -o rlmverify rlmverify.o $(LIBRARY) $(XTRALIB)
	rm -f rlmverify.o

roam_example:	roam_example.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o roam_example roam_example.o $(LIBRARY) $(XTRALIB)
	
$(ISV).set:	$(LIBRARY) rlmsign
	./rlmsign -settings
	
$(ISV):	$(LIBRARY) $(MAINSLIBRARY) $(ISV_XFER_OBJ) $(ISV_XFER_LIBS)
	ar $(ARXFLAGS) $(MAINSLIBRARY) isv_main.o
	$(LD) $(LDFLAGS) -o $(ISV) isv_main.o $(ISV_XFER_OBJ) $(ISV_XFER_LIBS) \
		$(LIBRARY) $(XTRALIB)
	rm -f isv_main.o
	strip $(STRIPFLAGS) $(ISV)
	
$(ISV)_mklic:	$(LIBRARY) $(MAINSLIBRARY)
	ar $(ARXFLAGS) $(MAINSLIBRARY) rlm_mklic.o
	$(LD) $(LDFLAGS) -o $(ISV)_mklic rlm_mklic.o $(LIBRARY) $(XTRALIB)
	rm -f rlm_mklic.o
	strip $(STRIPFLAGS) $(ISV)_mklic

shared:	$(SO)

$(SO):	$(LIBRARY)
	rm -rf tmp
	mkdir tmp; \
	cd tmp; \
	mkdir r; \
	mkdir o;\
	cp ../$(LIBRARY) .; \
	ar x $(LIBRARY); \
	mv rlm_*.o r; \
	mv *.o o; \
	rm -f r/rlm_privkey.o; \
	rm -f r/rlm_act_admin.o; \
	rm -f r/rlm_genlic.o; \
	rm -f r/rlm_act_doit.o; \
	rm -f r/rlmgenkeys.o; \
	rm -f r/rlm_mklic.o; \
	rm -f r/rlmsign.o; \
	rm -f r/rlm_sign_license.o; \
	rm -f r/rlm_sign_lf.o; \
	rm -f r/rlmverify.o; \
	cd r; rm -f $(RLMWEBOBJS); cd ..; \
	cd o; rm -f $(WEBOBJS); cd ..; \
	ar cr o.a o/*.o; \
	cd .. 
	$(JAVALD) -o $(SO) $(SHAREDFLAG) tmp/r/*.o tmp/o.a -lc $(SHAREDXTRALIB)
	rm -rf tmp


	
$(OBJS):	../src/license.h

rlm_pubkey.o:	../src/rlm_pubkey.c

rlm_privkey.o:	../src/rlm_privkey.c

rlmstat:
	make links

links:
	rm -f $(UTILEXECS)
	ln -s rlmutil rlmdebug
	ln -s rlmutil rlmdown
	ln -s rlmutil rlmhostid
	ln -s rlmutil rlmnewlog
	ln -s rlmutil rlmremove
	ln -s rlmutil rlmreread
	ln -s rlmutil rlmstat
	ln -s rlmutil rlmswitch
	ln -s rlmutil rlmswitchr
	ln -s rlmutil rlmrefresh

clean:;	rm -f $(OBJS) $(KEYOBJS) rlmsign.o $(EXECS) $(ISV) $(UTILEXECS) $(SO) core $(ISV_XFER_OBJ)
