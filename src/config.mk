# 
# $Id$
#

#******* type of build, if BUILD is not 'debug', we make the optimised version
BUILD=opt

#******** location of files

INCLUDE_DIR = ${WORKSPACE}/include
INSTALL_PREFIX := ${HOME}
INSTALL_EXE_DIR = $(INSTALL_PREFIX)/bin
INSTALL_LIB_DIR = $(INSTALL_PREFIX)/lib
INSTALL_INCLUDE_DIR = $(INSTALL_PREFIX)/include

# location of .o, .a and executables, set DEST if you don't agree with the default
ifndef DEST
ifeq ($(BUILD),debug)
DEST=debug/
else # not debug
DEST=opt/
endif # debug/ ?
endif # DEST not defined

#******* type of graphics used by display()
# possible values: X, PGM, MATHLINK, NONE
# note: only used in display/lib.mk and for GRAPH_LIBS
GRAPHICS=X

#******* command used for install* targets
INSTALL=cp
# could be something like 
#INSTALL="install -s -m 755"

#****** find out which system we're on

include $(WORKSPACE)/Makefile_common_system_type

#******** type of parallel library
# PARALIB can be EPX or PVM
ifeq ($(SYSTEM),CC)
PARALIB=EPX
else
PARALIB=PVM
endif # not CC

#******** compiler that will be used

# for C++ 
CXX=g++ 
# for C
CC=gcc


#******* compiler and linker extra options


#** EXTRA_OPT: for compiler
# allow the user to get some extra options by using make EXTRA_OPT=bla 
ifeq ($(CC),gcc)
EXTRA_OPT =-Wall -Wno-deprecated
endif

#** EXTRA_LINK_OPT: for linker
#allow the user to get extra options for link time
EXTRA_LINK_OPT=
#** compiler options


#******** variables used only for ecat7
# local/Makefile_common can override these defaults
LLN_INCLUDE_DIR=$(WORKSPACE)/../lln/ecat
LLN_LIB_DIR=$(LLN_INCLUDE_DIR)


#********  customisation
# include local configuration file, possibly overriding any of the above
# use -include to not warn when it does not exist

-include $(WORKSPACE)/local/config.mk



#***********************************************************************
# from here on, things are noncustomisable anymore 
# (except by editing this file, which is strongly discouraged for 'normal' users)
#***********************************************************************





#******* compiler and linker options

ifeq ($(SYSTEM),LINUX)
# note for gcc 2.95.2:
# do not use -malign-double as it crashes in iostream stuff
OPTIM_OPT=-O3  -ffast-math -DNDEBUG
else
ifeq ($(SYSTEM),CYGWIN)
OPTIM_OPT=-O3  -ffast-math -malign-double -DNDEBUG
else
OPTIM_OPT=-O3 -DNDEBUG
endif
endif

DEBUG_OPT=-D_DEBUG -g



ifeq ($(BUILD),debug)
CFLAGS = $(DEBUG_OPT)  $(EXTRA_OPT)  -I$(INCLUDE_DIR) 
else # release version
CFLAGS = $(OPTIM_OPT)  $(EXTRA_OPT)  -I$(INCLUDE_DIR) 
endif # BUILD ?



#** LINK_OPT:  add specific libraries and switches depending on platforms
# possibly this should check on AIX as well
# if so, we should additionally check on ifeq($(CC),gcc)
# as -Xlinker is only appropriate for gcc
ifeq ($(SYSTEM),CC)
LINK_OPT=-Xlinker -bbigtoc $(EXTRA_LINK_OPT) $(EXTRA_LIBS)
else
LINK_OPT=$(EXTRA_LINK_OPT) $(EXTRA_LIBS) 
endif


#******** libraries

#** system libraries



SYS_LIBS = -lm
 
# add any others specific to your system using the SYSTEM macros


#******* LLN matrix libraries
# check if we find the Louvain la Neuve distribution by looking for matrix.h
ifeq ($(wildcard $(LLN_INCLUDE_DIR)/matrix.h),$(LLN_INCLUDE_DIR)/matrix.h)
  # yes, the LLN files seem to be there, so we can compile 
  HAVE_LLN_MATRIX=1
  CFLAGS +=  -I $(LLN_INCLUDE_DIR) -D HAVE_LLN_MATRIX
  EXTRA_LIBS += $(LLN_LIB_DIR)/libecat.a
  ifeq ($(SYSTEM),SUN)
     SYS_LIBS += -lnsl -lsocket
  endif
endif


#****** other configuration checks
# make a variable that allows us to test if we have gcc 3.0 or later
# If so, this will allow us to simplify generating the dependencies a lot!
IS_GCC_3:=$(shell $(CXX) -v 2>&1 |grep "gcc version [3456789]")

# a variable that will be used to get the real name of an executable
# this is necessary on Windows.
# CYGWIN make automatically appends .exe to executable files in targets etc, but not
# commands such as cp
ifeq ($(SYSTEM),CYGWIN)
EXE_SUFFIX := .exe
else
#EXE_SUFFIX=
endif


