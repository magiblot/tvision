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

EXAMPLES = examples\tvdemo examples\tvdir examples\tvedit examples\tvforms \
           examples\tvhc

all: source\tvision $(EXAMPLES)

source\tvision:
    CD ..\source\tvision
    $(MAKEDIR)\$(MAKE) $(TVFLAGS) -DTVDIR=..\.. -DSRCDIR=.
    CD ..\..\project

$(EXAMPLES): source\tvision
    CD ..\$*
    $(MAKEDIR)\$(MAKE) $(TVFLAGS) -DTVDIR=..\..
    CD ..\..\project
