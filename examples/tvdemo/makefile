#
#  Turbo Vision  -  Version 2.0
#
#  TVDEMO Makefile
#
#  One of the following symbols may be defined to build an app in a
#  target other than real mode (the default).
#
#     OVERLAY -  build an overlayed DOS app
#
#     DOS16  -  build a 16 bit DPMI app
#
#     DOS32  -  build a 32 bit DPMI app
#
#
#  The following additional symbols may also be defined:
#
#     DEBUG   -  include debugging information in the app
#
#     TVDEBUG -  use the debugging version of the Turbo Vision
#                library.  This assumes that you have built
#                the debugging version of the Turbo Vision library
#                for whichever target you are compiling to.
#

.AUTODEPEND

SOURCE = tvdemo

!if !$d( SOURCE )
!error You must define SOURCE= to the file you wish to compile.
!endif

!if $d( OVERLAY ) && ( $d( DOS16 ) || $d( DOS32 ) )
!error Overlays cannot be used in protected mode.
!endif

!if !$d(BCROOT)
BCROOT = $(MAKEDIR)\..
!endif

!if !$d(TVDIR)                 # default is for Turbo Vision to be
TVDIR = $(BCROOT)              # installed in same subdir as compiler
!endif

!if "$(BCROOT)"=="$(TVDIR)"    # shorten the include/lib paths if TV
LIBPATH = $(BCROOT)\LIB;       # is in the same place as the compiler
INCPATH = $(BCROOT)\INCLUDE;
!else
LIBPATH = $(TVDIR)\LIB;$(BCROOT)\LIB
INCPATH = $(TVDIR)\INCLUDE;$(BCROOT)\INCLUDE
!endif

!ifdef DEBUG                   # set appropriate flags for debugging
CCDEBUGFLAG = -v
LINKDEBUGFLAG = /v
DEBUGSUFFIX = d
!endif

!ifdef TVDEBUG                 # this suffix is added to Turbo Vision
TVSUFFIX = d                   # library name, ie TV.LIB becomes TVD.LIB
!endif

!ifdef OVERLAY                 # set overlay flags
CCOVYFLAGS  = -Y -Vs
LINKOVY_ON  = /o+
LINKOVY_OFF = /o-
!endif

!if $d(BGI)                    # set BGI library
BGILIB = graphics.lib
!if $d(DOS16)
BGILIB = bgi16.lib
!endif
!if $d(DOS32)
BGILIB = bgi32.lib
!endif
!endif

#
# default options produce a real mode application
#

CFGFILE    = $(SOURCE).CFG
CC         = $(BCROOT)\BIN\bcc +$(CFGFILE)
TLINK      = $(BCROOT)\BIN\tlink
STARTUP    = c0l.obj
TVLIBNAME  = tv$(TVSUFFIX).lib
STDLIBS    = $(BGILIB) emu.lib mathl.lib cl.lib
CCFLAGS    = -ml $(CCDEBUGFLAG) $(CCOVYFLAGS)
LINKFLAGS  = $(LINKDEBUGFLAG)
OBJDIR     = objs16$(DEBUGSUFFIX)

#
# overlayed application options
#

!ifdef OVERLAY
TVLIBNAME  = tvo$(TVSUFFIX).lib
STDLIBS    = tvno$(TVSUFFIX).lib overlay.lib emu.lib mathl.lib cl.lib
!endif

#
# 16 bit protected mode application options
#

!ifdef DOS16
CFGFILE    = $(SOURCE).C16
CC         = $(BCROOT)\BIN\bcc +$(CFGFILE)
TLINK      = $(BCROOT)\BIN\tlink
STARTUP    = c0x.obj
TVLIBNAME  = tv$(TVSUFFIX).lib
STDLIBS    = $(BGILIB) dpmi16.lib emux.lib mathwl.lib cwl.lib
CCFLAGS    = -WX -ml $(CCDEBUGFLAG)
LINKFLAGS  = /Txe $(LINKDEBUGFLAG)
OBJDIR     = objs16$(DEBUGSUFFIX)
!endif

#
# 32 bit protected mode application options
#

!ifdef DOS32
CFGFILE    = $(SOURCE).C32
CC         = $(BCROOT)\BIN\bcc32 +$(CFGFILE)
TLINK      = $(BCROOT)\BIN\tlink32
STARTUP    = c0x32.obj
TVLIBNAME  = tv32$(TVSUFFIX).lib
STDLIBS    = $(BGILIB) dpmi32.lib cw32.lib
CCFLAGS    = -WX $(CCDEBUGFLAG)
LINKFLAGS  = /Tpe/ax $(LINKDEBUGFLAG)
OBJDIR     = objs32$(DEBUGSUFFIX)
!endif

#
# tell MAKE where to look for OBJ files
#

.path.obj = $(OBJDIR)

all: $(SOURCE).exe
  DEL $(CFGFILE)

#
# implicit rule takes care of compiling all C++ files
#

.cpp.obj:
  $(CC) $(CCFLAGS) -c -n$(OBJDIR) +$(CFGFILE) {$< }

#
# link the exe
#

OBJS = pfxASCII.OBJ    pfxCALC.OBJ     pfxCALENDAR.OBJ \
       pfxFILEVIEW.OBJ pfxGADGETS.OBJ  pfxMOUSEDLG.OBJ \
       pfxPUZZLE.OBJ   pfxTVDEMO1.OBJ  pfxTVDEMO2.OBJ  \
       pfxTVDEMO3.OBJ  pfxEVNTVIEW.OBJ pfxBACKGRND.OBJ

OBJPATH = $(OBJDIR)\ #

$(SOURCE).exe: $(OBJS:pfx=) $(TVDIR)\LIB\$(TVLIBNAME)
  $(TLINK) @&&|
  $(LINKFLAGS)/L$(LIBPATH) +    # linker options
  $(STARTUP) +                  # startup code
  $(OBJS:pfx=$(OBJPATH))        # object files
  $(SOURCE)                     # exe name
  $(SOURCE)                     # mapfile
  $(LINKOVY_ON) $(TVLIBNAME) +  # overlayable libraries
  $(LINKOVY_OFF) $(STDLIBS)     # non-overlayable libraries
|

#
# source files depend on the cfg file and OBJs directory
#

*.cpp: cfg dirs

#
# create the configuration file and OBJs directory
#

cfg:
  copy &&|
  -w
  -I$(INCPATH)
  -L$(LIBPATH)
| $(CFGFILE)

dirs:
  IF NOT EXIST $(OBJDIR) MD $(OBJDIR)
