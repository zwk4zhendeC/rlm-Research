#
# Example of calling RLM functions from Python.  This example uses Python 3.
# This is not a complete Python language interface for RLM - instead it shows
# how to pass the essential data types back and forth between Python and RLM.
#
#
# Use Python ctypes library to call RLM functions
# There is a good writeup here: https://docs.python.org/3.4/library/ctypes.html
from ctypes import *

import sys

# This loads rlm1122.dll from the current working directory
rlm=cdll.rlm1122

# This loads librlm1122.so from the current working directory - note that on 
# Windows ctypes adds the .dll but on Linux, etc we need to explicitly call 
# LoadLibrary and supply the suffix
#rlm=cdll.LoadLibrary("librlm1122.so")

# Specify the argument types and return type of the RLM functions to be used.  
# In C, rlm_init()'s prototype is 
#	RLM_HANDLE *rlm_init(const char *, const char *, const char *)
# Pass the const char* as c_char_p.  RLM_HANDLE is an opaque pointer - use an 
# unsigned long to represent this and other RLM handle types in Python.
rlm.rlm_init.argtypes = [ c_char_p, c_char_p, c_char_p ]
rlm.rlm_init.restype = c_ulong

# Argument and return types for rlm_checkout, rlm_stat, rlm_license_stat, 
# rlm_checkin, and rlm_close. Use an unsigned long for the 
# license handle, as we do for the RLM handle returned from rlm_init
rlm.rlm_checkout.argtypes= [ c_ulong, c_char_p, c_char_p, c_int ]
rlm.rlm_checkout.restype = c_ulong

rlm.rlm_stat.argtypes = [ c_ulong ] 
rlm.rlm_stat.restype = c_int

rlm.rlm_license_stat.argtypes = [ c_ulong ]
rlm.rlm_license_stat.restype = c_int

rlm.rlm_checkin.argtypes = [ c_ulong ]
rlm.rlm_checkin.restype = c_int

rlm.rlm_close.argtypes = [ c_ulong ]
rlm.rlm_close.restype = c_int

rlm.rlm_get_attr_health.argtypes = [c_ulong]
rlm.rlm_get_attr_health.restype = c_int


# Create a string buffer to pass to rlm_errstring if we need it
errbuf=create_string_buffer(512)

lic = c_ulong(0)
rh=rlm.rlm_init(b".",b"",b"")
stat = rlm.rlm_stat(rh)
if stat != 0:
    print("rlm_init error" + stat)
    sys.exit()
		
product=b"test"
version=b"1.0"
lic=rlm.rlm_checkout(rh, product, version, 1)
stat = rlm.rlm_license_stat(lic)
if stat != 0:
    rlm.rlm_errstring(lic, rh, errbuf)
    print(errbuf.value)
    sys.exit()
print("checkout OK")

rlm.rlm_checkin(lic)
rlm.rlm_close(rh)
