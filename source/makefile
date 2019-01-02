#
#   Borland C++ - (C) Copyright 1991, 1992 by Borland International
#   Makefile for building Turbo Vision library
#

#
# Configuration:
#
#   To build an overlayed version of the library:
#
#       -DOVERLAY
#
#   If you installed TVISION somewhere other than directly under
#   BORLANDC or TC, you must invoke MAKE with two defines:
#
#       -DBCROOT=... specifies the path where you installed the compiler
#
#       -DTVDIR=...  specifies the path where you installed TVISION
#
#   Examples:
#
#       for a standard installation:
#
#           make
#
#       for an installation in \COMPILER and TVISION in \AF:
#
#           make -DBCROOT=\COMPILER -DTVDIR=\AF
#
#   Any of these defines can be omitted if the corresponding name
#   is defined in the environment:
#
#       set BCROOT=\COMPILER
#       set TVDIR=\AF
#       make -DOVERLAY
#

#
#                       !!!!!!!!CAUTION!!!!!!!!
#
#   If you are using Turbo C++ and you do not have a copy of TASM,
#   DO NOT DELETE the .OBJ files in the LIB directory.  They are
#   built from .ASM files, and you will not be able to rebuild them.
#
#   If you accidentally delete any of these .OBJ files you will have 
#   to reinstall them from a backup or from your original diskettes.
#

BC=1    # remove this line if using TC

.autodepend
.swap

MODEL = l

!ifndef BCROOT
BCROOT = ..\..
!endif

!ifndef TVDIR
TVDIR = $(BCROOT)\tvision
!endif

OBJDIR = $(TVDIR)\objs

.path.obj = $(OBJDIR)

INCLUDE = $(BCROOT)\include;$(TVDIR)\include
LIBDIR = $(BCROOT)\lib
TVLIBDIR = $(TVDIR)\lib
TVSRCDIR = $(TVDIR)\source

TLIB = $(BCROOT)\bin\tlib /0

!ifdef DEBUG
NDEBUG = 
CBUGFLAGS = -v
ABUGFLAGS = /zi
LBUGFLAGS = /P128
!else
NDEBUG= yes
CBUGFLAGS= -DNDEBUG=yes
ABUGFLAGS=
LBUGFLAGS=
!endif

!if $d(BC)
CC = $(BCROOT)\bin\bcc
!else
CC = $(BCROOT)\bin\tcc
!endif

ASM = $(BCROOT)\bin\tasm

!ifdef OVERLAY
CCOVYFLAGS = -Y -Vs -B
AOVYFLAGS = /o
!endif

CFLAGS = -c $(CCOVYFLAGS) -P -O1 -m$(MODEL) -I$(INCLUDE) -n$(OBJDIR) $(CBUGFLAGS)
AFLAGS = /m$(MODEL) $(AOVYFLAGS) $(ABUGFLAGS)

BCC = $(CC) +tv.cfg

TASM = $(ASM) $(AFLAGS)

OBJS = COLORSEL.OBJ \
       DRIVERS.OBJ  \
       DRIVERS2.OBJ \
       EDITS.OBJ    \
       EDITSTAT.OBJ \
       FRAMELIN.OBJ \
       GRP.OBJ      \
       HELP.OBJ     \
       HELPBASE.OBJ \
       HISTLIST.OBJ \
       MAPCOLOR.OBJ \
       MENU.OBJ     \
       MISC.OBJ     \
       MSGBOX.OBJ   \
       NEW.OBJ      \
       NEWSTR.OBJ   \
       NMBKGRND.OBJ \
       NMBUTTON.OBJ \
       NMCHDRDL.OBJ \
       NMCHKBOX.OBJ \
       NMCLRSEL.OBJ \
       NMCLUSTR.OBJ \
       NMCOLLCT.OBJ \
       NMDIALOG.OBJ \
       NMDIRBOX.OBJ \
       NMDIRCOL.OBJ \
       NMDSKTOP.OBJ \
       NMEDITOR.OBJ \
       NMFILCOL.OBJ \
       NMFILDLG.OBJ \
       NMFILLST.OBJ \
       NMFRAME.OBJ  \
       NMGROUP.OBJ  \
       NMHIST.OBJ   \
       NMINPTLN.OBJ \
       NMLABEL.OBJ  \
       NMLSTBOX.OBJ \
       NMLSTVWR.OBJ \
       NMMNUBAR.OBJ \
       NMMNUBOX.OBJ \
       NMMNUVW.OBJ  \
       NMPRMTXT.OBJ \
       NMRBTNS.OBJ  \
       NMRESCOL.OBJ \
       NMSCOLL.OBJ  \
       NMSCRBAR.OBJ \
       NMSCROLL.OBJ \
       NMSTCTXT.OBJ \
       NMSTDDLG.OBJ \
       NMSTLINE.OBJ \
       NMSTRCOL.OBJ \
       NMSTRLST.OBJ \
       NMVIEW.OBJ   \
       NMWINDOW.OBJ \
       PALETTE.OBJ  \
       SBKGRND.OBJ  \
       SBUTTON.OBJ  \
       SCHDRDLG.OBJ \
       SCHECKBO.OBJ \
       SCLRSEL.OBJ  \
       SCLUSTER.OBJ \
       SDESKTOP.OBJ \
       SDIALOG.OBJ  \
       SDIRCOLL.OBJ \
       SDIRLIST.OBJ \
       SEDITORS.OBJ \
       SFILCOLL.OBJ \
       SFILDLG.OBJ  \
       SFILELST.OBJ \
       SFINFPNE.OBJ \
       SFINPUTL.OBJ \
       SFRAME.OBJ   \
       SGROUP.OBJ   \
       SHISTORY.OBJ \
       SINPUTLI.OBJ \
       SLABEL.OBJ   \
       SLISTBOX.OBJ \
       SLSTVIEW.OBJ \
       SMENUBAR.OBJ \
       SMENUBOX.OBJ \
       SMNUVIEW.OBJ \
       SPARAMTE.OBJ \
       SRADIOBU.OBJ \
       SRESCOLL.OBJ \
       SSCRLBAR.OBJ \
       SSCROLLE.OBJ \
       SSTATICT.OBJ \
       SSTATUSL.OBJ \
       SSTRCOLL.OBJ \
       SSTRLST.OBJ  \
       STDDLG.OBJ   \
       STRMSTAT.OBJ \
       SVIEW.OBJ    \
       SWAPST.OBJ   \
       SWINDOW.OBJ  \
       SYSERR.OBJ   \
       SYSINT.OBJ   \
       TAPPLICA.OBJ \
       TBKGRND.OBJ  \
       TBUTTON.OBJ  \
       TCHDRDLG.OBJ \
       TCHECKBO.OBJ \
       TCLUSTER.OBJ \
       TCMDSET.OBJ  \
       TCOLLECT.OBJ \
       TDESKTOP.OBJ \
       TDIALOG.OBJ  \
       TDIRCOLL.OBJ \
       TDIRLIST.OBJ \
       TEDITOR1.OBJ \
       TEDITOR2.OBJ \
       TEDITWND.OBJ \
       TEVENT.OBJ   \
       TEXTVIEW.OBJ \
       TFILDLG.OBJ  \
       TFILECOL.OBJ \
       TFILEDTR.OBJ \
       TFILLIST.OBJ \
       TFRAME.OBJ   \
       TGRMV.OBJ    \
       TGROUP.OBJ   \
       THISTORY.OBJ \
       THISTWIN.OBJ \
       THSTVIEW.OBJ \
       TINDICTR.OBJ \
       TINPUTLI.OBJ \
       TLABEL.OBJ   \
       TLISTBOX.OBJ \
       TLSTVIEW.OBJ \
       TMEMO.OBJ    \
       TMENUBAR.OBJ \
       TMENUBOX.OBJ \
       TMNUVIEW.OBJ \
       TMOUSE.OBJ   \
       TOBJECT.OBJ  \
       TOBJSTRM.OBJ \
       TPARAMTE.OBJ \
       TPOINT.OBJ   \
       TPROGRAM.OBJ \
       TRADIOBU.OBJ \
       TRESCOLL.OBJ \
       TRESFILE.OBJ \
       TSCREEN.OBJ  \
       TSCRLBAR.OBJ \
       TSCROLLE.OBJ \
       TSORTCOL.OBJ \
       TSTATICT.OBJ \
       TSTATUSL.OBJ \
       TSTRCOLL.OBJ \
       TSTRLIST.OBJ \
       TTPRVLNS.OBJ \
       TVCURSOR.OBJ \
       TVEXPOSD.OBJ \
       TVIEW.OBJ    \
       TVTEXT1.OBJ  \
       TVTEXT2.OBJ  \
       TVWRITE.OBJ  \
       TWINDOW.OBJ


.cpp.obj:
     $(BCC) {$. }

.asm.obj:
!if $d(BC)
     $(TASM) $&.asm, $(OBJDIR)\$&.obj
!else
     copy $(TVLIBDIR)\$&.obj $(OBJDIR)
!endif

all: tv.cfg dirs $(TVLIBDIR)\tv.lib
    del tv.cfg

dirs: makedummy $(OBJDIR)\dummy.txt
    if exist $(OBJDIR)\dummy.txt del $(OBJDIR)\dummy.txt

makedummy:
    echo dummy >$(OBJDIR)\dummy.txt

$(OBJDIR)\dummy.txt:
    md $(OBJDIR)

tv.cfg:
    echo $(CFLAGS) >tv.cfg

!if !$d(BC)
syserr.obj: syserr.cpp
     copy $(TVLIBDIR)\$&.obj $(OBJDIR)
!endif    

!if $d(OVERLAY) && $d(BC)
sysint.obj : sysint.asm
     $(ASM) /m$(MODEL) $&.asm, $(OBJDIR)\$&.obj
!endif

!if $d(BC)
*.asm : tvwrite.inc

tvwrite.inc : geninc.exe
     geninc > tvwrite.inc

geninc.exe : geninc.obj
     $(CC) -O1 -m$(MODEL) -L$(LIBDIR) -e$&.exe $(OBJDIR)\$&.obj
!endif

$(TVLIBDIR)\tv.lib: $(OBJS)
     del $(TVLIBDIR)\tv.lib
     cd $(OBJDIR)
     $(TLIB) $(LBUGFLAGS) $(TVLIBDIR)\tv.lib @$(TVDIR)\source\tvlib.rsp
     cd $(TVSRCDIR)

clean:
     del $(OBJDIR)\*.obj
