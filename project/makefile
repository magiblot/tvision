.autodepend
.swap

!if !$d(TVDIR)  # From the point of view of the underlying targets.
TVDIR = ..\..
TVFLAGS = $(TVFLAGS) -DTVDIR=$(TVDIR)
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
!if $d(NOTASM)
TVFLAGS = $(TVFLAGS) -DNOTASM
!endif

!if !$d(PRJDIR)
PRJDIR = $(TVDIR)\PROJECT
!endif
!if !$d(EXCLUDE)  # To avoid conflicts in files with both a CPP and ASM version.
!if $d(DOS32)
EXCLUDE=EDITS.CPP FRAMELIN.ASM TVCURSOR.ASM TGRMV.ASM
!else
EXCLUDE=EDITS.CPP FRAMELIN.CPP TVCURSOR.CPP TGRMV.CPP
!endif
!endif

all: source\tvision examples\tvdemo examples\tvedit

source\tvision:
    CD ..\$@
    IF EXIST *.CFG DEL *.CFG  # In case build failed last time.
    IF NOT EXIST excluded MD excluded
    @for %f in ($(EXCLUDE)) do IF EXIST %f MOVE %f excluded
    $(MAKEDIR)\$(MAKE) $(TVFLAGS) -DSRCDIR=.
    MOVE excluded\* .
    RD excluded
    CD $(PRJDIR)

examples\tvdemo: source\tvision
    CD ..\$@
    IF EXIST *.CFG DEL *.CFG
    $(MAKEDIR)\$(MAKE) $(TVFLAGS)
    CD $(PRJDIR)

examples\tvedit: source\tvision
    CD ..\$@
    IF EXIST *.CFG DEL *.CFG
    $(MAKEDIR)\$(MAKE) $(TVFLAGS)
    CD $(PRJDIR)
