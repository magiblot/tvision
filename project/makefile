.autodepend
.swap

!if $d(DOS16)
TVFLAGS = $(TVFLAGS) -DDOS16
!endif
!if $d(DOS32)
TVFLAGS = $(TVFLAGS) -DDOS32
!endif
!if $d(WIN32)
TVFLAGS = $(TVFLAGS) -DWIN32
!endif
!if $d(DEBUG)
TVFLAGS = $(TVFLAGS) -DDEBUG
!endif
!if $d(TVDEBUG)
TVFLAGS = $(TVFLAGS) -DTVDEBUG
!endif
!if $d(OVERLAY)
TVFLAGS = $(TVFLAGS) -DOVERLAY
!endif
!if $d(ALIGNMENT)
TVFLAGS = $(TVFLAGS) -DALIGNMENT=$(ALIGNMENT)
!endif
!if $d(EXCEPTIONS)
TVFLAGS = $(TVFLAGS) -DEXCEPTIONS
!endif
!if $d(NO_STREAMABLE)
TVFLAGS = $(TVFLAGS) -DNO_STREAMABLE
!endif

# Avoid conflicts in files with both a CPP and ASM version.
# This is done in several variables to avoid the 'Command arguments too long' issue.
!if $d(DOS32)
EXCLUDE1 = edits.cpp framelin.cpp tvcursor.asm tgrmv.asm ttprvlns.asm
EXCLUDE2 = tvexposd.cpp tvwrite.asm
!else
EXCLUDE1 = edits.cpp framelin.cpp tvcursor.cpp tgrmv.cpp ttprvlns.cpp
EXCLUDE2 = tvexposd.cpp tvwrite.cpp
!endif

EXAMPLES = examples\tvdemo examples\tvdir examples\tvedit examples\tvforms \
           examples\tvhc

all: source\tvision $(EXAMPLES)

source\tvision:
    CD ..\source\tvision
    IF NOT EXIST excluded MD excluded
    @FOR %f IN ($(EXCLUDE1)) DO IF EXIST %f MOVE %f excluded
    @FOR %f IN ($(EXCLUDE2)) DO IF EXIST %f MOVE %f excluded
    $(MAKEDIR)\$(MAKE) $(TVFLAGS) -DTVDIR=..\.. -DSRCDIR=.
    MOVE excluded\* .
    RD excluded
    CD ..\..\project

$(EXAMPLES): source\tvision
    CD ..\$*
    $(MAKEDIR)\$(MAKE) $(TVFLAGS) -DTVDIR=..\..
    CD ..\..\project
