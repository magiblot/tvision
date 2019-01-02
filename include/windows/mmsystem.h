/****************************************************************************/
/*                                                                          */
/*        MMSYSTEM.H - Include file for Multimedia APIs                     */
/*                                                                          */
/****************************************************************************/
/*    If defined, the following flags inhibit inclusion
 *    of the indicated items:
 *
 *      MMNODRV          - Installable driver support
 *      MMNOSOUND        - Sound support
 *      MMNOWAVE         - Waveform support
 *      MMNOMIDI         - MIDI support
 *      MMNOAUX          - Auxiliary audio support
 *      MMNOTIMER        - Timer support
 *      MMNOJOY          - Joystick support
 *      MMNOMCI          - MCI support
 *      MMNOMMIO         - Multimedia file I/O support
 *      MMNOMMSYSTEM     - General MMSYSTEM functions
 */

/*
 *      C/C++ Run Time Library - Version 6.0
 *
 *      Copyright (c) 1991, 1993 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __MMSYSTEM_H    /* prevent multiple includes */
#define __MMSYSTEM_H

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#if !defined(__FLAT__)

#if !defined(WINVER) || (WINVER < 0x030a)
//#error Multimedia APIs require Windows 3.1
#endif

#ifndef __WINDOWS_H
#include <windows.h>    /* <windows.h> must be included */
#endif  /* __WINDOWS_H */

#ifndef RC_INVOKED
#pragma option -a-      /* Assume byte packing throughout */
#endif


/****************************************************************************

                    General constants and data types

****************************************************************************/

/* general constants */
#define MAXPNAMELEN      32     /* max product name length (including NULL) */
#define MAXERRORLENGTH   128    /* max error text length (including NULL) */

/* general data types */
typedef WORD    VERSION;        /* major (high byte), minor (low byte) */

/* MMTIME data structure */
typedef struct mmtime_tag {
    UINT    wType;              /* indicates the contents of the union */
    union {
        DWORD ms;               /* milliseconds */
        DWORD sample;           /* samples */
        DWORD cb;               /* byte count */
        struct {                /* SMPTE */
            BYTE hour;          /* hours */
            BYTE min;           /* minutes */
            BYTE sec;           /* seconds */
            BYTE frame;         /* frames  */
            BYTE fps;           /* frames per second */
            BYTE dummy;         /* pad */
            } smpte;
        struct {                /* MIDI */
            DWORD songptrpos;   /* song pointer position */
            } midi;
        } u;
    } MMTIME;
typedef MMTIME       *PMMTIME;
typedef MMTIME NEAR *NPMMTIME;
typedef MMTIME FAR  *LPMMTIME;

/* types for wType field in MMTIME struct */
#define TIME_MS         0x0001  /* time in milliseconds */
#define TIME_SAMPLES    0x0002  /* number of wave samples */
#define TIME_BYTES      0x0004  /* current byte offset */
#define TIME_SMPTE      0x0008  /* SMPTE time */
#define TIME_MIDI       0x0010  /* MIDI time */


/****************************************************************************

                    Multimedia Extensions Window Messages

****************************************************************************/

#define MM_JOY1MOVE         0x3A0           /* joystick */
#define MM_JOY2MOVE         0x3A1
#define MM_JOY1ZMOVE        0x3A2
#define MM_JOY2ZMOVE        0x3A3
#define MM_JOY1BUTTONDOWN   0x3B5
#define MM_JOY2BUTTONDOWN   0x3B6
#define MM_JOY1BUTTONUP     0x3B7
#define MM_JOY2BUTTONUP     0x3B8

#define MM_MCINOTIFY        0x3B9           /* MCI */

#define MM_WOM_OPEN         0x3BB           /* waveform output */
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD

#define MM_WIM_OPEN         0x3BE           /* waveform input */
#define MM_WIM_CLOSE        0x3BF
#define MM_WIM_DATA         0x3C0

#define MM_MIM_OPEN         0x3C1           /* MIDI input */
#define MM_MIM_CLOSE        0x3C2
#define MM_MIM_DATA         0x3C3
#define MM_MIM_LONGDATA     0x3C4
#define MM_MIM_ERROR        0x3C5
#define MM_MIM_LONGERROR    0x3C6

#define MM_MOM_OPEN         0x3C7           /* MIDI output */
#define MM_MOM_CLOSE        0x3C8
#define MM_MOM_DONE         0x3C9


/****************************************************************************

                String resource number bases (internal use)

****************************************************************************/

#define MMSYSERR_BASE          0
#define WAVERR_BASE            32
#define MIDIERR_BASE           64
#define TIMERR_BASE            96
#define JOYERR_BASE            160
#define MCIERR_BASE            256

#define MCI_STRING_OFFSET      512
#define MCI_VD_OFFSET          1024
#define MCI_CD_OFFSET          1088
#define MCI_WAVE_OFFSET        1152
#define MCI_SEQ_OFFSET         1216

/****************************************************************************

                        General error return values

****************************************************************************/

/* general error return values */
#define MMSYSERR_NOERROR      0                    /* no error */
#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  /* unspecified error */
#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  /* device ID out of range */
#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  /* driver failed enable */
#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  /* device already allocated */
#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  /* device handle is invalid */
#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  /* no device driver present */
#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  /* memory allocation error */
#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  /* function isn't supported */
#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  /* error value out of range */
#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) /* invalid flag passed */
#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) /* invalid parameter passed */
#define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 11) /* last error in range */


/* If included with the 3.0 windows.h, define compatible aliases */
#if !defined(WINVER) || (WINVER < 0x030a)
DECLARE_HANDLE(HDRVR);
#endif  /* WIN3.0 */

#ifndef MMNODRV
/****************************************************************************

                        Installable driver support

****************************************************************************/

/* If included with the 3.0 windows.h, define compatible aliases */
#if !defined(WINVER) || (WINVER < 0x030a)

/* return values from DriverProc() function */
#define DRV_CANCEL              0x0000
#define DRV_OK                  0x0001
#define DRV_RESTART             0x0002

/* Driver messages */
#define DRV_LOAD                0x0001
#define DRV_ENABLE              0x0002
#define DRV_OPEN                0x0003
#define DRV_CLOSE               0x0004
#define DRV_DISABLE             0x0005
#define DRV_FREE                0x0006
#define DRV_CONFIGURE           0x0007
#define DRV_QUERYCONFIGURE      0x0008
#define DRV_INSTALL             0x0009
#define DRV_REMOVE              0x000A
#define DRV_RESERVED            0x0800
#define DRV_USER                0x4000

/* LPARAM of DRV_CONFIGURE message */
typedef struct tagDRVCONFIGINFO {
    DWORD   dwDCISize;
    LPCSTR  lpszDCISectionName;
    LPCSTR  lpszDCIAliasName;
} DRVCONFIGINFO;
typedef DRVCONFIGINFO        *PDRVCONFIGINFO;
typedef DRVCONFIGINFO  NEAR *NPDRVCONFIGINFO;
typedef DRVCONFIGINFO  FAR  *LPDRVCONFIGINFO;

/* installable driver function prototypes */
LRESULT   WINAPI DrvClose(HDRVR hDriver, LPARAM lParam1, LPARAM lParam2);
HDRVR     WINAPI DrvOpen(LPCSTR szDriverName, LPCSTR szSectionName,
    LPARAM lParam2);
LRESULT   WINAPI DrvSendMessage(HDRVR hDriver, UINT uMessage,
    LPARAM lParam1, LPARAM lParam2);
HINSTANCE WINAPI DrvGetModuleHandle(HDRVR hDriver);

LRESULT WINAPI DrvDefDriverProc(DWORD dwDriverIdentifier, HDRVR driverID,
    UINT uMessage, LPARAM lParam1, LPARAM lParam2);

#define DefDriverProc DrvDefDriverProc

#endif  /* WIN3.0 */


#if defined(WINVER) && (WINVER >= 0x030a)

/* return values from DriverProc() function */
#define DRV_CANCEL             DRVCNF_CANCEL
#define DRV_OK                 DRVCNF_OK
#define DRV_RESTART            DRVCNF_RESTART

#endif  /* WINVER >= 0x030a */

#define DRV_MCI_FIRST          DRV_RESERVED
#define DRV_MCI_LAST           (DRV_RESERVED + 0xFFF)

#endif  /* ifndef MMNODRV */


/****************************************************************************

                          Driver callback support

****************************************************************************/

/* flags used with waveOutOpen(), waveInOpen(), midiInOpen(), and */
/* midiOutOpen() to specify the type of the dwCallback parameter. */

#define CALLBACK_TYPEMASK   0x00070000l    /* callback type mask */
#define CALLBACK_NULL       0x00000000l    /* no callback */
#define CALLBACK_WINDOW     0x00010000l    /* dwCallback is a HWND */
#define CALLBACK_TASK       0x00020000l    /* dwCallback is a HTASK */
#define CALLBACK_FUNCTION   0x00030000l    /* dwCallback is a FARPROC */

/* driver callback prototypes */
typedef void (CALLBACK DRVCALLBACK) (HDRVR h, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef DRVCALLBACK FAR *LPDRVCALLBACK;

/****************************************************************************

                         Manufacturer and product IDs

    Used with wMid and wPid fields in WAVEOUTCAPS, WAVEINCAPS,
    MIDIOUTCAPS, MIDIINCAPS, AUXCAPS, JOYCAPS structures.

****************************************************************************/

/* manufacturer IDs */
#define MM_MICROSOFT            1       /* Microsoft Corp. */

/* product IDs */
#define MM_MIDI_MAPPER          1       /* MIDI Mapper */
#define MM_WAVE_MAPPER          2       /* Wave Mapper */

#define MM_SNDBLST_MIDIOUT      3       /* Sound Blaster MIDI output port */
#define MM_SNDBLST_MIDIIN       4       /* Sound Blaster MIDI input port  */
#define MM_SNDBLST_SYNTH        5       /* Sound Blaster internal synthesizer */
#define MM_SNDBLST_WAVEOUT      6       /* Sound Blaster waveform output */
#define MM_SNDBLST_WAVEIN       7       /* Sound Blaster waveform input */

#define MM_ADLIB                9       /* Ad Lib-compatible synthesizer */

#define MM_MPU401_MIDIOUT       10      /* MPU401-compatible MIDI output port */
#define MM_MPU401_MIDIIN        11      /* MPU401-compatible MIDI input port */

#define MM_PC_JOYSTICK          12      /* Joystick adapter */


#ifndef MMNOMMSYSTEM
/****************************************************************************

                    General MMSYSTEM support

****************************************************************************/

WORD WINAPI mmsystemGetVersion(void);
void WINAPI OutputDebugStr(LPCSTR);

#endif  /* ifndef MMNOMMSYSTEM */


#ifndef MMNOSOUND
/****************************************************************************

                            Sound support

****************************************************************************/

BOOL WINAPI sndPlaySound(LPCSTR lpszSoundName, UINT uFlags);

/* flag values for wFlags parameter */
#define SND_SYNC            0x0000  /* play synchronously (default) */
#define SND_ASYNC           0x0001  /* play asynchronously */
#define SND_NODEFAULT       0x0002  /* don't use default sound */
#define SND_MEMORY          0x0004  /* lpszSoundName points to a memory file */
#define SND_LOOP            0x0008  /* loop the sound until next sndPlaySound */
#define SND_NOSTOP          0x0010  /* don't stop any currently playing sound */

#endif  /* ifndef MMNOSOUND */


#ifndef MMNOWAVE
/****************************************************************************

                        Waveform audio support

****************************************************************************/

/* waveform audio error return values */
#define WAVERR_BADFORMAT      (WAVERR_BASE + 0)    /* unsupported wave format */
#define WAVERR_STILLPLAYING   (WAVERR_BASE + 1)    /* still something playing */
#define WAVERR_UNPREPARED     (WAVERR_BASE + 2)    /* header not prepared */
#define WAVERR_SYNC           (WAVERR_BASE + 3)    /* device is synchronous */
#define WAVERR_LASTERROR      (WAVERR_BASE + 3)    /* last error in range */

/* waveform audio data types */
DECLARE_HANDLE(HWAVE);
DECLARE_HANDLE(HWAVEIN);
DECLARE_HANDLE(HWAVEOUT);
typedef HWAVEIN FAR *LPHWAVEIN;
typedef HWAVEOUT FAR *LPHWAVEOUT;
typedef DRVCALLBACK WAVECALLBACK;
typedef WAVECALLBACK FAR *LPWAVECALLBACK;

/* wave callback messages */
#define WOM_OPEN        MM_WOM_OPEN
#define WOM_CLOSE       MM_WOM_CLOSE
#define WOM_DONE        MM_WOM_DONE
#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA

/* device ID for wave device mapper */
#define WAVE_MAPPER     (-1)

/* flags for dwFlags parameter in waveOutOpen() and waveInOpen() */
#define  WAVE_FORMAT_QUERY     0x0001
#define  WAVE_ALLOWSYNC        0x0002

/* wave data block header */
typedef struct wavehdr_tag {
    LPSTR       lpData;                 /* pointer to locked data buffer */
    DWORD       dwBufferLength;         /* length of data buffer */
    DWORD       dwBytesRecorded;        /* used for input only */
    DWORD       dwUser;                 /* for client's use */
    DWORD       dwFlags;                /* assorted flags (see defines) */
    DWORD       dwLoops;                /* loop control counter */
    struct wavehdr_tag far *lpNext;     /* reserved for driver */
    DWORD       reserved;               /* reserved for driver */
} WAVEHDR;
typedef WAVEHDR       *PWAVEHDR;
typedef WAVEHDR NEAR *NPWAVEHDR;
typedef WAVEHDR FAR  *LPWAVEHDR;

/* flags for dwFlags field of WAVEHDR */
#define WHDR_DONE       0x00000001  /* done bit */
#define WHDR_PREPARED   0x00000002  /* set if this header has been prepared */
#define WHDR_BEGINLOOP  0x00000004  /* loop start block */
#define WHDR_ENDLOOP    0x00000008  /* loop end block */
#define WHDR_INQUEUE    0x00000010  /* reserved for driver */

/* waveform output device capabilities structure */
typedef struct waveoutcaps_tag {
    UINT    wMid;                  /* manufacturer ID */
    UINT    wPid;                  /* product ID */
    VERSION vDriverVersion;        /* version of the driver */
    char    szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    DWORD   dwFormats;             /* formats supported */
    UINT    wChannels;             /* number of sources supported */
    DWORD   dwSupport;             /* functionality supported by driver */
} WAVEOUTCAPS;
typedef WAVEOUTCAPS       *PWAVEOUTCAPS;
typedef WAVEOUTCAPS NEAR *NPWAVEOUTCAPS;
typedef WAVEOUTCAPS FAR  *LPWAVEOUTCAPS;

/* flags for dwSupport field of WAVEOUTCAPS */
#define WAVECAPS_PITCH          0x0001   /* supports pitch control */
#define WAVECAPS_PLAYBACKRATE   0x0002   /* supports playback rate control */
#define WAVECAPS_VOLUME         0x0004   /* supports volume control */
#define WAVECAPS_LRVOLUME       0x0008   /* separate left-right volume control */
#define WAVECAPS_SYNC           0x0010

/* waveform input device capabilities structure */
typedef struct waveincaps_tag {
    UINT    wMid;                    /* manufacturer ID */
    UINT    wPid;                    /* product ID */
    VERSION vDriverVersion;          /* version of the driver */
    char    szPname[MAXPNAMELEN];    /* product name (NULL terminated string) */
    DWORD   dwFormats;               /* formats supported */
    UINT    wChannels;               /* number of channels supported */
} WAVEINCAPS;
typedef WAVEINCAPS       *PWAVEINCAPS;
typedef WAVEINCAPS NEAR *NPWAVEINCAPS;
typedef WAVEINCAPS FAR  *LPWAVEINCAPS;

/* defines for dwFormat field of WAVEINCAPS and WAVEOUTCAPS */
#define WAVE_INVALIDFORMAT     0x00000000       /* invalid format */
#define WAVE_FORMAT_1M08       0x00000001       /* 11.025 kHz, Mono,   8-bit  */
#define WAVE_FORMAT_1S08       0x00000002       /* 11.025 kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_1M16       0x00000004       /* 11.025 kHz, Mono,   16-bit */
#define WAVE_FORMAT_1S16       0x00000008       /* 11.025 kHz, Stereo, 16-bit */
#define WAVE_FORMAT_2M08       0x00000010       /* 22.05  kHz, Mono,   8-bit  */
#define WAVE_FORMAT_2S08       0x00000020       /* 22.05  kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_2M16       0x00000040       /* 22.05  kHz, Mono,   16-bit */
#define WAVE_FORMAT_2S16       0x00000080       /* 22.05  kHz, Stereo, 16-bit */
#define WAVE_FORMAT_4M08       0x00000100       /* 44.1   kHz, Mono,   8-bit  */
#define WAVE_FORMAT_4S08       0x00000200       /* 44.1   kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_4M16       0x00000400       /* 44.1   kHz, Mono,   16-bit */
#define WAVE_FORMAT_4S16       0x00000800       /* 44.1   kHz, Stereo, 16-bit */

/* general waveform format structure (information common to all formats) */
typedef struct waveformat_tag {
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
} WAVEFORMAT;
typedef WAVEFORMAT       *PWAVEFORMAT;
typedef WAVEFORMAT NEAR *NPWAVEFORMAT;
typedef WAVEFORMAT FAR  *LPWAVEFORMAT;

/* flags for wFormatTag field of WAVEFORMAT */
#define WAVE_FORMAT_PCM     1

/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;
} PCMWAVEFORMAT;
typedef PCMWAVEFORMAT       *PPCMWAVEFORMAT;
typedef PCMWAVEFORMAT NEAR *NPPCMWAVEFORMAT;
typedef PCMWAVEFORMAT FAR  *LPPCMWAVEFORMAT;

/* waveform audio function prototypes */
UINT WINAPI waveOutGetNumDevs(void);
UINT WINAPI waveOutGetDevCaps(UINT uDeviceID, WAVEOUTCAPS FAR* lpCaps,
    UINT uSize);
UINT WINAPI waveOutGetVolume(UINT uDeviceID, DWORD FAR* lpdwVolume);
UINT WINAPI waveOutSetVolume(UINT uDeviceID, DWORD dwVolume);
UINT WINAPI waveOutGetErrorText(UINT uError, LPSTR lpText, UINT uSize);
UINT WINAPI waveOutOpen(HWAVEOUT FAR* lphWaveOut, UINT uDeviceID,
    const WAVEFORMAT FAR* lpFormat, DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
UINT WINAPI waveOutClose(HWAVEOUT hWaveOut);
UINT WINAPI waveOutPrepareHeader(HWAVEOUT hWaveOut,
     WAVEHDR FAR* lpWaveOutHdr, UINT uSize);
UINT WINAPI waveOutUnprepareHeader(HWAVEOUT hWaveOut,
    WAVEHDR FAR* lpWaveOutHdr, UINT uSize);
UINT WINAPI waveOutWrite(HWAVEOUT hWaveOut, WAVEHDR FAR* lpWaveOutHdr,
    UINT uSize);
UINT WINAPI waveOutPause(HWAVEOUT hWaveOut);
UINT WINAPI waveOutRestart(HWAVEOUT hWaveOut);
UINT WINAPI waveOutReset(HWAVEOUT hWaveOut);
UINT WINAPI waveOutBreakLoop(HWAVEOUT hWaveOut);
UINT WINAPI waveOutGetPosition(HWAVEOUT hWaveOut, MMTIME FAR* lpInfo,
    UINT uSize);
UINT WINAPI waveOutGetPitch(HWAVEOUT hWaveOut, DWORD FAR* lpdwPitch);
UINT WINAPI waveOutSetPitch(HWAVEOUT hWaveOut, DWORD dwPitch);
UINT WINAPI waveOutGetPlaybackRate(HWAVEOUT hWaveOut, DWORD FAR* lpdwRate);
UINT WINAPI waveOutSetPlaybackRate(HWAVEOUT hWaveOut, DWORD dwRate);
UINT WINAPI waveOutGetID(HWAVEOUT hWaveOut, UINT FAR* lpuDeviceID);

#if defined(WINVER) && (WINVER >= 0x030a)
DWORD WINAPI waveOutMessage(HWAVEOUT hWaveOut, UINT uMessage, DWORD dw1, DWORD dw2);
#endif  /* WINVER >= 0x030a */

UINT WINAPI waveInGetNumDevs(void);
UINT WINAPI waveInGetDevCaps(UINT uDeviceID, WAVEINCAPS FAR* lpCaps,
    UINT uSize);
UINT WINAPI waveInGetErrorText(UINT uError, LPSTR lpText, UINT uSize);
UINT WINAPI waveInOpen(HWAVEIN FAR* lphWaveIn, UINT uDeviceID,
    const WAVEFORMAT FAR* lpFormat, DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
UINT WINAPI waveInClose(HWAVEIN hWaveIn);
UINT WINAPI waveInPrepareHeader(HWAVEIN hWaveIn,
    WAVEHDR FAR* lpWaveInHdr, UINT uSize);
UINT WINAPI waveInUnprepareHeader(HWAVEIN hWaveIn,
    WAVEHDR FAR* lpWaveInHdr, UINT uSize);
UINT WINAPI waveInAddBuffer(HWAVEIN hWaveIn,
    WAVEHDR FAR* lpWaveInHdr, UINT uSize);
UINT WINAPI waveInStart(HWAVEIN hWaveIn);
UINT WINAPI waveInStop(HWAVEIN hWaveIn);
UINT WINAPI waveInReset(HWAVEIN hWaveIn);
UINT WINAPI waveInGetPosition(HWAVEIN hWaveIn, MMTIME FAR* lpInfo,
    UINT uSize);
UINT WINAPI waveInGetID(HWAVEIN hWaveIn, UINT FAR* lpuDeviceID);

#if defined(WINVER) && (WINVER >= 0x030a)
DWORD WINAPI waveInMessage(HWAVEIN hWaveIn, UINT uMessage, DWORD dw1, DWORD dw2);
#endif  /* WINVER >= 0x030a */

#endif  /* ifndef MMNOWAVE */


#ifndef MMNOMIDI
/****************************************************************************

                            MIDI audio support

****************************************************************************/

/* MIDI error return values */
#define MIDIERR_UNPREPARED    (MIDIERR_BASE + 0)   /* header not prepared */
#define MIDIERR_STILLPLAYING  (MIDIERR_BASE + 1)   /* still something playing */
#define MIDIERR_NOMAP         (MIDIERR_BASE + 2)   /* no current map */
#define MIDIERR_NOTREADY      (MIDIERR_BASE + 3)   /* hardware is still busy */
#define MIDIERR_NODEVICE      (MIDIERR_BASE + 4)   /* port no longer connected */
#define MIDIERR_INVALIDSETUP  (MIDIERR_BASE + 5)   /* invalid setup */
#define MIDIERR_LASTERROR     (MIDIERR_BASE + 5)   /* last error in range */

/* MIDI audio data types */
DECLARE_HANDLE(HMIDI);
DECLARE_HANDLE(HMIDIIN);
DECLARE_HANDLE(HMIDIOUT);
typedef HMIDIIN FAR *LPHMIDIIN;
typedef HMIDIOUT FAR *LPHMIDIOUT;
typedef DRVCALLBACK MIDICALLBACK;
typedef MIDICALLBACK FAR *LPMIDICALLBACK;
#define MIDIPATCHSIZE   128
typedef WORD PATCHARRAY[MIDIPATCHSIZE];
typedef WORD FAR *LPPATCHARRAY;
typedef WORD KEYARRAY[MIDIPATCHSIZE];
typedef WORD FAR *LPKEYARRAY;

/* MIDI callback messages */
#define MIM_OPEN        MM_MIM_OPEN
#define MIM_CLOSE       MM_MIM_CLOSE
#define MIM_DATA        MM_MIM_DATA
#define MIM_LONGDATA    MM_MIM_LONGDATA
#define MIM_ERROR       MM_MIM_ERROR
#define MIM_LONGERROR   MM_MIM_LONGERROR
#define MOM_OPEN        MM_MOM_OPEN
#define MOM_CLOSE       MM_MOM_CLOSE
#define MOM_DONE        MM_MOM_DONE

/* device ID for MIDI mapper */
#define MIDIMAPPER     (-1)
#define MIDI_MAPPER    (-1)

/* flags for wFlags parm of midiOutCachePatches(), midiOutCacheDrumPatches() */
#define MIDI_CACHE_ALL      1
#define MIDI_CACHE_BESTFIT  2
#define MIDI_CACHE_QUERY    3
#define MIDI_UNCACHE        4

/* MIDI output device capabilities structure */
typedef struct midioutcaps_tag {
    UINT    wMid;                  /* manufacturer ID */
    UINT    wPid;                  /* product ID */
    VERSION vDriverVersion;        /* version of the driver */
    char    szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    UINT    wTechnology;           /* type of device */
    UINT    wVoices;               /* # of voices (internal synth only) */
    UINT    wNotes;                /* max # of notes (internal synth only) */
    UINT    wChannelMask;          /* channels used (internal synth only) */
    DWORD   dwSupport;             /* functionality supported by driver */
} MIDIOUTCAPS;
typedef MIDIOUTCAPS       *PMIDIOUTCAPS;
typedef MIDIOUTCAPS NEAR *NPMIDIOUTCAPS;
typedef MIDIOUTCAPS FAR  *LPMIDIOUTCAPS;

/* flags for wTechnology field of MIDIOUTCAPS structure */
#define MOD_MIDIPORT    1  /* output port */
#define MOD_SYNTH       2  /* generic internal synth */
#define MOD_SQSYNTH     3  /* square wave internal synth */
#define MOD_FMSYNTH     4  /* FM internal synth */
#define MOD_MAPPER      5  /* MIDI mapper */

/* flags for dwSupport field of MIDIOUTCAPS structure */
#define MIDICAPS_VOLUME          0x0001  /* supports volume control */
#define MIDICAPS_LRVOLUME        0x0002  /* separate left-right volume control */
#define MIDICAPS_CACHE           0x0004

/* MIDI output device capabilities structure */
typedef struct midiincaps_tag {
    UINT    wMid;                  /* manufacturer ID */
    UINT    wPid;                  /* product ID */
    VERSION vDriverVersion;        /* version of the driver */
    char    szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
} MIDIINCAPS;
typedef MIDIINCAPS      *PMIDIINCAPS;
typedef MIDIINCAPS NEAR *NPMIDIINCAPS;
typedef MIDIINCAPS FAR  *LPMIDIINCAPS;

/* MIDI data block header */
typedef struct midihdr_tag {
    LPSTR       lpData;               /* pointer to locked data block */
    DWORD       dwBufferLength;       /* length of data in data block */
    DWORD       dwBytesRecorded;      /* used for input only */
    DWORD       dwUser;               /* for client's use */
    DWORD       dwFlags;              /* assorted flags (see defines) */
    struct midihdr_tag far *lpNext;   /* reserved for driver */
    DWORD       reserved;             /* reserved for driver */
} MIDIHDR;
typedef MIDIHDR       *PMIDIHDR;
typedef MIDIHDR NEAR *NPMIDIHDR;
typedef MIDIHDR FAR  *LPMIDIHDR;

/* flags for dwFlags field of MIDIHDR structure */
#define MHDR_DONE       0x00000001       /* done bit */
#define MHDR_PREPARED   0x00000002       /* set if header prepared */
#define MHDR_INQUEUE    0x00000004       /* reserved for driver */

/* MIDI function prototypes */
UINT WINAPI midiOutGetNumDevs(void);
UINT WINAPI midiOutGetDevCaps(UINT uDeviceID,
    MIDIOUTCAPS FAR* lpCaps, UINT uSize);
UINT WINAPI midiOutGetVolume(UINT uDeviceID, DWORD FAR* lpdwVolume);
UINT WINAPI midiOutSetVolume(UINT uDeviceID, DWORD dwVolume);
UINT WINAPI midiOutGetErrorText(UINT uError, LPSTR lpText, UINT uSize);
UINT WINAPI midiOutOpen(HMIDIOUT FAR* lphMidiOut, UINT uDeviceID,
    DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
UINT WINAPI midiOutClose(HMIDIOUT hMidiOut);
UINT WINAPI midiOutPrepareHeader(HMIDIOUT hMidiOut,
    MIDIHDR FAR* lpMidiOutHdr, UINT uSize);
UINT WINAPI midiOutUnprepareHeader(HMIDIOUT hMidiOut,
    MIDIHDR FAR* lpMidiOutHdr, UINT uSize);
UINT WINAPI midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg);
UINT WINAPI midiOutLongMsg(HMIDIOUT hMidiOut,
    MIDIHDR FAR* lpMidiOutHdr, UINT uSize);
UINT WINAPI midiOutReset(HMIDIOUT hMidiOut);
UINT WINAPI midiOutCachePatches(HMIDIOUT hMidiOut,
    UINT uBank, WORD FAR* lpwPatchArray, UINT uFlags);
UINT WINAPI midiOutCacheDrumPatches(HMIDIOUT hMidiOut,
    UINT uPatch, WORD FAR* lpwKeyArray, UINT uFlags);
UINT WINAPI midiOutGetID(HMIDIOUT hMidiOut, UINT FAR* lpuDeviceID);

#if defined(WINVER) && (WINVER >= 0x030a)
DWORD WINAPI midiOutMessage(HMIDIOUT hMidiOut, UINT uMessage, DWORD dw1, DWORD dw2);
#endif  /* WINVER >= 0x030a */

UINT WINAPI midiInGetNumDevs(void);
UINT WINAPI midiInGetDevCaps(UINT uDeviceID,
    LPMIDIINCAPS lpCaps, UINT uSize);
UINT WINAPI midiInGetErrorText(UINT uError, LPSTR lpText, UINT uSize);
UINT WINAPI midiInOpen(HMIDIIN FAR* lphMidiIn, UINT uDeviceID,
    DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
UINT WINAPI midiInClose(HMIDIIN hMidiIn);
UINT WINAPI midiInPrepareHeader(HMIDIIN hMidiIn,
    MIDIHDR FAR* lpMidiInHdr, UINT uSize);
UINT WINAPI midiInUnprepareHeader(HMIDIIN hMidiIn,
    MIDIHDR FAR* lpMidiInHdr, UINT uSize);
UINT WINAPI midiInAddBuffer(HMIDIIN hMidiIn,
    MIDIHDR FAR* lpMidiInHdr, UINT uSize);
UINT WINAPI midiInStart(HMIDIIN hMidiIn);
UINT WINAPI midiInStop(HMIDIIN hMidiIn);
UINT WINAPI midiInReset(HMIDIIN hMidiIn);
UINT WINAPI midiInGetID(HMIDIIN hMidiIn, UINT FAR* lpuDeviceID);

#if defined(WINVER) && (WINVER >= 0x030a)
DWORD WINAPI midiInMessage(HMIDIIN hMidiIn, UINT uMessage, DWORD dw1, DWORD dw2);
#endif  /* WINVER >= 0x030a */

#endif  /* ifndef MMNOMIDI */


#ifndef MMNOAUX
/****************************************************************************

                        Auxiliary audio support

****************************************************************************/

/* device ID for aux device mapper */
#define AUX_MAPPER     (-1)

/* Auxiliary audio device capabilities structure */
typedef struct auxcaps_tag {
    UINT    wMid;                  /* manufacturer ID */
    UINT    wPid;                  /* product ID */
    VERSION vDriverVersion;        /* version of the driver */
    char    szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    UINT    wTechnology;           /* type of device */
    DWORD   dwSupport;             /* functionality supported by driver */
} AUXCAPS;
typedef AUXCAPS       *PAUXCAPS;
typedef AUXCAPS NEAR *NPAUXCAPS;
typedef AUXCAPS FAR  *LPAUXCAPS;

/* flags for wTechnology field in AUXCAPS structure */
#define AUXCAPS_CDAUDIO    1       /* audio from internal CD-ROM drive */
#define AUXCAPS_AUXIN      2       /* audio from auxiliary input jacks */

/* flags for dwSupport field in AUXCAPS structure */
#define AUXCAPS_VOLUME          0x0001  /* supports volume control */
#define AUXCAPS_LRVOLUME        0x0002  /* separate left-right volume control */

/* auxiliary audio function prototypes */
UINT WINAPI auxGetNumDevs(void);
UINT WINAPI auxGetDevCaps(UINT uDeviceID, AUXCAPS FAR* lpCaps, UINT uSize);
UINT WINAPI auxSetVolume(UINT uDeviceID, DWORD dwVolume);
UINT WINAPI auxGetVolume(UINT uDeviceID, DWORD FAR* lpdwVolume);

#if defined(WINVER) && (WINVER >= 0x030a)
DWORD WINAPI auxOutMessage(UINT uDeviceID, UINT uMessage, DWORD dw1, DWORD dw2);
#endif  /* WINVER >= 0x030a */

#endif  /* ifndef MMNOAUX */


#ifndef MMNOTIMER
/****************************************************************************

                            Timer support

****************************************************************************/

/* timer error return values */
#define TIMERR_NOERROR        (0)                  /* no error */
#define TIMERR_NOCANDO        (TIMERR_BASE+1)      /* request not completed */
#define TIMERR_STRUCT         (TIMERR_BASE+33)     /* time struct size */

/* timer data types */
typedef void (CALLBACK TIMECALLBACK) (UINT uTimerID, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef TIMECALLBACK FAR *LPTIMECALLBACK;

/* flags for wFlags parameter of timeSetEvent() function */
#define TIME_ONESHOT    0   /* program timer for single event */
#define TIME_PERIODIC   1   /* program for continuous periodic event */

/* timer device capabilities data structure */
typedef struct timecaps_tag {
    UINT    wPeriodMin;     /* minimum period supported  */
    UINT    wPeriodMax;     /* maximum period supported  */
    } TIMECAPS;
typedef TIMECAPS       *PTIMECAPS;
typedef TIMECAPS NEAR *NPTIMECAPS;
typedef TIMECAPS FAR  *LPTIMECAPS;

/* timer function prototypes */
UINT WINAPI timeGetSystemTime(MMTIME FAR* lpTime, UINT uSize);
DWORD WINAPI timeGetTime(void);
UINT WINAPI timeSetEvent(UINT uDelay, UINT uResolution,
    LPTIMECALLBACK lpFunction, DWORD dwUser, UINT uFlags);
UINT WINAPI timeKillEvent(UINT uTimerID);
UINT WINAPI timeGetDevCaps(TIMECAPS FAR* lpTimeCaps, UINT uSize);
UINT WINAPI timeBeginPeriod(UINT uPeriod);
UINT WINAPI timeEndPeriod(UINT uPeriod);

#endif  /* ifndef MMNOTIMER */


#ifndef MMNOJOY
/****************************************************************************

                            Joystick support

****************************************************************************/

/* joystick error return values */
#define JOYERR_NOERROR        (0)                  /* no error */
#define JOYERR_PARMS          (JOYERR_BASE+5)      /* bad parameters */
#define JOYERR_NOCANDO        (JOYERR_BASE+6)      /* request not completed */
#define JOYERR_UNPLUGGED      (JOYERR_BASE+7)      /* joystick is unplugged */

/* constants used with JOYINFO structure and MM_JOY* messages */
#define JOY_BUTTON1         0x0001
#define JOY_BUTTON2         0x0002
#define JOY_BUTTON3         0x0004
#define JOY_BUTTON4         0x0008
#define JOY_BUTTON1CHG      0x0100
#define JOY_BUTTON2CHG      0x0200
#define JOY_BUTTON3CHG      0x0400
#define JOY_BUTTON4CHG      0x0800

/* joystick ID constants */
#define JOYSTICKID1         0
#define JOYSTICKID2         1

/* joystick device capabilities data structure */
typedef struct joycaps_tag {
    UINT wMid;                  /* manufacturer ID */
    UINT wPid;                  /* product ID */
    char szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    UINT wXmin;                 /* minimum x position value */
    UINT wXmax;                 /* maximum x position value */
    UINT wYmin;                 /* minimum y position value */
    UINT wYmax;                 /* maximum y position value */
    UINT wZmin;                 /* minimum z position value */
    UINT wZmax;                 /* maximum z position value */
    UINT wNumButtons;           /* number of buttons */
    UINT wPeriodMin;            /* minimum message period when captured */
    UINT wPeriodMax;            /* maximum message period when captured */
    } JOYCAPS;
typedef JOYCAPS       *PJOYCAPS;
typedef JOYCAPS NEAR *NPJOYCAPS;
typedef JOYCAPS FAR  *LPJOYCAPS;

/* joystick information data structure */
typedef struct joyinfo_tag {
    UINT wXpos;                 /* x position */
    UINT wYpos;                 /* y position */
    UINT wZpos;                 /* z position */
    UINT wButtons;              /* button states */
    } JOYINFO;
typedef JOYINFO       *PJOYINFO;
typedef JOYINFO NEAR *NPJOYINFO;
typedef JOYINFO FAR  *LPJOYINFO;

/* joystick function prototypes */
UINT WINAPI joyGetDevCaps(UINT uJoyID, JOYCAPS FAR* lpCaps, UINT uSize);
UINT WINAPI joyGetNumDevs(void);
UINT WINAPI joyGetPos(UINT uJoyID, JOYINFO FAR* lpInfo);
UINT WINAPI joyGetThreshold(UINT uJoyID, UINT FAR* lpuThreshold);
UINT WINAPI joyReleaseCapture(UINT uJoyID);
UINT WINAPI joySetCapture(HWND hwnd, UINT uJoyID, UINT uPeriod,
    BOOL bChanged);
UINT WINAPI joySetThreshold(UINT uJoyID, UINT uThreshold);

#endif  /* ifndef MMNOJOY */


#ifndef MMNOMMIO
/****************************************************************************

                        Multimedia File I/O support

****************************************************************************/

/* MMIO error return values */
#define MMIOERR_BASE            256
#define MMIOERR_FILENOTFOUND    (MMIOERR_BASE + 1)  /* file not found */
#define MMIOERR_OUTOFMEMORY     (MMIOERR_BASE + 2)  /* out of memory */
#define MMIOERR_CANNOTOPEN      (MMIOERR_BASE + 3)  /* cannot open */
#define MMIOERR_CANNOTCLOSE     (MMIOERR_BASE + 4)  /* cannot close */
#define MMIOERR_CANNOTREAD      (MMIOERR_BASE + 5)  /* cannot read */
#define MMIOERR_CANNOTWRITE     (MMIOERR_BASE + 6)  /* cannot write */
#define MMIOERR_CANNOTSEEK      (MMIOERR_BASE + 7)  /* cannot seek */
#define MMIOERR_CANNOTEXPAND    (MMIOERR_BASE + 8)  /* cannot expand file */
#define MMIOERR_CHUNKNOTFOUND   (MMIOERR_BASE + 9)  /* chunk not found */
#define MMIOERR_UNBUFFERED      (MMIOERR_BASE + 10) /* file is unbuffered */

/* MMIO constants */
#define CFSEPCHAR       '+'             /* compound file name separator char. */

/* MMIO data types */
typedef DWORD           FOURCC;         /* a four character code */
typedef char _huge *    HPSTR;          /* a huge version of LPSTR */
DECLARE_HANDLE(HMMIO);                  /* a handle to an open file */
typedef LRESULT (CALLBACK MMIOPROC)(LPSTR lpmmioinfo, UINT uMessage,
            LPARAM lParam1, LPARAM lParam2);
typedef MMIOPROC FAR *LPMMIOPROC;

/* general MMIO information data structure */
typedef struct _MMIOINFO
{
        /* general fields */
        DWORD           dwFlags;        /* general status flags */
        FOURCC          fccIOProc;      /* pointer to I/O procedure */
        LPMMIOPROC      pIOProc;        /* pointer to I/O procedure */
        UINT            wErrorRet;      /* place for error to be returned */
        HTASK           htask;          /* alternate local task */

        /* fields maintained by MMIO functions during buffered I/O */
        LONG            cchBuffer;      /* size of I/O buffer (or 0L) */
        HPSTR           pchBuffer;      /* start of I/O buffer (or NULL) */
        HPSTR           pchNext;        /* pointer to next byte to read/write */
        HPSTR           pchEndRead;     /* pointer to last valid byte to read */
        HPSTR           pchEndWrite;    /* pointer to last byte to write */
        LONG            lBufOffset;     /* disk offset of start of buffer */

        /* fields maintained by I/O procedure */
        LONG            lDiskOffset;    /* disk offset of next read or write */
        DWORD           adwInfo[3];     /* data specific to type of MMIOPROC */

        /* other fields maintained by MMIO */
        DWORD           dwReserved1;    /* reserved for MMIO use */
        DWORD           dwReserved2;    /* reserved for MMIO use */
        HMMIO           hmmio;          /* handle to open file */
} MMIOINFO;
typedef MMIOINFO       *PMMIOINFO;
typedef MMIOINFO NEAR *NPMMIOINFO;
typedef MMIOINFO FAR  *LPMMIOINFO;

/* RIFF chunk information data structure */
typedef struct _MMCKINFO
{
        FOURCC          ckid;           /* chunk ID */
        DWORD           cksize;         /* chunk size */
        FOURCC          fccType;        /* form type or list type */
        DWORD           dwDataOffset;   /* offset of data portion of chunk */
        DWORD           dwFlags;        /* flags used by MMIO functions */
} MMCKINFO;
typedef MMCKINFO       *PMMCKINFO;
typedef MMCKINFO NEAR *NPMMCKINFO;
typedef MMCKINFO FAR  *LPMMCKINFO;

/* bit field masks */
#define MMIO_RWMODE     0x00000003      /* open file for reading/writing/both */
#define MMIO_SHAREMODE  0x00000070      /* file sharing mode number */

/* constants for dwFlags field of MMIOINFO */
#define MMIO_CREATE     0x00001000      /* create new file (or truncate file) */
#define MMIO_PARSE      0x00000100      /* parse new file returning path */
#define MMIO_DELETE     0x00000200      /* create new file (or truncate file) */
#define MMIO_EXIST      0x00004000      /* checks for existence of file */
#define MMIO_ALLOCBUF   0x00010000      /* mmioOpen() should allocate a buffer */
#define MMIO_GETTEMP    0x00020000      /* mmioOpen() should retrieve temp name */

#define MMIO_DIRTY      0x10000000      /* I/O buffer is dirty */


/* read/write mode numbers (bit field MMIO_RWMODE) */
#define MMIO_READ       0x00000000      /* open file for reading only */
#define MMIO_WRITE      0x00000001      /* open file for writing only */
#define MMIO_READWRITE  0x00000002      /* open file for reading and writing */

/* share mode numbers (bit field MMIO_SHAREMODE) */
#define MMIO_COMPAT     0x00000000      /* compatibility mode */
#define MMIO_EXCLUSIVE  0x00000010      /* exclusive-access mode */
#define MMIO_DENYWRITE  0x00000020      /* deny writing to other processes */
#define MMIO_DENYREAD   0x00000030      /* deny reading to other processes */
#define MMIO_DENYNONE   0x00000040      /* deny nothing to other processes */

/* various MMIO flags */
#define MMIO_FHOPEN             0x0010  /* mmioClose: keep file handle open */
#define MMIO_EMPTYBUF           0x0010  /* mmioFlush: empty the I/O buffer */
#define MMIO_TOUPPER            0x0010  /* mmioStringToFOURCC: to u-case */
#define MMIO_INSTALLPROC    0x00010000  /* mmioInstallIOProc: install MMIOProc */
#define MMIO_GLOBALPROC     0x10000000  /* mmioInstallIOProc: install globally */
#define MMIO_REMOVEPROC     0x00020000  /* mmioInstallIOProc: remove MMIOProc */
#define MMIO_FINDPROC       0x00040000  /* mmioInstallIOProc: find an MMIOProc */
#define MMIO_FINDCHUNK          0x0010  /* mmioDescend: find a chunk by ID */
#define MMIO_FINDRIFF           0x0020  /* mmioDescend: find a LIST chunk */
#define MMIO_FINDLIST           0x0040  /* mmioDescend: find a RIFF chunk */
#define MMIO_CREATERIFF         0x0020  /* mmioCreateChunk: make a LIST chunk */
#define MMIO_CREATELIST         0x0040  /* mmioCreateChunk: make a RIFF chunk */


/* message numbers for MMIOPROC I/O procedure functions */
#define MMIOM_READ      MMIO_READ       /* read */
#define MMIOM_WRITE    MMIO_WRITE       /* write */
#define MMIOM_SEEK              2       /* seek to a new position in file */
#define MMIOM_OPEN              3       /* open file */
#define MMIOM_CLOSE             4       /* close file */
#define MMIOM_WRITEFLUSH        5       /* write and flush */

#if defined(WINVER) && (WINVER >= 0x030a)
#define MMIOM_RENAME            6       /* rename specified file */
#endif  /* WINVER >= 0x030a */

#define MMIOM_USER         0x8000       /* beginning of user-defined messages */

/* standard four character codes */
#define FOURCC_RIFF     mmioFOURCC('R', 'I', 'F', 'F')
#define FOURCC_LIST     mmioFOURCC('L', 'I', 'S', 'T')

/* four character codes used to identify standard built-in I/O procedures */
#define FOURCC_DOS      mmioFOURCC('D', 'O', 'S', ' ')
#define FOURCC_MEM      mmioFOURCC('M', 'E', 'M', ' ')

/* flags for mmioSeek() */
#ifndef SEEK_SET
#define SEEK_SET        0               /* seek to an absolute position */
#define SEEK_CUR        1               /* seek relative to current position */
#define SEEK_END        2               /* seek relative to end of file */
#endif  /* ifndef SEEK_SET */

/* other constants */
#define MMIO_DEFAULTBUFFER      8192    /* default buffer size */

/* MMIO macros */
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

/* MMIO function prototypes */
FOURCC WINAPI mmioStringToFOURCC(LPCSTR sz, UINT uFlags);
LPMMIOPROC WINAPI mmioInstallIOProc(FOURCC fccIOProc, LPMMIOPROC pIOProc,
    DWORD dwFlags);
HMMIO WINAPI mmioOpen(LPSTR szFileName, MMIOINFO FAR* lpmmioinfo,
    DWORD dwOpenFlags);

#if defined(WINVER) && (WINVER >= 0x030a)
UINT WINAPI mmioRename(LPCSTR szFileName, LPCSTR szNewFileName,
     MMIOINFO FAR* lpmmioinfo, DWORD dwRenameFlags);
#endif  /* WINVER >= 0x030a */

UINT WINAPI mmioClose(HMMIO hmmio, UINT uFlags);
LONG WINAPI mmioRead(HMMIO hmmio, HPSTR pch, LONG cch);
LONG WINAPI mmioWrite(HMMIO hmmio, const char _huge* pch, LONG cch);
LONG WINAPI mmioSeek(HMMIO hmmio, LONG lOffset, int iOrigin);
UINT WINAPI mmioGetInfo(HMMIO hmmio, MMIOINFO FAR* lpmmioinfo, UINT uFlags);
UINT WINAPI mmioSetInfo(HMMIO hmmio, const MMIOINFO FAR* lpmmioinfo, UINT uFlags);
UINT WINAPI mmioSetBuffer(HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer,
    UINT uFlags);
UINT WINAPI mmioFlush(HMMIO hmmio, UINT uFlags);
UINT WINAPI mmioAdvance(HMMIO hmmio, MMIOINFO FAR* lpmmioinfo, UINT uFlags);
LRESULT WINAPI mmioSendMessage(HMMIO hmmio, UINT uMessage,
    LPARAM lParam1, LPARAM lParam2);
UINT WINAPI mmioDescend(HMMIO hmmio, MMCKINFO FAR* lpck,
    const MMCKINFO FAR* lpckParent, UINT uFlags);
UINT WINAPI mmioAscend(HMMIO hmmio, MMCKINFO FAR* lpck, UINT uFlags);
UINT WINAPI mmioCreateChunk(HMMIO hmmio, MMCKINFO FAR* lpck, UINT uFlags);

#endif  /* ifndef MMNOMMIO */


#ifndef MMNOMCI
/****************************************************************************

                            MCI support

****************************************************************************/

typedef UINT (CALLBACK *YIELDPROC) (UINT uDeviceID, DWORD dwYieldData);

/* MCI function prototypes */
DWORD WINAPI mciSendCommand (UINT uDeviceID, UINT uMessage,
    DWORD dwParam1, DWORD dwParam2);
DWORD WINAPI mciSendString (LPCSTR lpstrCommand,
    LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);
UINT WINAPI mciGetDeviceID (LPCSTR lpstrName);
UINT WINAPI mciGetDeviceIDFromElementID (DWORD dwElementID,
    LPCSTR lpstrType);
BOOL WINAPI mciGetErrorString (DWORD wError, LPSTR lpstrBuffer,
    UINT uLength);
BOOL WINAPI mciSetYieldProc (UINT uDeviceID, YIELDPROC fpYieldProc,
    DWORD dwYieldData);

#if defined(WINVER) && (WINVER >= 0x030a)
HTASK WINAPI mciGetCreatorTask(UINT uDeviceID);
YIELDPROC WINAPI mciGetYieldProc (UINT uDeviceID, DWORD FAR* lpdwYieldData);
#endif  /* WINVER >= 0x030a */

/* If included with the 3.0 windows.h */
#if !defined(WINVER) || (WINVER < 0x030a)
BOOL WINAPI mciExecute (LPCSTR lpstrCommand);
#endif  /* WIN3.0 */

/* MCI error return values */
#define MCIERR_INVALID_DEVICE_ID        (MCIERR_BASE + 1)
#define MCIERR_UNRECOGNIZED_KEYWORD     (MCIERR_BASE + 3)
#define MCIERR_UNRECOGNIZED_COMMAND     (MCIERR_BASE + 5)
#define MCIERR_HARDWARE                 (MCIERR_BASE + 6)
#define MCIERR_INVALID_DEVICE_NAME      (MCIERR_BASE + 7)
#define MCIERR_OUT_OF_MEMORY            (MCIERR_BASE + 8)
#define MCIERR_DEVICE_OPEN              (MCIERR_BASE + 9)
#define MCIERR_CANNOT_LOAD_DRIVER       (MCIERR_BASE + 10)
#define MCIERR_MISSING_COMMAND_STRING   (MCIERR_BASE + 11)
#define MCIERR_PARAM_OVERFLOW           (MCIERR_BASE + 12)
#define MCIERR_MISSING_STRING_ARGUMENT  (MCIERR_BASE + 13)
#define MCIERR_BAD_INTEGER              (MCIERR_BASE + 14)
#define MCIERR_PARSER_INTERNAL          (MCIERR_BASE + 15)
#define MCIERR_DRIVER_INTERNAL          (MCIERR_BASE + 16)
#define MCIERR_MISSING_PARAMETER        (MCIERR_BASE + 17)
#define MCIERR_UNSUPPORTED_FUNCTION     (MCIERR_BASE + 18)
#define MCIERR_FILE_NOT_FOUND           (MCIERR_BASE + 19)
#define MCIERR_DEVICE_NOT_READY         (MCIERR_BASE + 20)
#define MCIERR_INTERNAL                 (MCIERR_BASE + 21)
#define MCIERR_DRIVER                   (MCIERR_BASE + 22)
#define MCIERR_CANNOT_USE_ALL           (MCIERR_BASE + 23)
#define MCIERR_MULTIPLE                 (MCIERR_BASE + 24)
#define MCIERR_EXTENSION_NOT_FOUND      (MCIERR_BASE + 25)
#define MCIERR_OUTOFRANGE               (MCIERR_BASE + 26)
#define MCIERR_FLAGS_NOT_COMPATIBLE     (MCIERR_BASE + 28)
#define MCIERR_FILE_NOT_SAVED           (MCIERR_BASE + 30)
#define MCIERR_DEVICE_TYPE_REQUIRED     (MCIERR_BASE + 31)
#define MCIERR_DEVICE_LOCKED            (MCIERR_BASE + 32)
#define MCIERR_DUPLICATE_ALIAS          (MCIERR_BASE + 33)
#define MCIERR_BAD_CONSTANT             (MCIERR_BASE + 34)
#define MCIERR_MUST_USE_SHAREABLE       (MCIERR_BASE + 35)
#define MCIERR_MISSING_DEVICE_NAME      (MCIERR_BASE + 36)
#define MCIERR_BAD_TIME_FORMAT          (MCIERR_BASE + 37)
#define MCIERR_NO_CLOSING_QUOTE         (MCIERR_BASE + 38)
#define MCIERR_DUPLICATE_FLAGS          (MCIERR_BASE + 39)
#define MCIERR_INVALID_FILE             (MCIERR_BASE + 40)
#define MCIERR_NULL_PARAMETER_BLOCK     (MCIERR_BASE + 41)
#define MCIERR_UNNAMED_RESOURCE         (MCIERR_BASE + 42)
#define MCIERR_NEW_REQUIRES_ALIAS       (MCIERR_BASE + 43)
#define MCIERR_NOTIFY_ON_AUTO_OPEN      (MCIERR_BASE + 44)
#define MCIERR_NO_ELEMENT_ALLOWED       (MCIERR_BASE + 45)
#define MCIERR_NONAPPLICABLE_FUNCTION   (MCIERR_BASE + 46)
#define MCIERR_ILLEGAL_FOR_AUTO_OPEN    (MCIERR_BASE + 47)
#define MCIERR_FILENAME_REQUIRED        (MCIERR_BASE + 48)
#define MCIERR_EXTRA_CHARACTERS         (MCIERR_BASE + 49)
#define MCIERR_DEVICE_NOT_INSTALLED     (MCIERR_BASE + 50)
#define MCIERR_GET_CD                   (MCIERR_BASE + 51)
#define MCIERR_SET_CD                   (MCIERR_BASE + 52)
#define MCIERR_SET_DRIVE                (MCIERR_BASE + 53)
#define MCIERR_DEVICE_LENGTH            (MCIERR_BASE + 54)
#define MCIERR_DEVICE_ORD_LENGTH        (MCIERR_BASE + 55)
#define MCIERR_NO_INTEGER               (MCIERR_BASE + 56)

#define MCIERR_WAVE_OUTPUTSINUSE        (MCIERR_BASE + 64)
#define MCIERR_WAVE_SETOUTPUTINUSE      (MCIERR_BASE + 65)
#define MCIERR_WAVE_INPUTSINUSE         (MCIERR_BASE + 66)
#define MCIERR_WAVE_SETINPUTINUSE       (MCIERR_BASE + 67)
#define MCIERR_WAVE_OUTPUTUNSPECIFIED   (MCIERR_BASE + 68)
#define MCIERR_WAVE_INPUTUNSPECIFIED    (MCIERR_BASE + 69)
#define MCIERR_WAVE_OUTPUTSUNSUITABLE   (MCIERR_BASE + 70)
#define MCIERR_WAVE_SETOUTPUTUNSUITABLE (MCIERR_BASE + 71)
#define MCIERR_WAVE_INPUTSUNSUITABLE    (MCIERR_BASE + 72)
#define MCIERR_WAVE_SETINPUTUNSUITABLE  (MCIERR_BASE + 73)

#define MCIERR_SEQ_DIV_INCOMPATIBLE     (MCIERR_BASE + 80)
#define MCIERR_SEQ_PORT_INUSE           (MCIERR_BASE + 81)
#define MCIERR_SEQ_PORT_NONEXISTENT     (MCIERR_BASE + 82)
#define MCIERR_SEQ_PORT_MAPNODEVICE     (MCIERR_BASE + 83)
#define MCIERR_SEQ_PORT_MISCERROR       (MCIERR_BASE + 84)
#define MCIERR_SEQ_TIMER                (MCIERR_BASE + 85)
#define MCIERR_SEQ_PORTUNSPECIFIED      (MCIERR_BASE + 86)
#define MCIERR_SEQ_NOMIDIPRESENT        (MCIERR_BASE + 87)

#define MCIERR_NO_WINDOW                (MCIERR_BASE + 90)
#define MCIERR_CREATEWINDOW             (MCIERR_BASE + 91)
#define MCIERR_FILE_READ                (MCIERR_BASE + 92)
#define MCIERR_FILE_WRITE               (MCIERR_BASE + 93)

/* all custom device driver errors must be >= than this value */
#define MCIERR_CUSTOM_DRIVER_BASE       (MCIERR_BASE + 256)

/* MCI command message identifiers */
#define MCI_OPEN                        0x0803
#define MCI_CLOSE                       0x0804
#define MCI_ESCAPE                      0x0805
#define MCI_PLAY                        0x0806
#define MCI_SEEK                        0x0807
#define MCI_STOP                        0x0808
#define MCI_PAUSE                       0x0809
#define MCI_INFO                        0x080A
#define MCI_GETDEVCAPS                  0x080B
#define MCI_SPIN                        0x080C
#define MCI_SET                         0x080D
#define MCI_STEP                        0x080E
#define MCI_RECORD                      0x080F
#define MCI_SYSINFO                     0x0810
#define MCI_BREAK                       0x0811
#define MCI_SOUND                       0x0812
#define MCI_SAVE                        0x0813
#define MCI_STATUS                      0x0814
#define MCI_CUE                         0x0830
#define MCI_REALIZE                     0x0840
#define MCI_WINDOW                      0x0841
#define MCI_PUT                         0x0842
#define MCI_WHERE                       0x0843
#define MCI_FREEZE                      0x0844
#define MCI_UNFREEZE                    0x0845
#define MCI_LOAD                        0x0850
#define MCI_CUT                         0x0851
#define MCI_COPY                        0x0852
#define MCI_PASTE                       0x0853
#define MCI_UPDATE                      0x0854
#define MCI_RESUME                      0x0855
#define MCI_DELETE                      0x0856

/* all custom MCI command messages must be >= than this value */
#define MCI_USER_MESSAGES               (0x400 + DRV_MCI_FIRST)


/* device ID for "all devices" */
#define MCI_ALL_DEVICE_ID               0xFFFF

/* constants for predefined MCI device types */
#define MCI_DEVTYPE_VCR                 (MCI_STRING_OFFSET + 1)
#define MCI_DEVTYPE_VIDEODISC           (MCI_STRING_OFFSET + 2)
#define MCI_DEVTYPE_OVERLAY             (MCI_STRING_OFFSET + 3)
#define MCI_DEVTYPE_CD_AUDIO            (MCI_STRING_OFFSET + 4)
#define MCI_DEVTYPE_DAT                 (MCI_STRING_OFFSET + 5)
#define MCI_DEVTYPE_SCANNER             (MCI_STRING_OFFSET + 6)
#define MCI_DEVTYPE_ANIMATION           (MCI_STRING_OFFSET + 7)
#define MCI_DEVTYPE_DIGITAL_VIDEO       (MCI_STRING_OFFSET + 8)
#define MCI_DEVTYPE_OTHER               (MCI_STRING_OFFSET + 9)
#define MCI_DEVTYPE_WAVEFORM_AUDIO      (MCI_STRING_OFFSET + 10)
#define MCI_DEVTYPE_SEQUENCER           (MCI_STRING_OFFSET + 11)

#define MCI_DEVTYPE_FIRST               MCI_DEVTYPE_VCR
#define MCI_DEVTYPE_LAST                MCI_DEVTYPE_SEQUENCER

/* return values for 'status mode' command */
#define MCI_MODE_NOT_READY              (MCI_STRING_OFFSET + 12)
#define MCI_MODE_STOP                   (MCI_STRING_OFFSET + 13)
#define MCI_MODE_PLAY                   (MCI_STRING_OFFSET + 14)
#define MCI_MODE_RECORD                 (MCI_STRING_OFFSET + 15)
#define MCI_MODE_SEEK                   (MCI_STRING_OFFSET + 16)
#define MCI_MODE_PAUSE                  (MCI_STRING_OFFSET + 17)
#define MCI_MODE_OPEN                   (MCI_STRING_OFFSET + 18)

/* constants used in 'set time format' and 'status time format' commands */
#define MCI_FORMAT_MILLISECONDS         0
#define MCI_FORMAT_HMS                  1
#define MCI_FORMAT_MSF                  2
#define MCI_FORMAT_FRAMES               3
#define MCI_FORMAT_SMPTE_24             4
#define MCI_FORMAT_SMPTE_25             5
#define MCI_FORMAT_SMPTE_30             6
#define MCI_FORMAT_SMPTE_30DROP         7
#define MCI_FORMAT_BYTES                8
#define MCI_FORMAT_SAMPLES              9
#define MCI_FORMAT_TMSF                 10

/* MCI time format conversion macros */
#define MCI_MSF_MINUTE(msf)             ((BYTE)(msf))
#define MCI_MSF_SECOND(msf)             ((BYTE)(((WORD)(msf)) >> 8))
#define MCI_MSF_FRAME(msf)              ((BYTE)((msf)>>16))

#define MCI_MAKE_MSF(m, s, f)           ((DWORD)(((BYTE)(m) | \
                                                  ((WORD)(s)<<8)) | \
                                                 (((DWORD)(BYTE)(f))<<16)))

#define MCI_TMSF_TRACK(tmsf)            ((BYTE)(tmsf))
#define MCI_TMSF_MINUTE(tmsf)           ((BYTE)(((WORD)(tmsf)) >> 8))
#define MCI_TMSF_SECOND(tmsf)           ((BYTE)((tmsf)>>16))
#define MCI_TMSF_FRAME(tmsf)            ((BYTE)((tmsf)>>24))

#define MCI_MAKE_TMSF(t, m, s, f)       ((DWORD)(((BYTE)(t) | \
                                                  ((WORD)(m)<<8)) | \
                                                 (((DWORD)(BYTE)(s) | \
                                                   ((WORD)(f)<<8))<<16)))

#define MCI_HMS_HOUR(hms)               ((BYTE)(hms))
#define MCI_HMS_MINUTE(hms)             ((BYTE)(((WORD)(hms)) >> 8))
#define MCI_HMS_SECOND(hms)             ((BYTE)((hms)>>16))

#define MCI_MAKE_HMS(h, m, s)           ((DWORD)(((BYTE)(h) | \
                                                  ((WORD)(m)<<8)) | \
                                                 (((DWORD)(BYTE)(s))<<16)))


/* flags for wParam of MM_MCINOTIFY message */
#define MCI_NOTIFY_SUCCESSFUL           0x0001
#define MCI_NOTIFY_SUPERSEDED           0x0002
#define MCI_NOTIFY_ABORTED              0x0004
#define MCI_NOTIFY_FAILURE              0x0008


/* common flags for dwFlags parameter of MCI command messages */
#define MCI_NOTIFY                      0x00000001L
#define MCI_WAIT                        0x00000002L
#define MCI_FROM                        0x00000004L
#define MCI_TO                          0x00000008L
#define MCI_TRACK                       0x00000010L

/* flags for dwFlags parameter of MCI_OPEN command message */
#define MCI_OPEN_SHAREABLE              0x00000100L
#define MCI_OPEN_ELEMENT                0x00000200L
#define MCI_OPEN_ALIAS                  0x00000400L
#define MCI_OPEN_ELEMENT_ID             0x00000800L
#define MCI_OPEN_TYPE_ID                0x00001000L
#define MCI_OPEN_TYPE                   0x00002000L

/* flags for dwFlags parameter of MCI_SEEK command message */
#define MCI_SEEK_TO_START               0x00000100L
#define MCI_SEEK_TO_END                 0x00000200L

/* flags for dwFlags parameter of MCI_STATUS command message */
#define MCI_STATUS_ITEM                 0x00000100L
#define MCI_STATUS_START                0x00000200L

/* flags for dwItem field of the MCI_STATUS_PARMS parameter block */
#define MCI_STATUS_LENGTH               0x00000001L
#define MCI_STATUS_POSITION             0x00000002L
#define MCI_STATUS_NUMBER_OF_TRACKS     0x00000003L
#define MCI_STATUS_MODE                 0x00000004L
#define MCI_STATUS_MEDIA_PRESENT        0x00000005L
#define MCI_STATUS_TIME_FORMAT          0x00000006L
#define MCI_STATUS_READY                0x00000007L
#define MCI_STATUS_CURRENT_TRACK        0x00000008L

/* flags for dwFlags parameter of MCI_INFO command message */
#define MCI_INFO_PRODUCT                0x00000100L
#define MCI_INFO_FILE                   0x00000200L

/* flags for dwFlags parameter of MCI_GETDEVCAPS command message */
#define MCI_GETDEVCAPS_ITEM             0x00000100L

/* flags for dwItem field of the MCI_GETDEVCAPS_PARMS parameter block */
#define MCI_GETDEVCAPS_CAN_RECORD       0x00000001L
#define MCI_GETDEVCAPS_HAS_AUDIO        0x00000002L
#define MCI_GETDEVCAPS_HAS_VIDEO        0x00000003L
#define MCI_GETDEVCAPS_DEVICE_TYPE      0x00000004L
#define MCI_GETDEVCAPS_USES_FILES       0x00000005L
#define MCI_GETDEVCAPS_COMPOUND_DEVICE  0x00000006L
#define MCI_GETDEVCAPS_CAN_EJECT        0x00000007L
#define MCI_GETDEVCAPS_CAN_PLAY         0x00000008L
#define MCI_GETDEVCAPS_CAN_SAVE         0x00000009L

/* flags for dwFlags parameter of MCI_SYSINFO command message */
#define MCI_SYSINFO_QUANTITY            0x00000100L
#define MCI_SYSINFO_OPEN                0x00000200L
#define MCI_SYSINFO_NAME                0x00000400L
#define MCI_SYSINFO_INSTALLNAME         0x00000800L

/* flags for dwFlags parameter of MCI_SET command message */
#define MCI_SET_DOOR_OPEN               0x00000100L
#define MCI_SET_DOOR_CLOSED             0x00000200L
#define MCI_SET_TIME_FORMAT             0x00000400L
#define MCI_SET_AUDIO                   0x00000800L
#define MCI_SET_VIDEO                   0x00001000L
#define MCI_SET_ON                      0x00002000L
#define MCI_SET_OFF                     0x00004000L

/* flags for dwAudio field of MCI_SET_PARMS or MCI_SEQ_SET_PARMS */
#define MCI_SET_AUDIO_ALL               0x00000000L
#define MCI_SET_AUDIO_LEFT              0x00000001L
#define MCI_SET_AUDIO_RIGHT             0x00000002L

/* flags for dwFlags parameter of MCI_BREAK command message */
#define MCI_BREAK_KEY                   0x00000100L
#define MCI_BREAK_HWND                  0x00000200L
#define MCI_BREAK_OFF                   0x00000400L

/* flags for dwFlags parameter of MCI_RECORD command message */
#define MCI_RECORD_INSERT               0x00000100L
#define MCI_RECORD_OVERWRITE            0x00000200L

/* flags for dwFlags parameter of MCI_SOUND command message */
#define MCI_SOUND_NAME                  0x00000100L

/* flags for dwFlags parameter of MCI_SAVE command message */
#define MCI_SAVE_FILE                   0x00000100L

/* flags for dwFlags parameter of MCI_LOAD command message */
#define MCI_LOAD_FILE                   0x00000100L

/* generic parameter block for MCI command messages with no special parameters */
typedef struct tagMCI_GENERIC_PARMS {
    DWORD   dwCallback;
} MCI_GENERIC_PARMS;
typedef MCI_GENERIC_PARMS FAR *LPMCI_GENERIC_PARMS;

/* parameter block for MCI_OPEN command message */
typedef struct tagMCI_OPEN_PARMS {
    DWORD   dwCallback;
    UINT    wDeviceID;
    UINT    wReserved0;
    LPCSTR  lpstrDeviceType;
    LPCSTR  lpstrElementName;
    LPCSTR  lpstrAlias;
} MCI_OPEN_PARMS;
typedef MCI_OPEN_PARMS FAR *LPMCI_OPEN_PARMS;

/* parameter block for MCI_PLAY command message */
typedef struct tagMCI_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_PLAY_PARMS;
typedef MCI_PLAY_PARMS FAR *LPMCI_PLAY_PARMS;

/* parameter block for MCI_SEEK command message */
typedef struct tagMCI_SEEK_PARMS {
    DWORD   dwCallback;
    DWORD   dwTo;
} MCI_SEEK_PARMS;
typedef MCI_SEEK_PARMS FAR *LPMCI_SEEK_PARMS;

/* parameter block for MCI_STATUS command message */
typedef struct tagMCI_STATUS_PARMS {
    DWORD   dwCallback;
    DWORD   dwReturn;
    DWORD   dwItem;
    DWORD   dwTrack;
} MCI_STATUS_PARMS;
typedef MCI_STATUS_PARMS FAR * LPMCI_STATUS_PARMS;

/* parameter block for MCI_INFO command message */
typedef struct tagMCI_INFO_PARMS {
    DWORD   dwCallback;
    LPSTR   lpstrReturn;
    DWORD   dwRetSize;
} MCI_INFO_PARMS;
typedef MCI_INFO_PARMS FAR * LPMCI_INFO_PARMS;

/* parameter block for MCI_GETDEVCAPS command message */
typedef struct tagMCI_GETDEVCAPS_PARMS {
    DWORD   dwCallback;
    DWORD   dwReturn;
    DWORD   dwItem;
} MCI_GETDEVCAPS_PARMS;
typedef MCI_GETDEVCAPS_PARMS FAR * LPMCI_GETDEVCAPS_PARMS;

/* parameter block for MCI_SYSINFO command message */
typedef struct tagMCI_SYSINFO_PARMS {
    DWORD   dwCallback;
    LPSTR   lpstrReturn;
    DWORD   dwRetSize;
    DWORD   dwNumber;
    UINT    wDeviceType;
    UINT    wReserved0;
} MCI_SYSINFO_PARMS;
typedef MCI_SYSINFO_PARMS FAR * LPMCI_SYSINFO_PARMS;

/* parameter block for MCI_SET command message */
typedef struct tagMCI_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
} MCI_SET_PARMS;
typedef MCI_SET_PARMS FAR *LPMCI_SET_PARMS;

/* parameter block for MCI_BREAK command message */
typedef struct tagMCI_BREAK_PARMS {
    DWORD   dwCallback;
    int     nVirtKey;
    UINT    wReserved0;
    HWND    hwndBreak;
    UINT    wReserved1;
} MCI_BREAK_PARMS;
typedef MCI_BREAK_PARMS FAR * LPMCI_BREAK_PARMS;

/* parameter block for MCI_SOUND command message */
typedef struct tagMCI_SOUND_PARMS {
    DWORD   dwCallback;
    LPCSTR  lpstrSoundName;
} MCI_SOUND_PARMS;
typedef MCI_SOUND_PARMS FAR * LPMCI_SOUND_PARMS;

/* parameter block for MCI_SAVE command message */
typedef struct tagMCI_SAVE_PARMS {
    DWORD   dwCallback;
    LPCSTR  lpfilename;
} MCI_SAVE_PARMS;
typedef MCI_SAVE_PARMS FAR * LPMCI_SAVE_PARMS;

/* parameter block for MCI_LOAD command message */
typedef struct tagMCI_LOAD_PARMS {
    DWORD   dwCallback;
    LPCSTR  lpfilename;
} MCI_LOAD_PARMS;
typedef MCI_LOAD_PARMS FAR * LPMCI_LOAD_PARMS;

/* parameter block for MCI_RECORD command message */
typedef struct tagMCI_RECORD_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_RECORD_PARMS;
typedef MCI_RECORD_PARMS FAR *LPMCI_RECORD_PARMS;


/* MCI extensions for videodisc devices */

/* flag for dwReturn field of MCI_STATUS_PARMS */
/* MCI_STATUS command, (dwItem == MCI_STATUS_MODE) */
#define MCI_VD_MODE_PARK                (MCI_VD_OFFSET + 1)

/* flag for dwReturn field of MCI_STATUS_PARMS */
/* MCI_STATUS command, (dwItem == MCI_VD_STATUS_MEDIA_TYPE) */
#define MCI_VD_MEDIA_CLV                (MCI_VD_OFFSET + 2)
#define MCI_VD_MEDIA_CAV                (MCI_VD_OFFSET + 3)
#define MCI_VD_MEDIA_OTHER              (MCI_VD_OFFSET + 4)

#define MCI_VD_FORMAT_TRACK             0x4001

/* flags for dwFlags parameter of MCI_PLAY command message */
#define MCI_VD_PLAY_REVERSE             0x00010000L
#define MCI_VD_PLAY_FAST                0x00020000L
#define MCI_VD_PLAY_SPEED               0x00040000L
#define MCI_VD_PLAY_SCAN                0x00080000L
#define MCI_VD_PLAY_SLOW                0x00100000L

/* flag for dwFlags parameter of MCI_SEEK command message */
#define MCI_VD_SEEK_REVERSE             0x00010000L

/* flags for dwItem field of MCI_STATUS_PARMS parameter block */
#define MCI_VD_STATUS_SPEED             0x00004002L
#define MCI_VD_STATUS_FORWARD           0x00004003L
#define MCI_VD_STATUS_MEDIA_TYPE        0x00004004L
#define MCI_VD_STATUS_SIDE              0x00004005L
#define MCI_VD_STATUS_DISC_SIZE         0x00004006L

/* flags for dwFlags parameter of MCI_GETDEVCAPS command message */
#define MCI_VD_GETDEVCAPS_CLV           0x00010000L
#define MCI_VD_GETDEVCAPS_CAV           0x00020000L

#define MCI_VD_SPIN_UP                  0x00010000L
#define MCI_VD_SPIN_DOWN                0x00020000L

/* flags for dwItem field of MCI_GETDEVCAPS_PARMS parameter block */
#define MCI_VD_GETDEVCAPS_CAN_REVERSE   0x00004002L
#define MCI_VD_GETDEVCAPS_FAST_RATE     0x00004003L
#define MCI_VD_GETDEVCAPS_SLOW_RATE     0x00004004L
#define MCI_VD_GETDEVCAPS_NORMAL_RATE   0x00004005L

/* flags for the dwFlags parameter of MCI_STEP command message */
#define MCI_VD_STEP_FRAMES              0x00010000L
#define MCI_VD_STEP_REVERSE             0x00020000L

/* flag for the MCI_ESCAPE command message */
#define MCI_VD_ESCAPE_STRING            0x00000100L

/* parameter block for MCI_PLAY command message */
typedef struct tagMCI_VD_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
    DWORD   dwSpeed;
    } MCI_VD_PLAY_PARMS;
typedef MCI_VD_PLAY_PARMS FAR *LPMCI_VD_PLAY_PARMS;

/* parameter block for MCI_STEP command message */
typedef struct tagMCI_VD_STEP_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrames;
} MCI_VD_STEP_PARMS;
typedef MCI_VD_STEP_PARMS FAR *LPMCI_VD_STEP_PARMS;

/* parameter block for MCI_ESCAPE command message */
typedef struct tagMCI_VD_ESCAPE_PARMS {
    DWORD   dwCallback;
    LPCSTR  lpstrCommand;
} MCI_VD_ESCAPE_PARMS;
typedef MCI_VD_ESCAPE_PARMS FAR *LPMCI_VD_ESCAPE_PARMS;


/* MCI extensions for waveform audio devices */

/* flags for the dwFlags parameter of MCI_OPEN command message */
#define MCI_WAVE_OPEN_BUFFER            0x00010000L

/* flags for the dwFlags parameter of MCI_SET command message */
#define MCI_WAVE_SET_FORMATTAG          0x00010000L
#define MCI_WAVE_SET_CHANNELS           0x00020000L
#define MCI_WAVE_SET_SAMPLESPERSEC      0x00040000L
#define MCI_WAVE_SET_AVGBYTESPERSEC     0x00080000L
#define MCI_WAVE_SET_BLOCKALIGN         0x00100000L
#define MCI_WAVE_SET_BITSPERSAMPLE      0x00200000L

/* flags for the dwFlags parameter of MCI_STATUS, MCI_SET command messages */
#define MCI_WAVE_INPUT                  0x00400000L
#define MCI_WAVE_OUTPUT                 0x00800000L

/* flags for the dwItem field of MCI_STATUS_PARMS parameter block */
#define MCI_WAVE_STATUS_FORMATTAG       0x00004001L
#define MCI_WAVE_STATUS_CHANNELS        0x00004002L
#define MCI_WAVE_STATUS_SAMPLESPERSEC   0x00004003L
#define MCI_WAVE_STATUS_AVGBYTESPERSEC  0x00004004L
#define MCI_WAVE_STATUS_BLOCKALIGN      0x00004005L
#define MCI_WAVE_STATUS_BITSPERSAMPLE   0x00004006L
#define MCI_WAVE_STATUS_LEVEL           0x00004007L

/* flags for the dwFlags parameter of MCI_SET command message */
#define MCI_WAVE_SET_ANYINPUT           0x04000000L
#define MCI_WAVE_SET_ANYOUTPUT          0x08000000L

/* flags for the dwFlags parameter of MCI_GETDEVCAPS command message */
#define MCI_WAVE_GETDEVCAPS_INPUTS      0x00004001L
#define MCI_WAVE_GETDEVCAPS_OUTPUTS     0x00004002L

/* parameter block for MCI_OPEN command message */
typedef struct tagMCI_WAVE_OPEN_PARMS {
    DWORD   dwCallback;
    UINT    wDeviceID;
    UINT    wReserved0;
    LPCSTR  lpstrDeviceType;
    LPCSTR  lpstrElementName;
    LPCSTR  lpstrAlias;
    DWORD   dwBufferSeconds;
} MCI_WAVE_OPEN_PARMS;
typedef MCI_WAVE_OPEN_PARMS FAR *LPMCI_WAVE_OPEN_PARMS;

/* parameter block for MCI_DELETE command message */
typedef struct tagMCI_WAVE_DELETE_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_WAVE_DELETE_PARMS;
typedef MCI_WAVE_DELETE_PARMS FAR *LPMCI_WAVE_DELETE_PARMS;

/* parameter block for MCI_SET command message */
typedef struct tagMCI_WAVE_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
    UINT    wInput;
    UINT    wReserved0;
    UINT    wOutput;
    UINT    wReserved1;
    UINT    wFormatTag;
    UINT    wReserved2;
    UINT    nChannels;
    UINT    wReserved3;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    UINT    nBlockAlign;
    UINT    wReserved4;
    UINT    wBitsPerSample;
    UINT    wReserved5;
} MCI_WAVE_SET_PARMS;
typedef MCI_WAVE_SET_PARMS FAR * LPMCI_WAVE_SET_PARMS;


/* MCI extensions for MIDI sequencer devices */

/* flags for the dwReturn field of MCI_STATUS_PARMS parameter block */
/* MCI_STATUS command, (dwItem == MCI_SEQ_STATUS_DIVTYPE) */
#define     MCI_SEQ_DIV_PPQN            (0 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_24        (1 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_25        (2 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_30DROP    (3 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_30        (4 + MCI_SEQ_OFFSET)

/* flags for the dwMaster field of MCI_SEQ_SET_PARMS parameter block */
/* MCI_SET command, (dwFlags == MCI_SEQ_SET_MASTER) */
#define     MCI_SEQ_FORMAT_SONGPTR      0x4001
#define     MCI_SEQ_FILE                0x4002
#define     MCI_SEQ_MIDI                0x4003
#define     MCI_SEQ_SMPTE               0x4004
#define     MCI_SEQ_NONE                65533

/* flags for the dwItem field of MCI_STATUS_PARMS parameter block */
#define MCI_SEQ_STATUS_TEMPO            0x00004002L
#define MCI_SEQ_STATUS_PORT             0x00004003L
#define MCI_SEQ_STATUS_SLAVE            0x00004007L
#define MCI_SEQ_STATUS_MASTER           0x00004008L
#define MCI_SEQ_STATUS_OFFSET           0x00004009L
#define MCI_SEQ_STATUS_DIVTYPE          0x0000400AL

/* flags for the dwFlags parameter of MCI_SET command message */
#define MCI_SEQ_SET_TEMPO               0x00010000L
#define MCI_SEQ_SET_PORT                0x00020000L
#define MCI_SEQ_SET_SLAVE               0x00040000L
#define MCI_SEQ_SET_MASTER              0x00080000L
#define MCI_SEQ_SET_OFFSET              0x01000000L

/* parameter block for MCI_SET command message */
typedef struct tagMCI_SEQ_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
    DWORD   dwTempo;
    DWORD   dwPort;
    DWORD   dwSlave;
    DWORD   dwMaster;
    DWORD   dwOffset;
} MCI_SEQ_SET_PARMS;
typedef MCI_SEQ_SET_PARMS FAR * LPMCI_SEQ_SET_PARMS;


/* MCI extensions for animation devices */

/* flags for dwFlags parameter of MCI_OPEN command message */
#define MCI_ANIM_OPEN_WS                0x00010000L
#define MCI_ANIM_OPEN_PARENT            0x00020000L
#define MCI_ANIM_OPEN_NOSTATIC          0x00040000L

/* flags for dwFlags parameter of MCI_PLAY command message */
#define MCI_ANIM_PLAY_SPEED             0x00010000L
#define MCI_ANIM_PLAY_REVERSE           0x00020000L
#define MCI_ANIM_PLAY_FAST              0x00040000L
#define MCI_ANIM_PLAY_SLOW              0x00080000L
#define MCI_ANIM_PLAY_SCAN              0x00100000L

/* flags for dwFlags parameter of MCI_STEP command message */
#define MCI_ANIM_STEP_REVERSE           0x00010000L
#define MCI_ANIM_STEP_FRAMES            0x00020000L

/* flags for dwItem field of MCI_STATUS_PARMS parameter block */
#define MCI_ANIM_STATUS_SPEED           0x00004001L
#define MCI_ANIM_STATUS_FORWARD         0x00004002L
#define MCI_ANIM_STATUS_HWND            0x00004003L
#define MCI_ANIM_STATUS_HPAL            0x00004004L
#define MCI_ANIM_STATUS_STRETCH         0x00004005L

/* flags for the dwFlags parameter of MCI_INFO command message */
#define MCI_ANIM_INFO_TEXT              0x00010000L

/* flags for dwItem field of MCI_GETDEVCAPS_PARMS parameter block */
#define MCI_ANIM_GETDEVCAPS_CAN_REVERSE 0x00004001L
#define MCI_ANIM_GETDEVCAPS_FAST_RATE   0x00004002L
#define MCI_ANIM_GETDEVCAPS_SLOW_RATE   0x00004003L
#define MCI_ANIM_GETDEVCAPS_NORMAL_RATE 0x00004004L
#define MCI_ANIM_GETDEVCAPS_PALETTES    0x00004006L
#define MCI_ANIM_GETDEVCAPS_CAN_STRETCH 0x00004007L
#define MCI_ANIM_GETDEVCAPS_MAX_WINDOWS 0x00004008L

/* flags for the MCI_REALIZE command message */
#define MCI_ANIM_REALIZE_NORM           0x00010000L
#define MCI_ANIM_REALIZE_BKGD           0x00020000L

/* flags for dwFlags parameter of MCI_WINDOW command message */
#define MCI_ANIM_WINDOW_HWND            0x00010000L
#define MCI_ANIM_WINDOW_STATE           0x00040000L
#define MCI_ANIM_WINDOW_TEXT            0x00080000L
#define MCI_ANIM_WINDOW_ENABLE_STRETCH  0x00100000L
#define MCI_ANIM_WINDOW_DISABLE_STRETCH 0x00200000L

/* flags for hWnd field of MCI_ANIM_WINDOW_PARMS parameter block */
/* MCI_WINDOW command message, (dwFlags == MCI_ANIM_WINDOW_HWND) */
#define MCI_ANIM_WINDOW_DEFAULT         0x00000000L

/* flags for dwFlags parameter of MCI_PUT command message */
#define MCI_ANIM_RECT                   0x00010000L
#define MCI_ANIM_PUT_SOURCE             0x00020000L
#define MCI_ANIM_PUT_DESTINATION        0x00040000L

/* flags for dwFlags parameter of MCI_WHERE command message */
#define MCI_ANIM_WHERE_SOURCE           0x00020000L
#define MCI_ANIM_WHERE_DESTINATION      0x00040000L

/* flags for dwFlags parameter of MCI_UPDATE command message */
#define MCI_ANIM_UPDATE_HDC             0x00020000L

/* parameter block for MCI_OPEN command message */
typedef struct tagMCI_ANIM_OPEN_PARMS {
    DWORD   dwCallback;
    UINT    wDeviceID;
    UINT    wReserved0;
    LPCSTR  lpstrDeviceType;
    LPCSTR  lpstrElementName;
    LPCSTR  lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
    UINT    wReserved1;
} MCI_ANIM_OPEN_PARMS;
typedef MCI_ANIM_OPEN_PARMS FAR *LPMCI_ANIM_OPEN_PARMS;

/* parameter block for MCI_PLAY command message */
typedef struct tagMCI_ANIM_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
    DWORD   dwSpeed;
} MCI_ANIM_PLAY_PARMS;
typedef MCI_ANIM_PLAY_PARMS FAR *LPMCI_ANIM_PLAY_PARMS;

/* parameter block for MCI_STEP command message */
typedef struct tagMCI_ANIM_STEP_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrames;
} MCI_ANIM_STEP_PARMS;
typedef MCI_ANIM_STEP_PARMS FAR *LPMCI_ANIM_STEP_PARMS;

/* parameter block for MCI_WINDOW command message */
typedef struct tagMCI_ANIM_WINDOW_PARMS {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    wReserved1;
    UINT    nCmdShow;
    UINT    wReserved2;
    LPCSTR  lpstrText;
} MCI_ANIM_WINDOW_PARMS;
typedef MCI_ANIM_WINDOW_PARMS FAR * LPMCI_ANIM_WINDOW_PARMS;

/* parameter block for MCI_PUT, MCI_UPDATE, MCI_WHERE command messages */
typedef struct tagMCI_ANIM_RECT_PARMS {
    DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else   /* ifdef MCI_USE_OFFEXT */
    RECT    rc;
#endif  /* ifdef MCI_USE_OFFEXT */
} MCI_ANIM_RECT_PARMS;
typedef MCI_ANIM_RECT_PARMS FAR * LPMCI_ANIM_RECT_PARMS;

/* parameter block for MCI_UPDATE PARMS */
typedef struct tagMCI_ANIM_UPDATE_PARMS {
    DWORD   dwCallback;
    RECT    rc;
    HDC     hDC;
} MCI_ANIM_UPDATE_PARMS;
typedef MCI_ANIM_UPDATE_PARMS FAR * LPMCI_ANIM_UPDATE_PARMS;


/* MCI extensions for video overlay devices */

/* flags for dwFlags parameter of MCI_OPEN command message */
#define MCI_OVLY_OPEN_WS                0x00010000L
#define MCI_OVLY_OPEN_PARENT            0x00020000L

/* flags for dwFlags parameter of MCI_STATUS command message */
#define MCI_OVLY_STATUS_HWND            0x00004001L
#define MCI_OVLY_STATUS_STRETCH         0x00004002L

/* flags for dwFlags parameter of MCI_INFO command message */
#define MCI_OVLY_INFO_TEXT              0x00010000L

/* flags for dwItem field of MCI_GETDEVCAPS_PARMS parameter block */
#define MCI_OVLY_GETDEVCAPS_CAN_STRETCH 0x00004001L
#define MCI_OVLY_GETDEVCAPS_CAN_FREEZE  0x00004002L
#define MCI_OVLY_GETDEVCAPS_MAX_WINDOWS 0x00004003L

/* flags for dwFlags parameter of MCI_WINDOW command message */
#define MCI_OVLY_WINDOW_HWND            0x00010000L
#define MCI_OVLY_WINDOW_STATE           0x00040000L
#define MCI_OVLY_WINDOW_TEXT            0x00080000L
#define MCI_OVLY_WINDOW_ENABLE_STRETCH  0x00100000L
#define MCI_OVLY_WINDOW_DISABLE_STRETCH 0x00200000L

/* flags for hWnd parameter of MCI_OVLY_WINDOW_PARMS parameter block */
#define MCI_OVLY_WINDOW_DEFAULT         0x00000000L

/* flags for dwFlags parameter of MCI_PUT command message */
#define MCI_OVLY_RECT                   0x00010000L
#define MCI_OVLY_PUT_SOURCE             0x00020000L
#define MCI_OVLY_PUT_DESTINATION        0x00040000L
#define MCI_OVLY_PUT_FRAME              0x00080000L
#define MCI_OVLY_PUT_VIDEO              0x00100000L

/* flags for dwFlags parameter of MCI_WHERE command message */
#define MCI_OVLY_WHERE_SOURCE           0x00020000L
#define MCI_OVLY_WHERE_DESTINATION      0x00040000L
#define MCI_OVLY_WHERE_FRAME            0x00080000L
#define MCI_OVLY_WHERE_VIDEO            0x00100000L

/* parameter block for MCI_OPEN command message */
typedef struct tagMCI_OVLY_OPEN_PARMS {
    DWORD   dwCallback;
    UINT    wDeviceID;
    UINT    wReserved0;
    LPCSTR  lpstrDeviceType;
    LPCSTR  lpstrElementName;
    LPCSTR  lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
    UINT    wReserved1;
 } MCI_OVLY_OPEN_PARMS;
typedef MCI_OVLY_OPEN_PARMS FAR *LPMCI_OVLY_OPEN_PARMS;

/* parameter block for MCI_WINDOW command message */
typedef struct tagMCI_OVLY_WINDOW_PARMS {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    wReserved1;
    UINT    nCmdShow;
    UINT    wReserved2;
    LPCSTR  lpstrText;
} MCI_OVLY_WINDOW_PARMS;
typedef MCI_OVLY_WINDOW_PARMS FAR * LPMCI_OVLY_WINDOW_PARMS;

/* parameter block for MCI_PUT, MCI_UPDATE, and MCI_WHERE command messages */
typedef struct tagMCI_OVLY_RECT_PARMS {
    DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else   /* ifdef MCI_USE_OFFEXT */
    RECT    rc;
#endif  /* ifdef MCI_USE_OFFEXT */
} MCI_OVLY_RECT_PARMS;
typedef MCI_OVLY_RECT_PARMS FAR * LPMCI_OVLY_RECT_PARMS;

/* parameter block for MCI_SAVE command message */
typedef struct tagMCI_OVLY_SAVE_PARMS {
    DWORD   dwCallback;
    LPCSTR  lpfilename;
    RECT    rc;
} MCI_OVLY_SAVE_PARMS;
typedef MCI_OVLY_SAVE_PARMS FAR * LPMCI_OVLY_SAVE_PARMS;

/* parameter block for MCI_LOAD command message */
typedef struct tagMCI_OVLY_LOAD_PARMS {
    DWORD   dwCallback;
    LPCSTR  lpfilename;
    RECT    rc;
} MCI_OVLY_LOAD_PARMS;
typedef MCI_OVLY_LOAD_PARMS FAR * LPMCI_OVLY_LOAD_PARMS;

#endif  /* ifndef MMNOMCI */

/****************************************************************************

                        DISPLAY Driver extensions

****************************************************************************/

#ifndef C1_TRANSPARENT
    #define CAPS1           94          /* other caps */
    #define C1_TRANSPARENT  0x0001      /* new raster cap */
    #define NEWTRANSPARENT  3           /* use with SetBkMode() */

    #define QUERYROPSUPPORT 40          /* use to determine ROP support */
#endif  /* ifndef C1_TRANSPARENT */

/****************************************************************************

                        DIB Driver extensions

****************************************************************************/

#define SELECTDIB       41                      /* DIB.DRV select dib escape */
#define DIBINDEX(n)     MAKELONG((n),0x10FF)


/****************************************************************************

                        ScreenSaver support

    The current application will receive a syscommand of SC_SCREENSAVE just
    before the screen saver is invoked.  If the app wishes to prevent a
    screen save, return non-zero value, otherwise call DefWindowProc().

****************************************************************************/

#ifndef SC_SCREENSAVE

    #define SC_SCREENSAVE   0xF140

#endif  /* ifndef SC_SCREENSAVE */


#else    /* defined __FLAT__ */

#ifndef __WINDEF_H
#include "windef.h"
#endif

#ifndef RC_INVOKED
#pragma option -a-      /* Assume byte packing throughout */
#endif

/* general constants */
#define MAXPNAMELEN      32     // max product name length (including NULL)
#define MAXERRORLENGTH   128    // max error text length (including final NULL)

/* general data types */
typedef UINT    MMVERSION;      // major (high byte), minor (low byte)
typedef UINT    MMRESULT;       // error return code, 0 means no error
                                // call as if(err=xxxx(...)) Error(err); else
typedef DWORD   MCIERROR;       // error return code, 0 means no error

/***************************************************************************
*   Multimedia time
***************************************************************************/
typedef struct tagMMTIME {
    UINT    wType;              // the contents of the union
    union {
        DWORD ms;               // milliseconds
        DWORD sample;           // samples
        DWORD cb;               // byte count
        struct {                // SMPTE
            BYTE hour;          // hours
            BYTE min;           // minutes  (min is also a macro, so what?)
            BYTE sec;           // seconds
            BYTE frame;         // frames
            BYTE fps;           // frames per second (24, 25, 29(30 drop) or 30)
            BYTE dummy;
            } smpte;
        struct {                // MIDI
            DWORD songptrpos;   // song pointer position
            } midi;
        } u;
    } MMTIME;
typedef MMTIME       *PMMTIME;
typedef MMTIME      *NPMMTIME;
typedef MMTIME      *LPMMTIME;

// values for wType field in MMTIME struct
#define TIME_MS         0x0001  // time in Milliseconds
#define TIME_SAMPLES    0x0002  // number of wave samples
#define TIME_BYTES      0x0004  // current byte offset
#define TIME_SMPTE      0x0008  // SMPTE time
#define TIME_MIDI       0x0010  // MIDI time

/****************************************************************************
*                   Multimedia Window Messages
****************************************************************************/

#define MM_JOY1MOVE         0x3A0           // joystick
#define MM_JOY2MOVE         0x3A1
#define MM_JOY1ZMOVE        0x3A2
#define MM_JOY2ZMOVE        0x3A3
#define MM_JOY1BUTTONDOWN   0x3B5
#define MM_JOY2BUTTONDOWN   0x3B6
#define MM_JOY1BUTTONUP     0x3B7
#define MM_JOY2BUTTONUP     0x3B8

#define MM_MCINOTIFY        0x3B9           // MCI
#define MM_MCISYSTEM_STRING 0x3CA

#define MM_WOM_OPEN         0x3BB           // waveform output
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD

#define MM_WIM_OPEN         0x3BE           // waveform input
#define MM_WIM_CLOSE        0x3BF
#define MM_WIM_DATA         0x3C0

#define MM_MIM_OPEN         0x3C1           // MIDI input
#define MM_MIM_CLOSE        0x3C2
#define MM_MIM_DATA         0x3C3
#define MM_MIM_LONGDATA     0x3C4
#define MM_MIM_ERROR        0x3C5
#define MM_MIM_LONGERROR    0x3C6

#define MM_MOM_OPEN         0x3C7           // MIDI output
#define MM_MOM_CLOSE        0x3C8
#define MM_MOM_DONE         0x3C9


/****************************************************************************

                String resource number bases (internal use)

****************************************************************************/

#define MMSYSERR_BASE          0
#define WAVERR_BASE            32
#define MIDIERR_BASE           64
#define TIMERR_BASE            96   // was 128, changed to match Win 31 Sonic
#define JOYERR_BASE            160
#define MCIERR_BASE            256

#define MCI_STRING_OFFSET      512  // if this number is changed you MUST
                                    // alter the MCI_DEVTYPE_... list below
#define MCI_VD_OFFSET          1024
#define MCI_CD_OFFSET          1088
#define MCI_WAVE_OFFSET        1152
#define MCI_SEQ_OFFSET         1216

/****************************************************************************

                        General error return values

****************************************************************************/

#define MMSYSERR_NOERROR      0                    // no error
#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  // unspecified error
#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  // device ID out of range
#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  // driver failed enable
#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  // device already allocated
#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  // device handle is invalid
#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  // no device driver present
#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  // memory allocation error
#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  // function isn't supported
#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  // error value out of range
#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) // invalid flag passed
#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) // invalid parameter passed
#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) // handle being used
                                                   // simultaneously on another
                                                   // thread (eg callback)
#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) // "Specified alias not found in WIN.INI
#define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 13) // last error in range


#ifndef MMNODRV
/****************************************************************************

                        Installable driver support

****************************************************************************/

typedef HANDLE HDRVR;

#ifndef DRV_LOAD

/* Driver messages */
#define DRV_LOAD            0x0001
#define DRV_ENABLE          0x0002
#define DRV_OPEN            0x0003
#define DRV_CLOSE           0x0004
#define DRV_DISABLE         0x0005
#define DRV_FREE            0x0006
#define DRV_CONFIGURE       0x0007
#define DRV_QUERYCONFIGURE  0x0008
#define DRV_INSTALL         0x0009
#define DRV_REMOVE          0x000A
#define DRV_EXITSESSION     0x000B
#define DRV_POWER           0x000F
#define DRV_RESERVED        0x0800
#define DRV_USER            0x4000

/* LPARAM of DRV_CONFIGURE message */
typedef struct tagDRVCONFIGINFO {
    DWORD   dwDCISize;
    LPCWSTR   lpszDCISectionName;
    LPCWSTR   lpszDCIAliasName;
} DRVCONFIGINFO;
typedef DRVCONFIGINFO        *PDRVCONFIGINFO;
typedef DRVCONFIGINFO       *NPDRVCONFIGINFO;
typedef DRVCONFIGINFO       *LPDRVCONFIGINFO;

/* Supported return values for DRV_CONFIGURE message */
#define DRVCNF_CANCEL       0x0000
#define DRVCNF_OK           0x0001
#define DRVCNF_RESTART      0x0002

// return values from DriverProc() function
#define DRV_CANCEL             DRVCNF_CANCEL
#define DRV_OK                 DRVCNF_OK
#define DRV_RESTART            DRVCNF_RESTART

// installable driver function prototypes
LRESULT APIENTRY CloseDriver(HDRVR hDriver, LONG lParam1, LONG lParam2);
HDRVR   APIENTRY OpenDriver( LPCWSTR szDriverName, LPCWSTR szSectionName,
                          LONG lParam2);
LRESULT APIENTRY SendDriverMessage( HDRVR hDriver, UINT message, LONG lParam1,
                                 LONG lParam2);
HMODULE APIENTRY DrvGetModuleHandle( HDRVR hDriver );
HMODULE APIENTRY GetDriverModuleHandle( HDRVR hDriver );

#define DRV_MCI_FIRST          DRV_RESERVED
#define DRV_MCI_LAST           (DRV_RESERVED + 0xFFF)

#endif
#endif  //ifndef MMNODRV


/****************************************************************************

                          Driver callback support

****************************************************************************/

// flags used with waveOutOpen(), waveInOpen(), midiInOpen(), and
// midiOutOpen() to specify the type of the dwCallback parameter.
#define CALLBACK_TYPEMASK   0x00070000l    // callback type mask
#define CALLBACK_NULL       0x00000000l    // no callback
#define CALLBACK_WINDOW     0x00010000l    // dwCallback is a HWND
#define CALLBACK_TASK       0x00020000l    // dwCallback is a HTASK
#define CALLBACK_FUNCTION   0x00030000l    // dwCallback is a FARPROC

#ifndef MMNODRV  //ifndef MMNODRV

// driver callback prototypes
typedef void DRVCALLBACK
                    (HDRVR h, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef DRVCALLBACK     *PDRVCALLBACK;
typedef DRVCALLBACK     *LPDRVCALLBACK;

#endif  //ifndef MMNODRV

/****************************************************************************

                         Manufacturer and product IDs

    Used with wMid and wPid fields in WAVEOUTCAPS, WAVEINCAPS,
    MIDIOUTCAPS, MIDIINCAPS, AUXCAPS, JOYCAPS structures.

****************************************************************************/

// manufacturer IDs
#define MM_MICROSOFT            1       // Microsoft Corp.

// product IDs
#define MM_MIDI_MAPPER          1       // MIDI Mapper
#define MM_WAVE_MAPPER          2       // Wave Mapper

#define MM_SNDBLST_MIDIOUT      3       // Sound Blaster MIDI output port
#define MM_SNDBLST_MIDIIN       4       // Sound Blaster MIDI input port
#define MM_SNDBLST_SYNTH        5       // Sound Blaster internal synthesizer
#define MM_SNDBLST_WAVEOUT      6       // Sound Blaster waveform output
#define MM_SNDBLST_WAVEIN       7       // Sound Blaster waveform input

#define MM_ADLIB                9       // Ad Lib-compatible synthesizer

#define MM_MPU401_MIDIOUT       10      // MPU401-compatible MIDI output port
#define MM_MPU401_MIDIIN        11      // MPU401-compatible MIDI input port

#define MM_PC_JOYSTICK          12      // Joystick adapter


/****************************************************************************
*                    General MMSYSTEM support
****************************************************************************/

UINT APIENTRY mmsystemGetVersion(VOID);

// We should point people to using OutputDebugString
#define OutputDebugStr OutputDebugString
#ifndef OutputDebugStr
    VOID APIENTRY OutputDebugStr(LPCSTR);
#endif


/****************************************************************************
*                           Sound support
*  Note:  It may be fairly obvious that there are two very similar looking
*  API calls to provide high level sound support.  The major difference is
*  that PlaySound takes a module handle.  The position is to be reviewed.
****************************************************************************/

BOOL APIENTRY sndPlaySoundA( LPCSTR lpszSoundName, UINT uFlags );
BOOL APIENTRY sndPlaySoundW( LPCWSTR lpszSoundName, UINT uFlags );
#ifdef UNICODE
#define sndPlaySound  sndPlaySoundW
#else
#define sndPlaySound  sndPlaySoundA
#endif // !UNICODE

// flag values for uFlags parameter
/**** These are the Win3.1 values ***
 *  #define SND_SYNC            0x0000  // play synchronously (default)
 *  #define SND_ASYNC           0x0001  // play asynchronously
 *  #define SND_NODEFAULT       0x0002  // don't use default sound
 *  #define SND_MEMORY          0x0004  // lpszSoundName points to a memory file
 *  #define SND_LOOP            0x0008  // loop the sound until next sndPlaySound
 *  #define SND_NOSTOP          0x0010  // don't stop any currently playing sound
 * NT defines some additional flags.
 **************/

#define SND_SYNC       0x00000000   // play synchronously (default)
#define SND_ASYNC      0x00000001   // play asynchronously

#define SND_NODEFAULT  0x00000002   // silence not default, if sound not found

#define SND_MEMORY     0x00000004   // lpszSoundName points to a memory file
#define SND_ALIAS      0x00010000   // name is a WIN.INI [sounds] entry
#define SND_FILENAME   0x00020000   // name is a file name
#define SND_RESOURCE   0x00040004   // name is a resource name or atom
#define SND_ALIAS_ID   0x00110000   // name is a WIN.INI [sounds] entry identifier

#define SND_ALIAS_ID   0x00110000   // name is a WIN.INI [sounds] entry identifier
#define SND_ALIAS_START  0      // must be > 4096 to keep strings in same section of resource file

// sndAlias creates the alias identifier
#define sndAlias( ch0, ch1 ) \
                ( SND_ALIAS_START + (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ))

#define SND_ALIAS_SYSTEMASTERISK        sndAlias('S', '*')
#define SND_ALIAS_SYSTEMQUESTION        sndAlias('S', '?')
#define SND_ALIAS_SYSTEMHAND            sndAlias('S', 'H')
#define SND_ALIAS_SYSTEMEXIT            sndAlias('S', 'E')
#define SND_ALIAS_SYSTEMSTART           sndAlias('S', 'S')
#define SND_ALIAS_SYSTEMWELCOME         sndAlias('S', 'W')
#define SND_ALIAS_SYSTEMEXCLAMATION     sndAlias('S', '!')
#define SND_ALIAS_SYSTEMDEFAULT         sndAlias('S', 'D')

// 00000004 bit off means behaviour as 3.1 with SND_MEMORY not set
// ??? Needs better definition

#define SND_LOOP       0x00000008   // loop the sound until next sndPlaySound
#define SND_NOSTOP     0x00000010   // don't stop any currently playing sound
#define SND_VALID      0x0000001F  // valid flags          /* ;Internal */

#define SND_NOWAIT     0x00002000   // don't wait if the driver is busy

#define SND_VALIDFLAGS 0x0017201F   // Set of valid flag bits.  Anything outside
                                    // this range will raise an error
#define SND_RESERVED   0xFF000000   // In particular these flags are reserved

#define SND_TYPE_MASK  0x00170007

/****************************************************************************/

BOOL APIENTRY PlaySoundA( LPCSTR lpszName, HANDLE hModule, DWORD dwFlags );
BOOL APIENTRY PlaySoundW( LPCWSTR lpszName, HANDLE hModule, DWORD dwFlags );
#ifdef UNICODE
#define PlaySound  PlaySoundW
#else
#define PlaySound  PlaySoundA
#endif // !UNICODE

#ifndef MMNOWAVE
/****************************************************************************

                        Waveform audio support

****************************************************************************/

// waveform audio error return values
#define WAVERR_BADFORMAT      (WAVERR_BASE + 0)    // unsupported wave format
#define WAVERR_STILLPLAYING   (WAVERR_BASE + 1)    // still something playing
#define WAVERR_UNPREPARED     (WAVERR_BASE + 2)    // header not prepared
#define WAVERR_SYNC           (WAVERR_BASE + 3)    // device is synchronous
#define WAVERR_LASTERROR      (WAVERR_BASE + 3)    // last error in range

// waveform audio data types
typedef HANDLE          HWAVE;
typedef HWAVE           HWAVEIN;
typedef HWAVE           HWAVEOUT;
typedef HWAVEIN         *LPHWAVEIN;
typedef HWAVEOUT        *LPHWAVEOUT;
typedef DRVCALLBACK     WAVECALLBACK;
typedef WAVECALLBACK    *LPWAVECALLBACK;

// wave callback messages
#define WOM_OPEN        MM_WOM_OPEN
#define WOM_CLOSE       MM_WOM_CLOSE
#define WOM_DONE        MM_WOM_DONE
#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA

// device ID for wave device mapper
#define WAVE_MAPPER     ((DWORD)(-1))

// flags for dwFlags parameter in waveOutOpen() and waveInOpen()
#define  WAVE_FORMAT_QUERY     0x00000001
#define  WAVE_ALLOWSYNC        0x00000002
#define  WAVE_VALID            0x00000003       /* ;Internal */

// wave data block header
typedef struct wavehdr_tag {
    LPSTR                 lpData;          // pointer to locked data buffer
    DWORD                 dwBufferLength;  // length of data buffer
    DWORD                 dwBytesRecorded; // used for input only
    DWORD                 dwUser;          // for client's use
    DWORD                 dwFlags;         // assorted flags (see defines)
    DWORD                 dwLoops;         // loop control counter
    struct wavehdr_tag  * lpNext;          // reserved for driver
    DWORD                 reserved;        // reserved for driver
} WAVEHDR;
typedef WAVEHDR       *PWAVEHDR;
typedef WAVEHDR      *NPWAVEHDR;
typedef WAVEHDR      *LPWAVEHDR;

// flags for dwFlags field of WAVEHDR
#define WHDR_DONE       0x00000001  // done bit
#define WHDR_PREPARED   0x00000002  // set if this header has been prepared
#define WHDR_BEGINLOOP  0x00000004  // loop start block
#define WHDR_ENDLOOP    0x00000008  // loop end block
#define WHDR_INQUEUE    0x00000010  // reserved for driver
#define WHDR_VALID      0x0000001F  // valid flags      /* ;Internal */

// waveform output device capabilities structure
typedef struct tagWAVEOUTCAPSA {
    WORD    wMid;                   // manufacturer ID
    WORD    wPid;                   // product ID
    MMVERSION vDriverVersion;       // version of the driver
    CHAR    szPname[MAXPNAMELEN];   // product name (NULL terminated string)
    DWORD   dwFormats;              // formats supported
    WORD    wChannels;              // number of sources supported
    DWORD   dwSupport;              // functionality supported by driver
} WAVEOUTCAPSA;
// waveform output device capabilities structure
typedef struct tagWAVEOUTCAPSW {
    WORD    wMid;                   // manufacturer ID
    WORD    wPid;                   // product ID
    MMVERSION vDriverVersion;       // version of the driver
    WCHAR   szPname[MAXPNAMELEN];   // product name (NULL terminated string)
    DWORD   dwFormats;              // formats supported
    WORD    wChannels;              // number of sources supported
    DWORD   dwSupport;              // functionality supported by driver
} WAVEOUTCAPSW;
#ifdef UNICODE
typedef WAVEOUTCAPSW WAVEOUTCAPS;
#else
typedef WAVEOUTCAPSA WAVEOUTCAPS;
#endif // UNICODE

typedef WAVEOUTCAPSA     *PWAVEOUTCAPSA;
typedef WAVEOUTCAPSW     *PWAVEOUTCAPSW;
#ifdef UNICODE
typedef PWAVEOUTCAPSW PWAVEOUTCAPS;
#else
typedef PWAVEOUTCAPSA PWAVEOUTCAPS;
#endif // UNICODE
typedef WAVEOUTCAPSA     *NPWAVEOUTCAPSA;
typedef WAVEOUTCAPSW     *NPWAVEOUTCAPSW;
#ifdef UNICODE
typedef NPWAVEOUTCAPSW NPWAVEOUTCAPS;
#else
typedef NPWAVEOUTCAPSA NPWAVEOUTCAPS;
#endif // UNICODE
typedef WAVEOUTCAPSA     *LPWAVEOUTCAPSA;
typedef WAVEOUTCAPSW     *LPWAVEOUTCAPSW;
#ifdef UNICODE
typedef LPWAVEOUTCAPSW LPWAVEOUTCAPS;
#else
typedef LPWAVEOUTCAPSA LPWAVEOUTCAPS;
#endif // UNICODE

// flags for dwSupport field of WAVEOUTCAPS
#define WAVECAPS_PITCH        0x00000001  // supports pitch control
#define WAVECAPS_PLAYBACKRATE 0x00000002  // supports playback rate control
#define WAVECAPS_VOLUME       0x00000004  // supports volume control
#define WAVECAPS_LRVOLUME     0x00000008  // separate left-right volume control
#define WAVECAPS_SYNC         0x00000010

// waveform input device capabilities structure
typedef struct tagWAVEINCAPSA {
    WORD    wMid;                    // manufacturer ID
    WORD    wPid;                    // product ID
    MMVERSION vDriverVersion;        // version of the driver
    CHAR    szPname[MAXPNAMELEN];    // product name (NULL terminated string)
    DWORD   dwFormats;               // formats supported
    WORD    wChannels;               // number of channels supported
} WAVEINCAPSA;
// waveform input device capabilities structure
typedef struct tagWAVEINCAPSW {
    WORD    wMid;                    // manufacturer ID
    WORD    wPid;                    // product ID
    MMVERSION vDriverVersion;        // version of the driver
    WCHAR   szPname[MAXPNAMELEN];    // product name (NULL terminated string)
    DWORD   dwFormats;               // formats supported
    WORD    wChannels;               // number of channels supported
} WAVEINCAPSW;
#ifdef UNICODE
typedef WAVEINCAPSW WAVEINCAPS;
#else
typedef WAVEINCAPSA WAVEINCAPS;
#endif // UNICODE
typedef WAVEINCAPSA  *PWAVEINCAPSA;
typedef WAVEINCAPSW  *PWAVEINCAPSW;
#ifdef UNICODE
typedef PWAVEINCAPSW PWAVEINCAPS;
#else
typedef PWAVEINCAPSA PWAVEINCAPS;
#endif // UNICODE
typedef WAVEINCAPSA  *LPWAVEINCAPSA;
typedef WAVEINCAPSW  *LPWAVEINCAPSW;
#ifdef UNICODE
typedef LPWAVEINCAPSW LPWAVEINCAPS;
#else
typedef LPWAVEINCAPSA LPWAVEINCAPS;
#endif // UNICODE

// defines for dwFormat field of WAVEINCAPS and WAVEOUTCAPS
#define WAVE_INVALIDFORMAT     0x00000000       // invalid format
#define WAVE_FORMAT_1M08       0x00000001       // 11.025 kHz, Mono,   8-bit
#define WAVE_FORMAT_1S08       0x00000002       // 11.025 kHz, Stereo, 8-bit
#define WAVE_FORMAT_1M16       0x00000004       // 11.025 kHz, Mono,   16-bit
#define WAVE_FORMAT_1S16       0x00000008       // 11.025 kHz, Stereo, 16-bit
#define WAVE_FORMAT_2M08       0x00000010       // 22.05  kHz, Mono,   8-bit
#define WAVE_FORMAT_2S08       0x00000020       // 22.05  kHz, Stereo, 8-bit
#define WAVE_FORMAT_2M16       0x00000040       // 22.05  kHz, Mono,   16-bit
#define WAVE_FORMAT_2S16       0x00000080       // 22.05  kHz, Stereo, 16-bit
#define WAVE_FORMAT_4M08       0x00000100       // 44.1   kHz, Mono,   8-bit
#define WAVE_FORMAT_4S08       0x00000200       // 44.1   kHz, Stereo, 8-bit
#define WAVE_FORMAT_4M16       0x00000400       // 44.1   kHz, Mono,   16-bit
#define WAVE_FORMAT_4S16       0x00000800       // 44.1   kHz, Stereo, 16-bit

// flags for wFormatTag field of WAVEFORMAT
#define WAVE_FORMAT_PCM     1  // Needed in resource files so outside #ifndef RC_INVOKED

#ifndef RC_INVOKED             // RC gets upset by #pragma pack()

// general waveform format (information common to all formats)
// as read from a file.  See Multimedia file formats in Programmer Reference
#pragma pack(2)                // File format is not double word aligned
typedef struct {
    WORD    wFormatTag;        // format type
    WORD    nChannels;         // number of channels (i.e. mono, stereo, etc.)
    DWORD   nSamplesPerSec;    // sample rate
    DWORD   nAvgBytesPerSec;   // for buffer estimation
    WORD    nBlockAlign;       // block size of data
} WAVEFORMAT;
typedef WAVEFORMAT       *PWAVEFORMAT;
typedef WAVEFORMAT      *NPWAVEFORMAT;
typedef WAVEFORMAT      *LPWAVEFORMAT;
// NOTE:  The fields in the structure above are copied into
// the MCI_WAVE_SET_PARMS structure during execution

// specific waveform format for PCM data
typedef struct tagPCMWAVEFORMAT {
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;        // corresponds to MCI_WAVE_SET_.... structure
} PCMWAVEFORMAT;
typedef PCMWAVEFORMAT       *PPCMWAVEFORMAT;
typedef PCMWAVEFORMAT      *NPPCMWAVEFORMAT;
typedef PCMWAVEFORMAT      *LPPCMWAVEFORMAT;

#pragma pack()                    // revert to command line packing

// waveform audio function prototypes
UINT APIENTRY waveOutGetNumDevs(VOID);

MMRESULT APIENTRY waveOutGetDevCapsA(UINT uDeviceID, LPWAVEOUTCAPSA lpCaps,
    UINT uSize);
MMRESULT APIENTRY waveOutGetDevCapsW(UINT uDeviceID, LPWAVEOUTCAPSW lpCaps,
    UINT uSize);
#ifdef UNICODE
#define waveOutGetDevCaps  waveOutGetDevCapsW
#else
#define waveOutGetDevCaps  waveOutGetDevCapsA
#endif // !UNICODE

MMRESULT APIENTRY waveOutGetVolume(UINT uDeviceID, LPDWORD lpdwVolume);
MMRESULT APIENTRY waveOutSetVolume(UINT uDeviceID, DWORD dwVolume);

MMRESULT APIENTRY waveOutGetErrorTextA(MMRESULT err, LPSTR lpText, UINT uSize);
MMRESULT APIENTRY waveOutGetErrorTextW(MMRESULT err, LPWSTR lpText, UINT uSize);
#ifdef UNICODE
#define waveOutGetErrorText  waveOutGetErrorTextW
#else
#define waveOutGetErrorText  waveOutGetErrorTextA
#endif // !UNICODE

MMRESULT APIENTRY waveOutOpen(LPHWAVEOUT lphWaveOut, UINT uDeviceID,
    LPWAVEFORMAT lpFormat, DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
MMRESULT APIENTRY waveOutClose(HWAVEOUT hWaveOut);
MMRESULT APIENTRY waveOutPrepareHeader(HWAVEOUT hWaveOut,
     LPWAVEHDR lpWaveOutHdr, UINT uSize);
MMRESULT APIENTRY waveOutUnprepareHeader(HWAVEOUT hWaveOut,
    LPWAVEHDR lpWaveOutHdr, UINT uSize);
MMRESULT APIENTRY waveOutWrite(HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr,
    UINT uSize);
MMRESULT APIENTRY waveOutPause(HWAVEOUT hWaveOut);
MMRESULT APIENTRY waveOutRestart(HWAVEOUT hWaveOut);
MMRESULT APIENTRY waveOutReset(HWAVEOUT hWaveOut);
MMRESULT APIENTRY waveOutBreakLoop(HWAVEOUT hWaveOut);
MMRESULT APIENTRY waveOutGetPosition(HWAVEOUT hWaveOut, LPMMTIME lpInfo,
    UINT uSize);
MMRESULT APIENTRY waveOutGetPitch(HWAVEOUT hWaveOut, LPDWORD lpdwPitch);
MMRESULT APIENTRY waveOutSetPitch(HWAVEOUT hWaveOut, DWORD dwPitch);
MMRESULT APIENTRY waveOutGetPlaybackRate(HWAVEOUT hWaveOut, LPDWORD lpdwRate);
MMRESULT APIENTRY waveOutSetPlaybackRate(HWAVEOUT hWaveOut, DWORD dwRate);
MMRESULT APIENTRY waveOutGetID(HWAVEOUT hWaveOut, PUINT lpuDeviceID);
MMRESULT APIENTRY waveOutMessage(HWAVEOUT hWaveOut, UINT msg, DWORD dw1, DWORD dw2);
UINT APIENTRY waveInGetNumDevs(VOID);

MMRESULT APIENTRY waveInGetDevCapsA(UINT uDeviceID, LPWAVEINCAPSA lpCaps,
    UINT uSize);
MMRESULT APIENTRY waveInGetDevCapsW(UINT uDeviceID, LPWAVEINCAPSW lpCaps,
    UINT uSize);
#ifdef UNICODE
#define waveInGetDevCaps  waveInGetDevCapsW
#else
#define waveInGetDevCaps  waveInGetDevCapsA
#endif // !UNICODE

MMRESULT APIENTRY waveInGetErrorTextA(MMRESULT err, LPSTR lpText, UINT uSize);
MMRESULT APIENTRY waveInGetErrorTextW(MMRESULT err, LPWSTR lpText, UINT uSize);
#ifdef UNICODE
#define waveInGetErrorText  waveInGetErrorTextW
#else
#define waveInGetErrorText  waveInGetErrorTextA
#endif // !UNICODE

MMRESULT APIENTRY waveInOpen(LPHWAVEIN lphWaveIn, UINT uDeviceID,
    LPWAVEFORMAT lpFormat, DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
MMRESULT APIENTRY waveInClose(HWAVEIN hWaveIn);
MMRESULT APIENTRY waveInPrepareHeader(HWAVEIN hWaveIn,
    LPWAVEHDR lpWaveInHdr, UINT uSize);
MMRESULT APIENTRY waveInUnprepareHeader(HWAVEIN hWaveIn,
    LPWAVEHDR lpWaveInHdr, UINT uSize);
MMRESULT APIENTRY waveInAddBuffer(HWAVEIN hWaveIn,
    LPWAVEHDR lpWaveInHdr, UINT uSize);
MMRESULT APIENTRY waveInStart(HWAVEIN hWaveIn);
MMRESULT APIENTRY waveInStop(HWAVEIN hWaveIn);
MMRESULT APIENTRY waveInReset(HWAVEIN hWaveIn);
MMRESULT APIENTRY waveInGetPosition(HWAVEIN hWaveIn, LPMMTIME lpInfo,
    UINT uSize);
MMRESULT APIENTRY waveInGetID(HWAVEIN hWaveIn, PUINT lpuDeviceID);
MMRESULT APIENTRY waveInMessage(HWAVEIN hWaveIn, UINT msg, DWORD dw1, DWORD dw2);

#endif /* RC_INVOKED */
#endif  //ifndef MMNOWAVE

#ifndef MMNOMIDI
/****************************************************************************

                            MIDI audio support

****************************************************************************/

// MIDI error return values
#define MIDIERR_UNPREPARED    (MIDIERR_BASE + 0)   /* header not prepared */
#define MIDIERR_STILLPLAYING  (MIDIERR_BASE + 1)   /* still something playing */
#define MIDIERR_NOMAP         (MIDIERR_BASE + 2)   /* no current map */
#define MIDIERR_NOTREADY      (MIDIERR_BASE + 3)   /* hardware is still busy */
#define MIDIERR_NODEVICE      (MIDIERR_BASE + 4)   /* port no longer connected */
#define MIDIERR_INVALIDSETUP  (MIDIERR_BASE + 5)   /* invalid setup */
#define MIDIERR_LASTERROR     (MIDIERR_BASE + 5)   /* last error in range */

// MIDI audio data types
typedef HANDLE  HMIDI;
typedef HMIDI   HMIDIIN;
typedef HMIDI   HMIDIOUT;
typedef HMIDIIN *LPHMIDIIN;
typedef HMIDIOUT *LPHMIDIOUT;
typedef DRVCALLBACK MIDICALLBACK;
typedef MIDICALLBACK *LPMIDICALLBACK;
#define MIDIPATCHSIZE   128
typedef WORD PATCHARRAY[MIDIPATCHSIZE];
typedef WORD     *LPPATCHARRAY;
typedef WORD KEYARRAY[MIDIPATCHSIZE];
typedef WORD     *LPKEYARRAY;

// MIDI callback messages
#define MIM_OPEN        MM_MIM_OPEN
#define MIM_CLOSE       MM_MIM_CLOSE
#define MIM_DATA        MM_MIM_DATA
#define MIM_LONGDATA    MM_MIM_LONGDATA
#define MIM_ERROR       MM_MIM_ERROR
#define MIM_LONGERROR   MM_MIM_LONGERROR
#define MOM_OPEN        MM_MOM_OPEN
#define MOM_CLOSE       MM_MOM_CLOSE
#define MOM_DONE        MM_MOM_DONE

// device ID for MIDI mapper
#define MIDIMAPPER     (-1)       /* Cannot be cast to DWORD as RC complains */
#define MIDI_MAPPER    ((DWORD)(-1))

// flags for wFlags parm of midiOutCachePatches(), midiOutCacheDrumPatches()
#define MIDI_CACHE_ALL      1
#define MIDI_CACHE_BESTFIT  2
#define MIDI_CACHE_QUERY    3
#define MIDI_UNCACHE        4
#define MIDI_CACHE_VALID    (MIDI_CACHE_ALL | MIDI_CACHE_BESTFIT | MIDI_CACHE_QUERY | MIDI_UNCACHE) /* ;Internal */

// MIDI output device capabilities structure
typedef struct tagMIDIOUTCAPSA {
    WORD    wMid;                  // manufacturer ID
    WORD    wPid;                  // product ID
    MMVERSION vDriverVersion;      // version of the driver
    CHAR    szPname[MAXPNAMELEN];  // product name (NULL terminated string)
    WORD    wTechnology;           // type of device
    WORD    wVoices;               // # of voices (internal synth only)
    WORD    wNotes;                // max # of notes (internal synth only)
    WORD    wChannelMask;          // channels used (internal synth only)
    DWORD   dwSupport;             // functionality supported by driver
} MIDIOUTCAPSA;
// MIDI output device capabilities structure
typedef struct tagMIDIOUTCAPSW {
    WORD    wMid;                  // manufacturer ID
    WORD    wPid;                  // product ID
    MMVERSION vDriverVersion;      // version of the driver
    WCHAR   szPname[MAXPNAMELEN];  // product name (NULL terminated string)
    WORD    wTechnology;           // type of device
    WORD    wVoices;               // # of voices (internal synth only)
    WORD    wNotes;                // max # of notes (internal synth only)
    WORD    wChannelMask;          // channels used (internal synth only)
    DWORD   dwSupport;             // functionality supported by driver
} MIDIOUTCAPSW;
#ifdef UNICODE
typedef MIDIOUTCAPSW MIDIOUTCAPS;
#else
typedef MIDIOUTCAPSA MIDIOUTCAPS;
#endif // UNICODE
typedef MIDIOUTCAPSA    *PMIDIOUTCAPSA;
typedef MIDIOUTCAPSW    *PMIDIOUTCAPSW;
#ifdef UNICODE
typedef PMIDIOUTCAPSW PMIDIOUTCAPS;
#else
typedef PMIDIOUTCAPSA PMIDIOUTCAPS;
#endif // UNICODE
typedef MIDIOUTCAPSA    *NPMIDIOUTCAPSA;
typedef MIDIOUTCAPSW    *NPMIDIOUTCAPSW;
#ifdef UNICODE
typedef NPMIDIOUTCAPSW NPMIDIOUTCAPS;
#else
typedef NPMIDIOUTCAPSA NPMIDIOUTCAPS;
#endif // UNICODE
typedef MIDIOUTCAPSA    *LPMIDIOUTCAPSA;
typedef MIDIOUTCAPSW    *LPMIDIOUTCAPSW;
#ifdef UNICODE
typedef LPMIDIOUTCAPSW LPMIDIOUTCAPS;
#else
typedef LPMIDIOUTCAPSA LPMIDIOUTCAPS;
#endif // UNICODE

// flags for wTechnology field of MIDIOUTCAPS structure
#define MOD_MIDIPORT    1  // output port
#define MOD_SYNTH       2  // generic internal synth
#define MOD_SQSYNTH     3  // square wave internal synth
#define MOD_FMSYNTH     4  // FM internal synth
#define MOD_MAPPER      5  // MIDI mapper

// flags for dwSupport field of MIDIOUTCAPS
#define MIDICAPS_VOLUME     0x00000001  // supports volume control
#define MIDICAPS_LRVOLUME   0x00000002  // separate left-right volume control
#define MIDICAPS_CACHE      0x00000004

// MIDI input device capabilities structure
typedef struct tagMIDIINCAPSA {
    WORD        wMid;                   // manufacturer ID
    WORD        wPid;                   // product ID
    MMVERSION   vDriverVersion;         // version of the driver
    CHAR        szPname[MAXPNAMELEN];   // product name (NULL terminated string)
} MIDIINCAPSA;
// MIDI input device capabilities structure
typedef struct tagMIDIINCAPSW {
    WORD        wMid;                   // manufacturer ID
    WORD        wPid;                   // product ID
    MMVERSION   vDriverVersion;         // version of the driver
    WCHAR       szPname[MAXPNAMELEN];   // product name (NULL terminated string)
} MIDIINCAPSW;
#ifdef UNICODE
typedef MIDIINCAPSW MIDIINCAPS;
#else
typedef MIDIINCAPSA MIDIINCAPS;
#endif // UNICODE
typedef MIDIINCAPSA     *PMIDIINCAPSA;
typedef MIDIINCAPSW     *PMIDIINCAPSW;
#ifdef UNICODE
typedef PMIDIINCAPSW PMIDIINCAPS;
#else
typedef PMIDIINCAPSA PMIDIINCAPS;
#endif // UNICODE
typedef MIDIINCAPSA     *NPMIDIINCAPSA;
typedef MIDIINCAPSW     *NPMIDIINCAPSW;
#ifdef UNICODE
typedef NPMIDIINCAPSW NPMIDIINCAPS;
#else
typedef NPMIDIINCAPSA NPMIDIINCAPS;
#endif // UNICODE
typedef MIDIINCAPSA     *LPMIDIINCAPSA;
typedef MIDIINCAPSW     *LPMIDIINCAPSW;
#ifdef UNICODE
typedef LPMIDIINCAPSW LPMIDIINCAPS;
#else
typedef LPMIDIINCAPSA LPMIDIINCAPS;
#endif // UNICODE

// MIDI data block header
typedef struct midihdr_tag {
    LPSTR       lpData;               // pointer to locked data block
    DWORD       dwBufferLength;       // length of data in data block
    DWORD       dwBytesRecorded;      // used for input only
    DWORD       dwUser;               // for client's use
    DWORD       dwFlags;              // assorted flags (see defines)
    struct midihdr_tag *lpNext;   // reserved for driver
    DWORD       reserved;             // reserved for driver
} MIDIHDR;
typedef MIDIHDR       *PMIDIHDR;
typedef MIDIHDR      *NPMIDIHDR;
typedef MIDIHDR      *LPMIDIHDR;

// flags for dwFlags field of MIDIHDR structure
#define MHDR_DONE       0x00000001       // done bit
#define MHDR_PREPARED   0x00000002       // set if header prepared
#define MHDR_INQUEUE    0x00000004       // reserved for driver
#define MHDR_VALID      0x00000007       // valid flags /* ;Internal */

// MIDI function prototypes
UINT APIENTRY midiOutGetNumDevs(VOID);

MMRESULT APIENTRY midiOutGetDevCapsA(UINT uDeviceID,
    LPMIDIOUTCAPSA lpCaps, UINT uSize);
MMRESULT APIENTRY midiOutGetDevCapsW(UINT uDeviceID,
    LPMIDIOUTCAPSW lpCaps, UINT uSize);
#ifdef UNICODE
#define midiOutGetDevCaps  midiOutGetDevCapsW
#else
#define midiOutGetDevCaps  midiOutGetDevCapsA
#endif // !UNICODE

MMRESULT APIENTRY midiOutGetVolume(UINT uDeviceID, LPDWORD lpdwVolume);
MMRESULT APIENTRY midiOutSetVolume(UINT uDeviceID, DWORD dwVolume);

MMRESULT APIENTRY midiOutGetErrorTextA(MMRESULT err, LPSTR lpText, UINT uSize);
MMRESULT APIENTRY midiOutGetErrorTextW(MMRESULT err, LPWSTR lpText, UINT uSize);
#ifdef UNICODE
#define midiOutGetErrorText  midiOutGetErrorTextW
#else
#define midiOutGetErrorText  midiOutGetErrorTextA
#endif // !UNICODE

MMRESULT APIENTRY midiOutOpen(LPHMIDIOUT lphMidiOut, UINT uDeviceID,
    DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
MMRESULT APIENTRY midiOutClose(HMIDIOUT hMidiOut);
MMRESULT APIENTRY midiOutPrepareHeader(HMIDIOUT hMidiOut,
    LPMIDIHDR lpMidiOutHdr, UINT uSize);
MMRESULT APIENTRY midiOutUnprepareHeader(HMIDIOUT hMidiOut,
    LPMIDIHDR lpMidiOutHdr, UINT uSize);
MMRESULT APIENTRY midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg);
MMRESULT APIENTRY midiOutLongMsg(HMIDIOUT hMidiOut,
    LPMIDIHDR lpMidiOutHdr, UINT uSize);
MMRESULT APIENTRY midiOutReset(HMIDIOUT hMidiOut);
MMRESULT APIENTRY midiOutCachePatches(HMIDIOUT hMidiOut,
    UINT uBank, LPWORD lpPatchArray, UINT uFlags);
MMRESULT APIENTRY midiOutCacheDrumPatches(HMIDIOUT hMidiOut,
    UINT uPatch, LPWORD lpKeyArray, UINT uFlags);
MMRESULT APIENTRY midiOutGetID(HMIDIOUT hMidiOut, PUINT lpuDeviceID);
MMRESULT APIENTRY
             midiOutMessage(HMIDIOUT hMidiOut, UINT msg, DWORD dw1, DWORD dw2);

UINT APIENTRY midiInGetNumDevs(VOID);

MMRESULT APIENTRY midiInGetDevCapsA(UINT uDeviceID,
    LPMIDIINCAPSA lpCaps, UINT uSize);
MMRESULT APIENTRY midiInGetDevCapsW(UINT uDeviceID,
    LPMIDIINCAPSW lpCaps, UINT uSize);
#ifdef UNICODE
#define midiInGetDevCaps  midiInGetDevCapsW
#else
#define midiInGetDevCaps  midiInGetDevCapsA
#endif // !UNICODE

MMRESULT APIENTRY midiInGetErrorTextA(MMRESULT err, LPSTR lpText, UINT uSize);
MMRESULT APIENTRY midiInGetErrorTextW(MMRESULT err, LPWSTR lpText, UINT uSize);
#ifdef UNICODE
#define midiInGetErrorText  midiInGetErrorTextW
#else
#define midiInGetErrorText  midiInGetErrorTextA
#endif // !UNICODE

MMRESULT APIENTRY midiInOpen(LPHMIDIIN lphMidiIn, UINT uDeviceID,
    DWORD dwCallback, DWORD dwInstance, DWORD dwFlags);
MMRESULT APIENTRY midiInClose(HMIDIIN hMidiIn);
MMRESULT APIENTRY midiInPrepareHeader(HMIDIIN hMidiIn,
    LPMIDIHDR lpMidiInHdr, UINT uSize);
MMRESULT APIENTRY midiInUnprepareHeader(HMIDIIN hMidiIn,
    LPMIDIHDR lpMidiInHdr, UINT uSize);
MMRESULT APIENTRY midiInAddBuffer(HMIDIIN hMidiIn,
    LPMIDIHDR lpMidiInHdr, UINT uSize);
MMRESULT APIENTRY midiInStart(HMIDIIN hMidiIn);
MMRESULT APIENTRY midiInStop(HMIDIIN hMidiIn);
MMRESULT APIENTRY midiInReset(HMIDIIN hMidiIn);
MMRESULT APIENTRY midiInGetID(HMIDIIN hMidiIn, PUINT lpuDeviceID);
MMRESULT APIENTRY
                 midiInMessage(HMIDIIN hMidiIn, UINT msg, DWORD dw1, DWORD dw2);

#endif  //ifndef MMNOMIDI


#ifndef MMNOAUX
/****************************************************************************

                        Auxiliary audio support

****************************************************************************/

// device ID for aux device mapper
#define AUX_MAPPER     ((DWORD)(-1))

// Auxiliary audio device capabilities structure
typedef struct tagAUXCAPSA {
    WORD        wMid;                   // manufacturer ID
    WORD        wPid;                   // product ID
    MMVERSION   vDriverVersion;         // version of the driver
    CHAR        szPname[MAXPNAMELEN];   // product name (NULL terminated string)
    WORD        wTechnology;            // type of device
    DWORD       dwSupport;              // functionality supported by driver
} AUXCAPSA;
// Auxiliary audio device capabilities structure
typedef struct tagAUXCAPSW {
    WORD        wMid;                   // manufacturer ID
    WORD        wPid;                   // product ID
    MMVERSION   vDriverVersion;         // version of the driver
    WCHAR       szPname[MAXPNAMELEN];   // product name (NULL terminated string)
    WORD        wTechnology;            // type of device
    DWORD       dwSupport;              // functionality supported by driver
} AUXCAPSW;
#ifdef UNICODE
typedef AUXCAPSW AUXCAPS;
#else
typedef AUXCAPSA AUXCAPS;
#endif // UNICODE
typedef AUXCAPSA      *PAUXCAPSA;
typedef AUXCAPSW      *PAUXCAPSW;
#ifdef UNICODE
typedef PAUXCAPSW PAUXCAPS;
#else
typedef PAUXCAPSA PAUXCAPS;
#endif // UNICODE
typedef AUXCAPSA     *NPAUXCAPSA;
typedef AUXCAPSW     *NPAUXCAPSW;
#ifdef UNICODE
typedef NPAUXCAPSW NPAUXCAPS;
#else
typedef NPAUXCAPSA NPAUXCAPS;
#endif // UNICODE
typedef AUXCAPSA     *LPAUXCAPSA;
typedef AUXCAPSW     *LPAUXCAPSW;
#ifdef UNICODE
typedef LPAUXCAPSW LPAUXCAPS;
#else
typedef LPAUXCAPSA LPAUXCAPS;
#endif // UNICODE

// flags for wTechnology field in AUXCAPS structure
#define AUXCAPS_CDAUDIO    1       // audio from internal CD-ROM drive
#define AUXCAPS_AUXIN      2       // audio from auxiliary input jacks

// flags for dwSupport field in AUXCAPS structure
#define AUXCAPS_VOLUME     0x00000001   // supports volume control
#define AUXCAPS_LRVOLUME   0x00000002   // separate left-right volume control

// auxiliary audio function prototypes
UINT APIENTRY auxGetNumDevs(VOID);
MMRESULT APIENTRY auxGetDevCapsA(UINT uDeviceID, LPAUXCAPSA lpCaps, UINT uSize);
MMRESULT APIENTRY auxGetDevCapsW(UINT uDeviceID, LPAUXCAPSW lpCaps, UINT uSize);
#ifdef UNICODE
#define auxGetDevCaps  auxGetDevCapsW
#else
#define auxGetDevCaps  auxGetDevCapsA
#endif // !UNICODE

MMRESULT APIENTRY auxSetVolume(UINT uDeviceID, DWORD dwVolume);
MMRESULT APIENTRY auxGetVolume(UINT uDeviceID, LPDWORD lpdwVolume);
MMRESULT APIENTRY auxOutMessage(UINT uDeviceID, UINT msg, DWORD dw1, DWORD dw2);

#endif  //ifndef MMNOAUX


#ifndef MMNOTIMER
/****************************************************************************

                            Timer support

****************************************************************************/

// timer error return values
#define TIMERR_NOERROR        (0)                  // no error
#define TIMERR_NOCANDO        (TIMERR_BASE+1)      // request not completed
#define TIMERR_STRUCT         (TIMERR_BASE+33)     // time struct size

// timer data types
typedef VOID TIMECALLBACK
             (UINT uTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
typedef TIMECALLBACK  *LPTIMECALLBACK;

// flags for wFlags parameter of timeSetEvent() function
#define TIME_ONESHOT    0   // program timer for single event
#define TIME_PERIODIC   1   // program for continuous periodic event

// timer device capabilities data structure
typedef struct timecaps_tag {
    UINT    wPeriodMin;     // minimum period supported
    UINT    wPeriodMax;     // maximum period supported
    } TIMECAPS;
typedef TIMECAPS       *PTIMECAPS;
typedef TIMECAPS      *NPTIMECAPS;
typedef TIMECAPS      *LPTIMECAPS;

// timer function prototypes
MMRESULT APIENTRY timeGetSystemTime(LPMMTIME lpTime, UINT uSize);
DWORD APIENTRY timeGetTime(VOID);
UINT APIENTRY timeSetEvent(UINT uDelay, UINT uResolution,
    LPTIMECALLBACK lpFunction, DWORD dwUser, UINT uFlags);
MMRESULT APIENTRY timeKillEvent(UINT uId);
MMRESULT APIENTRY timeGetDevCaps(LPTIMECAPS lpTimeCaps, UINT uSize);
MMRESULT APIENTRY timeBeginPeriod(UINT uPeriod);
MMRESULT APIENTRY timeEndPeriod(UINT uPeriod);

#endif  //ifndef MMNOTIMER


#ifndef MMNOJOY
/****************************************************************************

                            Joystick support

****************************************************************************/

// joystick error return values
#define JOYERR_NOERROR        (0)                  // no error
#define JOYERR_PARMS          (JOYERR_BASE+5)      // bad parameters
#define JOYERR_NOCANDO        (JOYERR_BASE+6)      // request not completed
#define JOYERR_UNPLUGGED      (JOYERR_BASE+7)      // joystick is unplugged

// constants used with JOYINFO structure and MM_JOY* messages
#define JOY_BUTTON1         0x0001
#define JOY_BUTTON2         0x0002
#define JOY_BUTTON3         0x0004
#define JOY_BUTTON4         0x0008
#define JOY_BUTTON1CHG      0x0100
#define JOY_BUTTON2CHG      0x0200
#define JOY_BUTTON3CHG      0x0400
#define JOY_BUTTON4CHG      0x0800

// joystick ID constants
#define JOYSTICKID1         0
#define JOYSTICKID2         1

// joystick device capabilities data structure
typedef struct tagJOYCAPSA {
    WORD    wMid;                  // manufacturer ID
    WORD    wPid;                  // product ID
    CHAR    szPname[MAXPNAMELEN];  // product name (NULL terminated string)
    UINT    wXmin;                 // minimum x position value
    UINT    wXmax;                 // maximum x position value
    UINT    wYmin;                 // minimum y position value
    UINT    wYmax;                 // maximum y position value
    UINT    wZmin;                 // minimum z position value
    UINT    wZmax;                 // maximum z position value
    UINT    wNumButtons;           // number of buttons
    UINT    wPeriodMin;            // minimum message period when captured
    UINT    wPeriodMax;            // maximum message period when captured
} JOYCAPSA;
// joystick device capabilities data structure
typedef struct tagJOYCAPSW {
    WORD    wMid;                  // manufacturer ID
    WORD    wPid;                  // product ID
    WCHAR   szPname[MAXPNAMELEN];  // product name (NULL terminated string)
    UINT    wXmin;                 // minimum x position value
    UINT    wXmax;                 // maximum x position value
    UINT    wYmin;                 // minimum y position value
    UINT    wYmax;                 // maximum y position value
    UINT    wZmin;                 // minimum z position value
    UINT    wZmax;                 // maximum z position value
    UINT    wNumButtons;           // number of buttons
    UINT    wPeriodMin;            // minimum message period when captured
    UINT    wPeriodMax;            // maximum message period when captured
} JOYCAPSW;
#ifdef UNICODE
typedef JOYCAPSW JOYCAPS;
#else
typedef JOYCAPSA JOYCAPS;
#endif // UNICODE
typedef JOYCAPSA      *PJOYCAPSA;
typedef JOYCAPSW      *PJOYCAPSW;
#ifdef UNICODE
typedef PJOYCAPSW PJOYCAPS;
#else
typedef PJOYCAPSA PJOYCAPS;
#endif // UNICODE
typedef JOYCAPSA     *NPJOYCAPSA;
typedef JOYCAPSW     *NPJOYCAPSW;
#ifdef UNICODE
typedef NPJOYCAPSW NPJOYCAPS;
#else
typedef NPJOYCAPSA NPJOYCAPS;
#endif // UNICODE
typedef JOYCAPSA     *LPJOYCAPSA;
typedef JOYCAPSW     *LPJOYCAPSW;
#ifdef UNICODE
typedef LPJOYCAPSW LPJOYCAPS;
#else
typedef LPJOYCAPSA LPJOYCAPS;
#endif // UNICODE

// joystick information data structure
typedef struct joyinfo_tag {
    UINT wXpos;                 // x position
    UINT wYpos;                 // y position
    UINT wZpos;                 // z position
    UINT wButtons;              // button states
    } JOYINFO;
typedef JOYINFO       *PJOYINFO;
typedef JOYINFO      *NPJOYINFO;
typedef JOYINFO      *LPJOYINFO;

// joystick function prototypes
MMRESULT APIENTRY joyGetDevCapsA(UINT id, LPJOYCAPSA lpCaps, UINT uSize);
// joystick function prototypes
MMRESULT APIENTRY joyGetDevCapsW(UINT id, LPJOYCAPSW lpCaps, UINT uSize);
#ifdef UNICODE
#define joyGetDevCaps  joyGetDevCapsW
#else
#define joyGetDevCaps  joyGetDevCapsA
#endif // !UNICODE

UINT     APIENTRY joyGetNumDevs(VOID);
MMRESULT APIENTRY joyGetPos(UINT id, LPJOYINFO lpInfo);
MMRESULT APIENTRY joyGetThreshold(UINT id, PUINT lpuThreshold);
MMRESULT APIENTRY joyReleaseCapture(UINT id);
MMRESULT APIENTRY joySetCapture(HWND hwnd, UINT uId, UINT uPeriod,
    BOOL bChanged);
MMRESULT APIENTRY joySetThreshold(UINT id, UINT uThreshold);
UINT APIENTRY joySetCalibration(UINT id, PUINT pwXbase,       /* ;Internal */
    PUINT pwXdelta, PUINT pwYbase, PUINT pwYdelta,            /* ;Internal */
    PUINT pwZbase, PUINT pwZdelta);                           /* ;Internal */

#endif  //ifndef MMNOJOY

#ifndef MMNOMMIO
/****************************************************************************

                        Multimedia File I/O support

****************************************************************************/

// MMIO error return values
#define MMIOERR_BASE            256
#define MMIOERR_FILENOTFOUND    (MMIOERR_BASE + 1)  // file not found
#define MMIOERR_OUTOFMEMORY     (MMIOERR_BASE + 2)  // out of memory
#define MMIOERR_CANNOTOPEN      (MMIOERR_BASE + 3)  // cannot open
#define MMIOERR_CANNOTCLOSE     (MMIOERR_BASE + 4)  // cannot close
#define MMIOERR_CANNOTREAD      (MMIOERR_BASE + 5)  // cannot read
#define MMIOERR_CANNOTWRITE     (MMIOERR_BASE + 6)  // cannot write
#define MMIOERR_CANNOTSEEK      (MMIOERR_BASE + 7)  // cannot seek
#define MMIOERR_CANNOTEXPAND    (MMIOERR_BASE + 8)  // cannot expand file
#define MMIOERR_CHUNKNOTFOUND   (MMIOERR_BASE + 9)  // chunk not found
#define MMIOERR_UNBUFFERED      (MMIOERR_BASE + 10) // file is unbuffered

// MMIO constants
#define CFSEPCHAR       '+'             // compound file name separator char.

// MMIO data types
typedef DWORD           FOURCC;         // a four character code
typedef HANDLE          HMMIO;          // a handle to an open file
typedef LPSTR           HPSTR;          // a huge version of LPSTR

typedef LRESULT MMIOPROC
                    (LPSTR lpmmioinfo, UINT uMsg, LONG lParam1, LONG lParam2);
typedef MMIOPROC *LPMMIOPROC;

// general MMIO information data structure
typedef struct                  // The MMIO state
{
        /* general fields */
        DWORD           dwFlags;        // general status flags
        FOURCC          fccIOProc;      // 4 char id for the I/O procedure
        LPMMIOPROC      pIOProc;        // pointer to I/O procedure
        UINT            wErrorRet;      // place for error to be returned
        HANDLE          htask;          //

        /* fields maintained by MMIO functions during buffered I/O */
        LONG            cchBuffer;      // size of I/O buffer (or 0L)
        LPSTR           pchBuffer;      // start of I/O buffer (or NULL)
        LPSTR           pchNext;        // pointer to next byte to read/write
        LPSTR           pchEndRead;     // pointer to last valid byte to read
        LPSTR           pchEndWrite;    // pointer to last byte to write
        LONG            lBufOffset;     // disk offset of start of buffer

        /* fields maintained by I/O procedure */
        LONG            lDiskOffset;    // disk offset of next read or write
        DWORD           adwInfo[3];     // data specific to type of MMIOPROC

        /* other fields maintained by MMIO */
        DWORD           dwReserved1;    // reserved for MMIO use
        DWORD           dwReserved2;    // reserved for MMIO use
        HMMIO           hmmio;          // handle to open file

} MMIOINFO;
typedef MMIOINFO       *PMMIOINFO;
typedef MMIOINFO      *NPMMIOINFO;
typedef MMIOINFO      *LPMMIOINFO;

// RIFF chunk information data structure
typedef struct MMCKINFO_tag      // structure for representing RIFF chunk info.
{
        FOURCC          ckid;           // chunk ID
        DWORD           cksize;         // chunk size
        FOURCC          fccType;        // form type or list type
        DWORD           dwDataOffset;   // offset of data portion of chunk
        DWORD           dwFlags;        // flags used by MMIO functions
} MMCKINFO;
typedef MMCKINFO       *PMMCKINFO;
typedef MMCKINFO      *NPMMCKINFO;
typedef MMCKINFO      *LPMMCKINFO;

/* <dwFlags> field of MMIOINFO structure -- many same as OpenFile() flags */
/* Low word of flags will be passed to OpenFile() -- therefore, any MMIO-
    specific flags should be in the high word.  */
#define MMIO_RWMODE     0x00000003      // mask to get bits used for opening
                                        // file for reading/writing/both
#define MMIO_SHAREMODE  0x00000070      // file sharing mode number

// constants for dwFlags field of MMIOINFO
#define MMIO_CREATE     0x00001000      // create new file (or truncate file)
#define MMIO_PARSE      0x00000100      // parse new file returning path
#define MMIO_DELETE     0x00000200      // create new file (or truncate file)
#define MMIO_EXIST      0x00004000      // checks for existence of file
#define MMIO_ALLOCBUF   0x00010000      // mmioOpen() should allocate a buffer
#define MMIO_GETTEMP    0x00020000      // mmioOpen() should retrieve temp name

#define MMIO_DIRTY      0x10000000      // I/O buffer is dirty
/* MMIO_DIRTY is also used in the <dwFlags> field of MMCKINFO structure */

#define MMIO_OPEN_VALID 0x0003FFFF      // valid flags for mmioOpen /* ;Internal */

/* read/write mode numbers (bit field MMIO_RWMODE) */
#define MMIO_READ       0x00000000      // open file for reading only
#define MMIO_WRITE      0x00000001      // open file for writing only
#define MMIO_READWRITE  0x00000002      // open file for reading and writing

/* share mode numbers (bit field MMIO_SHAREMODE) */
#define MMIO_COMPAT     0x00000000      // compatibility mode
#define MMIO_EXCLUSIVE  0x00000010      // exclusive-access mode
#define MMIO_DENYWRITE  0x00000020      // deny writing to other processes
#define MMIO_DENYREAD   0x00000030      // deny reading to other processes
#define MMIO_DENYNONE   0x00000040      // deny nothing to other processes

/* flags for other functions */
#define MMIO_FHOPEN             0x0010  // mmioClose(): keep file handle open
#define MMIO_EMPTYBUF           0x0010  // mmioFlush(): empty the I/O buffer
#define MMIO_TOUPPER            0x0010  // mmioStringToFOURCC(): cvt. to u-case
#define MMIO_INSTALLPROC    0x00010000  // mmioInstallIOProc(): install MMIOProc
#define MMIO_GLOBALPROC     0x10000000  // mmioInstallIOProc: install globally
#define MMIO_UNICODEPROC    0x01000000  // mmioInstallIOProc(): Unicode MMIOProc
#define MMIO_REMOVEPROC     0x00020000  // mmioInstallIOProc(): remove MMIOProc
#define MMIO_FINDPROC       0x00040000  // mmioInstallIOProc(): find an MMIOProc
#define MMIO_FINDCHUNK          0x0010  // mmioDescend(): find a chunk by ID
#define MMIO_FINDRIFF           0x0020  // mmioDescend(): find a LIST chunk
                                        // ???
#define MMIO_FINDLIST           0x0040  // mmioDescend(): find a RIFF chunk
                                        // ???
#define MMIO_CREATERIFF         0x0020  // mmioCreateChunk(): make a LIST chunk
                                        // ???
#define MMIO_CREATELIST         0x0040  // mmioCreateChunk(): make a RIFF chunk
                                        // ???

#define MMIO_VALIDPROC      0x11070000  // valid for mmioInstallIOProc /* ;Internal */

/* message numbers for MMIOPROC I/O procedure functions */
#define MMIOM_READ      MMIO_READ       // read (must equal MMIO_READ!)
#define MMIOM_WRITE    MMIO_WRITE       // write (must equal MMIO_WRITE!)
#define MMIOM_SEEK              2       // seek to a new position in file
#define MMIOM_OPEN              3       // open file
#define MMIOM_CLOSE             4       // close file
#define MMIOM_WRITEFLUSH        5       // write and flush
#define MMIOM_RENAME            6       // rename specified file
#define MMIOM_USER         0x8000       // beginning of user-defined messages

/* standard four character codes */
#define FOURCC_RIFF     mmioFOURCC('R', 'I', 'F', 'F')
#define FOURCC_LIST     mmioFOURCC('L', 'I', 'S', 'T')

/* four character codes used to identify standard built-in I/O procedures */
#define FOURCC_DOS      mmioFOURCC('D', 'O', 'S', ' ')
#define FOURCC_MEM      mmioFOURCC('M', 'E', 'M', ' ')

// flags for mmioSeek()
#ifndef SEEK_SET
#define SEEK_SET        0               // seek to an absolute position
#define SEEK_CUR        1               // seek relative to current position
#define SEEK_END        2               // seek relative to end of file
#endif  //ifndef SEEK_SET

/* other constants */
#define MMIO_DEFAULTBUFFER      8192    // default buffer size

/* MMIO macros */
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
// This macro is machine byte-sex and word-sex dependent!!
// The characters are BYTES, so compatible with ANSI, not at all with UNICODE


/* MMIO prototypes */

FOURCC APIENTRY mmioStringToFOURCCA(LPCSTR sz, UINT uFlags);
FOURCC APIENTRY mmioStringToFOURCCW(LPCWSTR sz, UINT uFlags);
#ifdef UNICODE
#define mmioStringToFOURCC  mmioStringToFOURCCW
#else
#define mmioStringToFOURCC  mmioStringToFOURCCA
#endif // !UNICODE

LPMMIOPROC APIENTRY mmioInstallIOProcA( FOURCC fccIOProc, LPMMIOPROC pIOProc,
                                        DWORD dwFlags);
LPMMIOPROC APIENTRY mmioInstallIOProcW( FOURCC fccIOProc, LPMMIOPROC pIOProc,
                                        DWORD dwFlags);
#ifdef UNICODE
#define mmioInstallIOProc  mmioInstallIOProcW
#else
#define mmioInstallIOProc  mmioInstallIOProcA
#endif // !UNICODE
HMMIO APIENTRY mmioOpenA( LPSTR szFileName, LPMMIOINFO lpmmioinfo,
                          DWORD dwOpenFlags);
HMMIO APIENTRY mmioOpenW( LPWSTR szFileName, LPMMIOINFO lpmmioinfo,
                          DWORD dwOpenFlags);
#ifdef UNICODE
#define mmioOpen  mmioOpenW
#else
#define mmioOpen  mmioOpenA
#endif // !UNICODE

MMRESULT APIENTRY mmioRenameA( LPCSTR szFileName, LPCSTR szNewFileName,
                               LPMMIOINFO lpmmioinfo, DWORD dwRenameFlags);
MMRESULT APIENTRY mmioRenameW( LPCWSTR szFileName, LPCWSTR szNewFileName,
                               LPMMIOINFO lpmmioinfo, DWORD dwRenameFlags);
#ifdef UNICODE
#define mmioRename  mmioRenameW
#else
#define mmioRename  mmioRenameA
#endif // !UNICODE

MMRESULT APIENTRY mmioClose(HMMIO hmmio, UINT uFlags);
LRESULT APIENTRY mmioRead(HMMIO hmmio, LPSTR pch, LONG cch);
LRESULT APIENTRY mmioWrite(HMMIO hmmio, LPSTR pch, LONG cch);
LRESULT APIENTRY mmioSeek(HMMIO hmmio, LONG lOffset, int iOrigin);
MMRESULT APIENTRY mmioGetInfo(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags);
MMRESULT APIENTRY mmioSetInfo(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags);
MMRESULT APIENTRY mmioSetBuffer( HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer,
                                 UINT uFlags);
MMRESULT APIENTRY mmioFlush(HMMIO hmmio, UINT uFlags);
MMRESULT APIENTRY mmioAdvance(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags);
LRESULT APIENTRY mmioSendMessage( HMMIO hmmio, UINT uMsg,
                                  LONG lParam1, LONG lParam2);

/* RIFF I/O prototypes */
MMRESULT APIENTRY mmioDescend( HMMIO hmmio, LPMMCKINFO lpck,
                            LPMMCKINFO lpckParent, UINT uFlags);
MMRESULT APIENTRY mmioAscend(HMMIO hmmio, LPMMCKINFO lpck, UINT uFlags);
MMRESULT APIENTRY mmioCreateChunk(HMMIO hmmio, LPMMCKINFO lpck, UINT uFlags);

#endif  //ifndef MMNOMMIO

typedef  UINT    MCIDEVICEID;  // MCI device ID type
// BEWARE:  MCI device ids can be stored and read from Win 3 compatible
// files when the device id will be WORD size.

#ifndef MMNOMCI
/****************************************************************************

                            MCI support

****************************************************************************/

/*--------------------------------------------------------------------*\
 *
 *                           MCI functions
 *
 *--------------------------------------------------------------------
 * msiSendCommand
\*--------------------------------------------------------------------*/
MCIERROR APIENTRY mciSendCommandA(
    MCIDEVICEID wDeviceID,
    UINT uMessage,
    DWORD dwParam1,
    DWORD dwParam2 );
/*--------------------------------------------------------------------*\
 *
 *                           MCI functions
 *
 *--------------------------------------------------------------------
 * msiSendCommand
\*--------------------------------------------------------------------*/
MCIERROR APIENTRY mciSendCommandW(
    MCIDEVICEID wDeviceID,
    UINT uMessage,
    DWORD dwParam1,
    DWORD dwParam2 );
#ifdef UNICODE
#define mciSendCommand  mciSendCommandW
#else
#define mciSendCommand  mciSendCommandA
#endif // !UNICODE

/*--------------------------------------------------------------------*\
 * mciSendString
\*--------------------------------------------------------------------*/
MCIERROR APIENTRY mciSendStringA(
    LPCSTR lpstrCommand,
    LPSTR lpstrReturnString,
    UINT uReturnLength,
    HANDLE hwndCallback );
/*--------------------------------------------------------------------*\
 * mciSendString
\*--------------------------------------------------------------------*/
MCIERROR APIENTRY mciSendStringW(
    LPCWSTR lpstrCommand,
    LPWSTR lpstrReturnString,
    UINT uReturnLength,
    HANDLE hwndCallback );
#ifdef UNICODE
#define mciSendString  mciSendStringW
#else
#define mciSendString  mciSendStringA
#endif // !UNICODE

/*--------------------------------------------------------------------*\
 *  mciGetCreatorTask
\*--------------------------------------------------------------------*/
HANDLE APIENTRY mciGetCreatorTask(
    MCIDEVICEID wDeviceID);


/*--------------------------------------------------------------------*\
 *  mciGetDeviceID
\*--------------------------------------------------------------------*/
MCIDEVICEID APIENTRY mciGetDeviceIDA(
    LPCSTR lpstrName );
/*--------------------------------------------------------------------*\
 *  mciGetDeviceID
\*--------------------------------------------------------------------*/
MCIDEVICEID APIENTRY mciGetDeviceIDW(
    LPCWSTR lpstrName );
#ifdef UNICODE
#define mciGetDeviceID  mciGetDeviceIDW
#else
#define mciGetDeviceID  mciGetDeviceIDA
#endif // !UNICODE

/*--------------------------------------------------------------------*\
 *  mciGetDeviceIDFromElementID
\*--------------------------------------------------------------------*/
MCIDEVICEID APIENTRY mciGetDeviceIDFromElementIDA(
    DWORD dwElementID,
    LPCSTR lpstrType );
/*--------------------------------------------------------------------*\
 *  mciGetDeviceIDFromElementID
\*--------------------------------------------------------------------*/
MCIDEVICEID APIENTRY mciGetDeviceIDFromElementIDW(
    DWORD dwElementID,
    LPCWSTR lpstrType );
#ifdef UNICODE
#define mciGetDeviceIDFromElementID  mciGetDeviceIDFromElementIDW
#else
#define mciGetDeviceIDFromElementID  mciGetDeviceIDFromElementIDA
#endif // !UNICODE

/*--------------------------------------------------------------------*\
 * mciGetErrorString
\*--------------------------------------------------------------------*/
BOOL APIENTRY mciGetErrorStringA(
    DWORD dwError,           // Should be type==MCIERROR...
    LPSTR lpstrBuffer,
    UINT uLength );
/*--------------------------------------------------------------------*\
 * mciGetErrorString
\*--------------------------------------------------------------------*/
BOOL APIENTRY mciGetErrorStringW(
    DWORD dwError,           // Should be type==MCIERROR...
    LPWSTR lpstrBuffer,
    UINT uLength );
#ifdef UNICODE
#define mciGetErrorString  mciGetErrorStringW
#else
#define mciGetErrorString  mciGetErrorStringA
#endif // !UNICODE

typedef UINT (*YIELDPROC) (MCIDEVICEID wDeviceID, DWORD dwYieldData);

BOOL APIENTRY mciSetYieldProc ( MCIDEVICEID wDeviceID
                              , YIELDPROC fpYieldProc
                              , DWORD dwYieldData
                              );
YIELDPROC APIENTRY mciGetYieldProc ( MCIDEVICEID wDeviceID
                                   , LPDWORD lpdwYieldData
                                   );

#if (WINVER < 0x030a)
BOOL WINAPI mciExecute (LPCSTR lpstrCommand);
#endif /* ifdef WINVER < 0x030a */

// THIS NUMBER IS HARDCODED WHERE THE MCI_FORMAT_<x>'s ARE #define'd
// You must change those definitions if you change this number

//  MCI error return values
#define MCIERR_INVALID_DEVICE_ID        (MCIERR_BASE + 1)
#define MCIERR_UNRECOGNIZED_KEYWORD     (MCIERR_BASE + 3)
#define MCIERR_UNRECOGNIZED_COMMAND     (MCIERR_BASE + 5)
#define MCIERR_HARDWARE                 (MCIERR_BASE + 6)
#define MCIERR_INVALID_DEVICE_NAME      (MCIERR_BASE + 7)
#define MCIERR_OUT_OF_MEMORY            (MCIERR_BASE + 8)
#define MCIERR_DEVICE_OPEN              (MCIERR_BASE + 9)
#define MCIERR_CANNOT_LOAD_DRIVER       (MCIERR_BASE + 10)
#define MCIERR_MISSING_COMMAND_STRING   (MCIERR_BASE + 11)
#define MCIERR_PARAM_OVERFLOW           (MCIERR_BASE + 12)
#define MCIERR_MISSING_STRING_ARGUMENT  (MCIERR_BASE + 13)
#define MCIERR_BAD_INTEGER              (MCIERR_BASE + 14)
#define MCIERR_PARSER_INTERNAL          (MCIERR_BASE + 15)
#define MCIERR_DRIVER_INTERNAL          (MCIERR_BASE + 16)
#define MCIERR_MISSING_PARAMETER        (MCIERR_BASE + 17)
#define MCIERR_UNSUPPORTED_FUNCTION     (MCIERR_BASE + 18)
#define MCIERR_FILE_NOT_FOUND           (MCIERR_BASE + 19)
#define MCIERR_DEVICE_NOT_READY         (MCIERR_BASE + 20)
#define MCIERR_INTERNAL                 (MCIERR_BASE + 21)
#define MCIERR_DRIVER                   (MCIERR_BASE + 22)
#define MCIERR_CANNOT_USE_ALL           (MCIERR_BASE + 23)
#define MCIERR_MULTIPLE                 (MCIERR_BASE + 24)
#define MCIERR_EXTENSION_NOT_FOUND      (MCIERR_BASE + 25)
#define MCIERR_OUTOFRANGE               (MCIERR_BASE + 26)
#define MCIERR_FLAGS_NOT_COMPATIBLE     (MCIERR_BASE + 28)
#define MCIERR_FILE_NOT_SAVED           (MCIERR_BASE + 30)
#define MCIERR_DEVICE_TYPE_REQUIRED     (MCIERR_BASE + 31)
#define MCIERR_DEVICE_LOCKED            (MCIERR_BASE + 32)
#define MCIERR_DUPLICATE_ALIAS          (MCIERR_BASE + 33)
#define MCIERR_BAD_CONSTANT             (MCIERR_BASE + 34)
#define MCIERR_MUST_USE_SHAREABLE       (MCIERR_BASE + 35)
#define MCIERR_MISSING_DEVICE_NAME      (MCIERR_BASE + 36)
#define MCIERR_BAD_TIME_FORMAT          (MCIERR_BASE + 37)
#define MCIERR_NO_CLOSING_QUOTE         (MCIERR_BASE + 38)
#define MCIERR_DUPLICATE_FLAGS          (MCIERR_BASE + 39)
#define MCIERR_INVALID_FILE             (MCIERR_BASE + 40)
#define MCIERR_NULL_PARAMETER_BLOCK     (MCIERR_BASE + 41)
#define MCIERR_UNNAMED_RESOURCE         (MCIERR_BASE + 42)
#define MCIERR_NEW_REQUIRES_ALIAS       (MCIERR_BASE + 43)
#define MCIERR_NOTIFY_ON_AUTO_OPEN      (MCIERR_BASE + 44)
#define MCIERR_NO_ELEMENT_ALLOWED       (MCIERR_BASE + 45)
#define MCIERR_NONAPPLICABLE_FUNCTION   (MCIERR_BASE + 46)
#define MCIERR_ILLEGAL_FOR_AUTO_OPEN    (MCIERR_BASE + 47)
#define MCIERR_FILENAME_REQUIRED        (MCIERR_BASE + 48)
#define MCIERR_EXTRA_CHARACTERS         (MCIERR_BASE + 49)
#define MCIERR_DEVICE_NOT_INSTALLED     (MCIERR_BASE + 50)
#define MCIERR_GET_CD                   (MCIERR_BASE + 51)
#define MCIERR_SET_CD                   (MCIERR_BASE + 52)
#define MCIERR_SET_DRIVE                (MCIERR_BASE + 53)
#define MCIERR_DEVICE_LENGTH            (MCIERR_BASE + 54)
#define MCIERR_DEVICE_ORD_LENGTH        (MCIERR_BASE + 55)
#define MCIERR_NO_INTEGER               (MCIERR_BASE + 56)

#define MCIERR_WAVE_OUTPUTSINUSE        (MCIERR_BASE + 64)
#define MCIERR_WAVE_SETOUTPUTINUSE      (MCIERR_BASE + 65)
#define MCIERR_WAVE_INPUTSINUSE         (MCIERR_BASE + 66)
#define MCIERR_WAVE_SETINPUTINUSE       (MCIERR_BASE + 67)
#define MCIERR_WAVE_OUTPUTUNSPECIFIED   (MCIERR_BASE + 68)
#define MCIERR_WAVE_INPUTUNSPECIFIED    (MCIERR_BASE + 69)
#define MCIERR_WAVE_OUTPUTSUNSUITABLE   (MCIERR_BASE + 70)
#define MCIERR_WAVE_SETOUTPUTUNSUITABLE (MCIERR_BASE + 71)
#define MCIERR_WAVE_INPUTSUNSUITABLE    (MCIERR_BASE + 72)
#define MCIERR_WAVE_SETINPUTUNSUITABLE  (MCIERR_BASE + 73)

#define MCIERR_SEQ_DIV_INCOMPATIBLE     (MCIERR_BASE + 80)
#define MCIERR_SEQ_PORT_INUSE           (MCIERR_BASE + 81)
#define MCIERR_SEQ_PORT_NONEXISTENT     (MCIERR_BASE + 82)
#define MCIERR_SEQ_PORT_MAPNODEVICE     (MCIERR_BASE + 83)
#define MCIERR_SEQ_PORT_MISCERROR       (MCIERR_BASE + 84)
#define MCIERR_SEQ_TIMER                (MCIERR_BASE + 85)
#define MCIERR_SEQ_PORTUNSPECIFIED      (MCIERR_BASE + 86)
#define MCIERR_SEQ_NOMIDIPRESENT        (MCIERR_BASE + 87)

#define MCIERR_NO_WINDOW                (MCIERR_BASE + 90)
#define MCIERR_CREATEWINDOW             (MCIERR_BASE + 91)
#define MCIERR_FILE_READ                (MCIERR_BASE + 92)
#define MCIERR_FILE_WRITE               (MCIERR_BASE + 93)

// All custom device driver errors must be >= this value
#define MCIERR_CUSTOM_DRIVER_BASE       (MCIERR_BASE + 256)

/****************************************************************************
*          MCI command message identifiers
*****************************************************************************/
// Message numbers are hard coded because the resource compiler
// will otherwise fail on the RCDATA command table in mmsystem.rc

// They must be in the range between MCI_FIRST and MCI_LAST

#define MCI_FIRST                       0x0800
// Messages 0x801 and 0x802 are reserved
#define MCI_OPEN                        0x0803
#define MCI_CLOSE                       0x0804
#define MCI_ESCAPE                      0x0805
#define MCI_PLAY                        0x0806
#define MCI_SEEK                        0x0807
#define MCI_STOP                        0x0808
#define MCI_PAUSE                       0x0809
#define MCI_INFO                        0x080A
#define MCI_GETDEVCAPS                  0x080B
#define MCI_SPIN                        0x080C
#define MCI_SET                         0x080D
#define MCI_STEP                        0x080E
#define MCI_RECORD                      0x080F
#define MCI_SYSINFO                     0x0810
#define MCI_BREAK                       0x0811
#define MCI_SOUND                       0x0812
#define MCI_SAVE                        0x0813
#define MCI_STATUS                      0x0814

#define MCI_CUE                         0x0830

#define MCI_REALIZE                     0x0840
#define MCI_WINDOW                      0x0841
#define MCI_PUT                         0x0842
#define MCI_WHERE                       0x0843
#define MCI_FREEZE                      0x0844
#define MCI_UNFREEZE                    0x0845

#define MCI_LOAD                        0x0850
#define MCI_CUT                         0x0851
#define MCI_COPY                        0x0852
#define MCI_PASTE                       0x0853
#define MCI_UPDATE                      0x0854
#define MCI_RESUME                      0x0855
#define MCI_DELETE                      0x0856

#define MCI_LAST                        0x0FFF

// the next 0x400 message ID's are reserved for custom drivers
// all custom MCI command messages must be >= than this value
#define MCI_USER_MESSAGES               (0x400 + MCI_FIRST)


/****************************************************************************
*   Structures for the lpdwParams (dwParam2) of mciSendCommand for those
*   command messages that may be parsed in string form.
*****************************************************************************/



#define MCI_ALL_DEVICE_ID               ((MCIDEVICEID)-1)    // Matches all MCI devices

// constants for predefined MCI device types
//#define MCI_DEVTYPE_VCR                 (MCI_STRING_OFFSET + 1)
//#define MCI_DEVTYPE_VIDEODISC           (MCI_STRING_OFFSET + 2)
//#define MCI_DEVTYPE_OVERLAY             (MCI_STRING_OFFSET + 3)
//#define MCI_DEVTYPE_CD_AUDIO            (MCI_STRING_OFFSET + 4)
//#define MCI_DEVTYPE_DAT                 (MCI_STRING_OFFSET + 5)
//#define MCI_DEVTYPE_SCANNER             (MCI_STRING_OFFSET + 6)
//#define MCI_DEVTYPE_ANIMATION           (MCI_STRING_OFFSET + 7)
//#define MCI_DEVTYPE_DIGITAL_VIDEO       (MCI_STRING_OFFSET + 8)
//#define MCI_DEVTYPE_OTHER               (MCI_STRING_OFFSET + 9)
//#define MCI_DEVTYPE_WAVEFORM_AUDIO      (MCI_STRING_OFFSET + 10)
//#define MCI_DEVTYPE_SEQUENCER           (MCI_STRING_OFFSET + 11)

// Constant values are used because RC does not like the parentheses
#define MCI_DEVTYPE_VCR                 513
#define MCI_DEVTYPE_VIDEODISC           514
#define MCI_DEVTYPE_OVERLAY             515
#define MCI_DEVTYPE_CD_AUDIO            516
#define MCI_DEVTYPE_DAT                 517
#define MCI_DEVTYPE_SCANNER             518
#define MCI_DEVTYPE_ANIMATION           519
#define MCI_DEVTYPE_DIGITAL_VIDEO       520
#define MCI_DEVTYPE_OTHER               521
#define MCI_DEVTYPE_WAVEFORM_AUDIO      522
#define MCI_DEVTYPE_SEQUENCER           523

#define MCI_DEVTYPE_FIRST               MCI_DEVTYPE_VCR
#define MCI_DEVTYPE_LAST                MCI_DEVTYPE_SEQUENCER


#define MCI_DEVTYPE_FIRST_USER          0x1000

// return values for 'status mode' command
#define MCI_MODE_NOT_READY              (MCI_STRING_OFFSET + 12)
#define MCI_MODE_STOP                   (MCI_STRING_OFFSET + 13)
#define MCI_MODE_PLAY                   (MCI_STRING_OFFSET + 14)
#define MCI_MODE_RECORD                 (MCI_STRING_OFFSET + 15)
#define MCI_MODE_SEEK                   (MCI_STRING_OFFSET + 16)
#define MCI_MODE_PAUSE                  (MCI_STRING_OFFSET + 17)
#define MCI_MODE_OPEN                   (MCI_STRING_OFFSET + 18)

// constants used in 'set time format' and 'status time format' commands
// These are not based on MCI_FORMATS_OFFSET because of resource
// compiler limitations
#define MCI_FORMAT_MILLISECONDS         0
#define MCI_FORMAT_HMS                  1
#define MCI_FORMAT_MSF                  2
#define MCI_FORMAT_FRAMES               3
#define MCI_FORMAT_SMPTE_24             4
#define MCI_FORMAT_SMPTE_25             5
#define MCI_FORMAT_SMPTE_30             6
#define MCI_FORMAT_SMPTE_30DROP         7
#define MCI_FORMAT_BYTES                8
#define MCI_FORMAT_SAMPLES              9
#define MCI_FORMAT_TMSF                 10

// MCI time format conversion macros
// Note that these macros are byte-sex dependent as the SMPT format gives
// the layout in storage that's wanted.
#define MCI_MSF_MINUTE(msf)             ((BYTE)(msf))
#define MCI_MSF_SECOND(msf)             ((BYTE)(((WORD)(msf)) >> 8))
#define MCI_MSF_FRAME(msf)              ((BYTE)((msf)>>16))

#define MCI_MAKE_MSF(m, s, f)           ((DWORD)(((BYTE)(m) | \
                                                  ((WORD)(s)<<8)) | \
                                                 (((DWORD)(BYTE)(f))<<16)))

#define MCI_TMSF_TRACK(tmsf)            ((BYTE)(tmsf))
#define MCI_TMSF_MINUTE(tmsf)           ((BYTE)(((WORD)(tmsf)) >> 8))
#define MCI_TMSF_SECOND(tmsf)           ((BYTE)((tmsf)>>16))
#define MCI_TMSF_FRAME(tmsf)            ((BYTE)((tmsf)>>24))

#define MCI_MAKE_TMSF(t, m, s, f)       ( (DWORD)( ( (BYTE)(t)            \
                                                   | ( (WORD)(m) << 8 )   \
                                                   )                      \
                                                 | ( ( (DWORD)(BYTE)(s)   \
                                                     | ( (WORD)(f) <<8 )  \
                                                     )                    \
                                                   << 16                  \
                                                   )                      \
                                                 )                        \
                                        )

#define MCI_HMS_HOUR(hms)               ((BYTE)(hms))
#define MCI_HMS_MINUTE(hms)             ((BYTE)(((WORD)(hms)) >> 8))
#define MCI_HMS_SECOND(hms)             ((BYTE)((hms)>>16))

#define MCI_MAKE_HMS(h, m, s)           ( (DWORD)( ( (BYTE)(h)            \
                                                   | ( (WORD)(m) <<8 )    \
                                                   )                      \
                                                 | ( ( (DWORD)(BYTE)(s) ) \
                                                   <<16                   \
                                                   )                      \
                                                 )                        \
                                        )



/* Flags for wParam of the MM_MCINOTIFY message */
#define MCI_NOTIFY_SUCCESSFUL           0x0001
#define MCI_NOTIFY_SUPERSEDED           0x0002
#define MCI_NOTIFY_ABORTED              0x0004
#define MCI_NOTIFY_FAILURE              0x0008

// NOTE: All flags must have an "L" suffix in order to be parsed as
// DWORDs by the resource compiler

// common flags for dwFlags parameter of MCI command messages
#define MCI_NOTIFY                      0x00000001L
#define MCI_WAIT                        0x00000002L
#define MCI_FROM                        0x00000004L
#define MCI_TO                          0x00000008L
#define MCI_TRACK                       0x00000010L

// flags for dwFlags parameter of MCI_OPEN command message
#define MCI_OPEN_SHAREABLE              0x00000100L
#define MCI_OPEN_ELEMENT                0x00000200L
#define MCI_OPEN_ALIAS                  0x00000400L
#define MCI_OPEN_ELEMENT_ID             0x00000800L
#define MCI_OPEN_TYPE_ID                0x00001000L
#define MCI_OPEN_TYPE                   0x00002000L

// flags for dwFlags parameter of MCI_SEEK command message
#define MCI_SEEK_TO_START               0x00000100L
#define MCI_SEEK_TO_END                 0x00000200L

// flags for dwFlags parameter of MCI_STATUS command message
#define MCI_STATUS_ITEM                 0x00000100L
#define MCI_STATUS_START                0x00000200L

// flags for dwItem field of the MCI_STATUS_PARMS parameter block
#define MCI_STATUS_LENGTH               0x00000001L
#define MCI_STATUS_POSITION             0x00000002L
#define MCI_STATUS_NUMBER_OF_TRACKS     0x00000003L
#define MCI_STATUS_MODE                 0x00000004L
#define MCI_STATUS_MEDIA_PRESENT        0x00000005L
#define MCI_STATUS_TIME_FORMAT          0x00000006L
#define MCI_STATUS_READY                0x00000007L
#define MCI_STATUS_CURRENT_TRACK        0x00000008L

// flags for dwFlags parameter of MCI_INFO command message
#define MCI_INFO_PRODUCT                0x00000100L
#define MCI_INFO_FILE                   0x00000200L

// flags for dwFlags parameter of MCI_GETDEVCAPS command message
#define MCI_GETDEVCAPS_ITEM             0x00000100L

// flags for dwItem field of the MCI_GETDEVCAPS_PARMS parameter block
#define MCI_GETDEVCAPS_CAN_RECORD       0x00000001L
#define MCI_GETDEVCAPS_HAS_AUDIO        0x00000002L
#define MCI_GETDEVCAPS_HAS_VIDEO        0x00000003L
#define MCI_GETDEVCAPS_DEVICE_TYPE      0x00000004L
#define MCI_GETDEVCAPS_USES_FILES       0x00000005L
#define MCI_GETDEVCAPS_COMPOUND_DEVICE  0x00000006L
#define MCI_GETDEVCAPS_CAN_EJECT        0x00000007L
#define MCI_GETDEVCAPS_CAN_PLAY         0x00000008L
#define MCI_GETDEVCAPS_CAN_SAVE         0x00000009L

// flags for dwFlags parameter of MCI_SYSINFO command message
#define MCI_SYSINFO_QUANTITY            0x00000100L
#define MCI_SYSINFO_OPEN                0x00000200L
#define MCI_SYSINFO_NAME                0x00000400L
#define MCI_SYSINFO_INSTALLNAME         0x00000800L

// flags for dwFlags parameter of MCI_SET command message
#define MCI_SET_DOOR_OPEN               0x00000100L
#define MCI_SET_DOOR_CLOSED             0x00000200L
#define MCI_SET_TIME_FORMAT             0x00000400L
#define MCI_SET_AUDIO                   0x00000800L
#define MCI_SET_VIDEO                   0x00001000L
#define MCI_SET_ON                      0x00002000L
#define MCI_SET_OFF                     0x00004000L

// flags for dwAudio field of MCI_SET_PARMS or MCI_SEQ_SET_PARMS
#define MCI_SET_AUDIO_ALL               0x00004001L
#define MCI_SET_AUDIO_LEFT              0x00004002L
#define MCI_SET_AUDIO_RIGHT             0x00004003L

// flags for dwFlags parameter of MCI_BREAK command message
#define MCI_BREAK_KEY                   0x00000100L
#define MCI_BREAK_HWND                  0x00000200L
#define MCI_BREAK_OFF                   0x00000400L

// flags for dwFlags parameter of MCI_RECORD command message
#define MCI_RECORD_INSERT               0x00000100L
#define MCI_RECORD_OVERWRITE            0x00000200L

// flags for dwFlags parameter of MCI_SOUND command message
#define MCI_SOUND_NAME                  0x00000100L

// flags for dwFlags parameter of MCI_SAVE command message
#define MCI_SAVE_FILE                   0x00000100L

// flags for dwFlags parameter of MCI_LOAD command message
#define MCI_LOAD_FILE                   0x00000100L

/* Note that some structures below do not have all the fields of the
// corresponding Win 3.1 structures.  Some reserved WORD sized fields
// have disappeared.  This is deliberate.  The structures are the same
// size (with one exception which is two bytes longer).
//
// The FIRST entry in each structure must be dwCallback.  A large union
// could be defined... but...
*/

// generic parameter block for MCI command messages with no special parameters
typedef struct tagMCI_GENERIC_PARMS {
    DWORD   dwCallback;
} MCI_GENERIC_PARMS;
typedef MCI_GENERIC_PARMS     *PMCI_GENERIC_PARMS;
typedef MCI_GENERIC_PARMS     *LPMCI_GENERIC_PARMS;

// parameter block for MCI_OPEN command message
typedef struct tagMCI_OPEN_PARMSA {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCSTR     lpstrDeviceType;
    LPCSTR     lpstrElementName;
    LPCSTR     lpstrAlias;
} MCI_OPEN_PARMSA;
// parameter block for MCI_OPEN command message
typedef struct tagMCI_OPEN_PARMSW {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCWSTR    lpstrDeviceType;
    LPCWSTR    lpstrElementName;
    LPCWSTR    lpstrAlias;
} MCI_OPEN_PARMSW;
#ifdef UNICODE
typedef MCI_OPEN_PARMSW MCI_OPEN_PARMS;
#else
typedef MCI_OPEN_PARMSA MCI_OPEN_PARMS;
#endif // UNICODE
typedef MCI_OPEN_PARMSA     *PMCI_OPEN_PARMSA;
typedef MCI_OPEN_PARMSW     *PMCI_OPEN_PARMSW;
#ifdef UNICODE
typedef PMCI_OPEN_PARMSW PMCI_OPEN_PARMS;
#else
typedef PMCI_OPEN_PARMSA PMCI_OPEN_PARMS;
#endif // UNICODE
typedef MCI_OPEN_PARMSA     *LPMCI_OPEN_PARMSA;
typedef MCI_OPEN_PARMSW     *LPMCI_OPEN_PARMSW;
#ifdef UNICODE
typedef LPMCI_OPEN_PARMSW LPMCI_OPEN_PARMS;
#else
typedef LPMCI_OPEN_PARMSA LPMCI_OPEN_PARMS;
#endif // UNICODE

// parameter block for MCI_PLAY command message
typedef struct tagMCI_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_PLAY_PARMS;
typedef MCI_PLAY_PARMS     *PMCI_PLAY_PARMS;
typedef MCI_PLAY_PARMS     *LPMCI_PLAY_PARMS;

// parameter block for MCI_SEEK command message
typedef struct tagMCI_SEEK_PARMS {
    DWORD   dwCallback;
    DWORD   dwTo;
} MCI_SEEK_PARMS;
typedef MCI_SEEK_PARMS     *PMCI_SEEK_PARMS;
typedef MCI_SEEK_PARMS     *LPMCI_SEEK_PARMS;

// parameter block for MCI_STATUS command message
typedef struct tagMCI_STATUS_PARMS {
    DWORD   dwCallback;
    DWORD   dwReturn;
    DWORD   dwItem;
    DWORD   dwTrack;
} MCI_STATUS_PARMS;
typedef MCI_STATUS_PARMS     * PMCI_STATUS_PARMS;
typedef MCI_STATUS_PARMS     * LPMCI_STATUS_PARMS;

// parameter block for MCI_INFO command message
typedef struct tagMCI_INFO_PARMSA {
    DWORD   dwCallback;
    LPSTR   lpstrReturn;
    DWORD   dwRetSize;
} MCI_INFO_PARMSA;
// parameter block for MCI_INFO command message
typedef struct tagMCI_INFO_PARMSW {
    DWORD   dwCallback;
    LPWSTR  lpstrReturn;
    DWORD   dwRetSize;
} MCI_INFO_PARMSW;
#ifdef UNICODE
typedef MCI_INFO_PARMSW MCI_INFO_PARMS;
#else
typedef MCI_INFO_PARMSA MCI_INFO_PARMS;
#endif // UNICODE
typedef MCI_INFO_PARMSA     * PMCI_INFO_PARMSA;
typedef MCI_INFO_PARMSW     * PMCI_INFO_PARMSW;
#ifdef UNICODE
typedef PMCI_INFO_PARMSW PMCI_INFO_PARMS;
#else
typedef PMCI_INFO_PARMSA PMCI_INFO_PARMS;
#endif // UNICODE
typedef MCI_INFO_PARMSA     * LPMCI_INFO_PARMSA;
typedef MCI_INFO_PARMSW     * LPMCI_INFO_PARMSW;
#ifdef UNICODE
typedef LPMCI_INFO_PARMSW LPMCI_INFO_PARMS;
#else
typedef LPMCI_INFO_PARMSA LPMCI_INFO_PARMS;
#endif // UNICODE

// parameter block for MCI_GETDEVCAPS command message
typedef struct tagMCI_GETDEVCAPS_PARMS {
    DWORD   dwCallback;
    DWORD   dwReturn;
    DWORD   dwItem;
} MCI_GETDEVCAPS_PARMS;
typedef MCI_GETDEVCAPS_PARMS     * PMCI_GETDEVCAPS_PARMS;
typedef MCI_GETDEVCAPS_PARMS     * LPMCI_GETDEVCAPS_PARMS;

// parameter block for MCI_SYSINFO command message
typedef struct tagMCI_SYSINFO_PARMSA {
    DWORD   dwCallback;
    LPSTR   lpstrReturn;
    DWORD   dwRetSize;
    DWORD   dwNumber;
    UINT    wDeviceType;
} MCI_SYSINFO_PARMSA;
// parameter block for MCI_SYSINFO command message
typedef struct tagMCI_SYSINFO_PARMSW {
    DWORD   dwCallback;
    LPWSTR  lpstrReturn;
    DWORD   dwRetSize;
    DWORD   dwNumber;
    UINT    wDeviceType;
} MCI_SYSINFO_PARMSW;
#ifdef UNICODE
typedef MCI_SYSINFO_PARMSW MCI_SYSINFO_PARMS;
#else
typedef MCI_SYSINFO_PARMSA MCI_SYSINFO_PARMS;
#endif // UNICODE
typedef MCI_SYSINFO_PARMSA     *PMCI_SYSINFO_PARMSA;
typedef MCI_SYSINFO_PARMSW     *PMCI_SYSINFO_PARMSW;
#ifdef UNICODE
typedef PMCI_SYSINFO_PARMSW PMCI_SYSINFO_PARMS;
#else
typedef PMCI_SYSINFO_PARMSA PMCI_SYSINFO_PARMS;
#endif // UNICODE
typedef MCI_SYSINFO_PARMSA     *LPMCI_SYSINFO_PARMSA;
typedef MCI_SYSINFO_PARMSW     *LPMCI_SYSINFO_PARMSW;
#ifdef UNICODE
typedef LPMCI_SYSINFO_PARMSW LPMCI_SYSINFO_PARMS;
#else
typedef LPMCI_SYSINFO_PARMSA LPMCI_SYSINFO_PARMS;
#endif // UNICODE

// parameter block for MCI_SET command message
typedef struct tagMCI_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
} MCI_SET_PARMS;
typedef MCI_SET_PARMS     *PMCI_SET_PARMS;
typedef MCI_SET_PARMS     *LPMCI_SET_PARMS;

// parameter block for MCI_BREAK command message
typedef struct tagMCI_BREAK_PARMS {
    DWORD   dwCallback;
    int     nVirtKey;
    HWND    hwndBreak;
} MCI_BREAK_PARMS;
typedef MCI_BREAK_PARMS     * PMCI_BREAK_PARMS;
typedef MCI_BREAK_PARMS     * LPMCI_BREAK_PARMS;

// parameter block for MCI_SOUND command message
typedef struct tagMCI_SOUND_PARMSA {
    DWORD   dwCallback;
    LPCSTR   lpstrSoundName;
} MCI_SOUND_PARMSA;
// parameter block for MCI_SOUND command message
typedef struct tagMCI_SOUND_PARMSW {
    DWORD   dwCallback;
    LPCWSTR  lpstrSoundName;
} MCI_SOUND_PARMSW;
#ifdef UNICODE
typedef MCI_SOUND_PARMSW MCI_SOUND_PARMS;
#else
typedef MCI_SOUND_PARMSA MCI_SOUND_PARMS;
#endif // UNICODE
typedef MCI_SOUND_PARMSA     *PMCI_SOUND_PARMSA;
typedef MCI_SOUND_PARMSW     *PMCI_SOUND_PARMSW;
#ifdef UNICODE
typedef PMCI_SOUND_PARMSW PMCI_SOUND_PARMS;
#else
typedef PMCI_SOUND_PARMSA PMCI_SOUND_PARMS;
#endif // UNICODE
typedef MCI_SOUND_PARMSA     *LPMCI_SOUND_PARMSA;
typedef MCI_SOUND_PARMSW     *LPMCI_SOUND_PARMSW;
#ifdef UNICODE
typedef LPMCI_SOUND_PARMSW LPMCI_SOUND_PARMS;
#else
typedef LPMCI_SOUND_PARMSA LPMCI_SOUND_PARMS;
#endif // UNICODE

// parameter block for MCI_SAVE command message
typedef struct tagMCI_SAVE_PARMSA {
    DWORD    dwCallback;
    LPCSTR    lpfilename;
} MCI_SAVE_PARMSA;
// parameter block for MCI_SAVE command message
typedef struct tagMCI_SAVE_PARMSW {
    DWORD    dwCallback;
    LPCWSTR   lpfilename;
} MCI_SAVE_PARMSW;
#ifdef UNICODE
typedef MCI_SAVE_PARMSW MCI_SAVE_PARMS;
#else
typedef MCI_SAVE_PARMSA MCI_SAVE_PARMS;
#endif // UNICODE
typedef MCI_SAVE_PARMSA     *PMCI_SAVE_PARMSA;
typedef MCI_SAVE_PARMSW     *PMCI_SAVE_PARMSW;
#ifdef UNICODE
typedef PMCI_SAVE_PARMSW PMCI_SAVE_PARMS;
#else
typedef PMCI_SAVE_PARMSA PMCI_SAVE_PARMS;
#endif // UNICODE
typedef MCI_SAVE_PARMSA     *LPMCI_SAVE_PARMSA;
typedef MCI_SAVE_PARMSW     *LPMCI_SAVE_PARMSW;
#ifdef UNICODE
typedef LPMCI_SAVE_PARMSW LPMCI_SAVE_PARMS;
#else
typedef LPMCI_SAVE_PARMSA LPMCI_SAVE_PARMS;
#endif // UNICODE

// parameter block for MCI_LOAD command message
typedef struct tagMCI_LOAD_PARMSA {
    DWORD   dwCallback;
    LPCSTR   lpfilename;
} MCI_LOAD_PARMSA;
// parameter block for MCI_LOAD command message
typedef struct tagMCI_LOAD_PARMSW {
    DWORD   dwCallback;
    LPCWSTR  lpfilename;
} MCI_LOAD_PARMSW;
#ifdef UNICODE
typedef MCI_LOAD_PARMSW MCI_LOAD_PARMS;
#else
typedef MCI_LOAD_PARMSA MCI_LOAD_PARMS;
#endif // UNICODE
typedef MCI_LOAD_PARMSA     *PMCI_LOAD_PARMSA;
typedef MCI_LOAD_PARMSW     *PMCI_LOAD_PARMSW;
#ifdef UNICODE
typedef PMCI_LOAD_PARMSW PMCI_LOAD_PARMS;
#else
typedef PMCI_LOAD_PARMSA PMCI_LOAD_PARMS;
#endif // UNICODE
typedef MCI_LOAD_PARMSA     *LPMCI_LOAD_PARMSA;
typedef MCI_LOAD_PARMSW     *LPMCI_LOAD_PARMSW;
#ifdef UNICODE
typedef LPMCI_LOAD_PARMSW LPMCI_LOAD_PARMS;
#else
typedef LPMCI_LOAD_PARMSA LPMCI_LOAD_PARMS;
#endif // UNICODE

// parameter block for MCI_RECORD command message
typedef struct tagMCI_RECORD_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_RECORD_PARMS;
typedef MCI_RECORD_PARMS     *PMCI_RECORD_PARMS;
typedef MCI_RECORD_PARMS     *LPMCI_RECORD_PARMS;

/*****************************************************************************
*    Standard command parameters for videodisc drivers                       *
*****************************************************************************/

// string resource ID's for videodisc players
// return ID's for videodisc status command
// flag for dwReturn field of MCI_STATUS_PARMS
// MCI_STATUS command, (dwItem == MCI_STATUS_MODE)
#define MCI_VD_MODE_PARK                (MCI_VD_OFFSET + 1)

// return ID's for videodisc MCI_GETDEVCAPS command

// flag for dwReturn field of MCI_STATUS_PARMS
// MCI_STATUS command, (dwItem == MCI_VD_STATUS_MEDIA_TYPE)
#define MCI_VD_MEDIA_CLV                (MCI_VD_OFFSET + 2)
#define MCI_VD_MEDIA_CAV                (MCI_VD_OFFSET + 3)
#define MCI_VD_MEDIA_OTHER              (MCI_VD_OFFSET + 4)

#define MCI_VD_FORMAT_TRACK             0x4001

// flags for dwFlags parameter of MCI_PLAY command message
#define MCI_VD_PLAY_REVERSE             0x00010000L
#define MCI_VD_PLAY_FAST                0x00020000L
#define MCI_VD_PLAY_SPEED               0x00040000L
#define MCI_VD_PLAY_SCAN                0x00080000L
#define MCI_VD_PLAY_SLOW                0x00100000L

// flag for dwFlags parameter of MCI_SEEK command message
#define MCI_VD_SEEK_REVERSE             0x00010000L

// flags for dwItem field of MCI_STATUS_PARMS parameter block
#define MCI_VD_STATUS_SPEED             0x00004002L
#define MCI_VD_STATUS_FORWARD           0x00004003L
#define MCI_VD_STATUS_MEDIA_TYPE        0x00004004L
#define MCI_VD_STATUS_SIDE              0x00004005L
#define MCI_VD_STATUS_DISC_SIZE         0x00004006L

// flags for dwFlags parameter of MCI_GETDEVCAPS command message
#define MCI_VD_GETDEVCAPS_CLV           0x00010000L
#define MCI_VD_GETDEVCAPS_CAV           0x00020000L

#define MCI_VD_SPIN_UP                  0x00010000L
#define MCI_VD_SPIN_DOWN                0x00020000L

// flags for dwItem field of MCI_GETDEVCAPS_PARMS parameter block
#define MCI_VD_GETDEVCAPS_CAN_REVERSE   0x00004002L
#define MCI_VD_GETDEVCAPS_FAST_RATE     0x00004003L
#define MCI_VD_GETDEVCAPS_SLOW_RATE     0x00004004L
#define MCI_VD_GETDEVCAPS_NORMAL_RATE   0x00004005L

// flags for the dwFlags parameter of MCI_STEP command message
#define MCI_VD_STEP_FRAMES              0x00010000L
#define MCI_VD_STEP_REVERSE             0x00020000L

// flag for the MCI_ESCAPE command message
#define MCI_VD_ESCAPE_STRING            0x00000100L

// parameter block for MCI_PLAY command message
typedef struct tagMCI_VD_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
    DWORD   dwSpeed;
} MCI_VD_PLAY_PARMS;
typedef MCI_VD_PLAY_PARMS     *PMCI_VD_PLAY_PARMS;
typedef MCI_VD_PLAY_PARMS     *LPMCI_VD_PLAY_PARMS;

// parameter block for MCI_STEP command message
typedef struct tagMCI_VD_STEP_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrames;
} MCI_VD_STEP_PARMS;
typedef MCI_VD_STEP_PARMS     *PMCI_VD_STEP_PARMS;
typedef MCI_VD_STEP_PARMS     *LPMCI_VD_STEP_PARMS;

// parameter block for MCI_ESCAPE command message
typedef struct tagMCI_VD_ESCAPE_PARMSA {
    DWORD   dwCallback;
    LPCSTR    lpstrCommand;
} MCI_VD_ESCAPE_PARMSA;
// parameter block for MCI_ESCAPE command message
typedef struct tagMCI_VD_ESCAPE_PARMSW {
    DWORD   dwCallback;
    LPCWSTR   lpstrCommand;
} MCI_VD_ESCAPE_PARMSW;
#ifdef UNICODE
typedef MCI_VD_ESCAPE_PARMSW MCI_VD_ESCAPE_PARMS;
#else
typedef MCI_VD_ESCAPE_PARMSA MCI_VD_ESCAPE_PARMS;
#endif // UNICODE
typedef MCI_VD_ESCAPE_PARMSA     *PMCI_VD_ESCAPE_PARMSA;
typedef MCI_VD_ESCAPE_PARMSW     *PMCI_VD_ESCAPE_PARMSW;
#ifdef UNICODE
typedef PMCI_VD_ESCAPE_PARMSW PMCI_VD_ESCAPE_PARMS;
#else
typedef PMCI_VD_ESCAPE_PARMSA PMCI_VD_ESCAPE_PARMS;
#endif // UNICODE
typedef MCI_VD_ESCAPE_PARMSA     *LPMCI_VD_ESCAPE_PARMSA;
typedef MCI_VD_ESCAPE_PARMSW     *LPMCI_VD_ESCAPE_PARMSW;
#ifdef UNICODE
typedef LPMCI_VD_ESCAPE_PARMSW LPMCI_VD_ESCAPE_PARMS;
#else
typedef LPMCI_VD_ESCAPE_PARMSA LPMCI_VD_ESCAPE_PARMS;
#endif // UNICODE

/*****************************************************************************
*   Standard command parameters for waveform audio drivers
*****************************************************************************/

#define MCI_WAVE_PCM            (MCI_WAVE_OFFSET+0)
#define MCI_WAVE_MAPPER         (MCI_WAVE_OFFSET+1)

// flags for the dwFlags parameter of MCI_OPEN command message
#define MCI_WAVE_OPEN_BUFFER            0x00010000L

// flags for the dwFlags parameter of MCI_SET command message
#define MCI_WAVE_SET_FORMATTAG          0x00010000L
#define MCI_WAVE_SET_CHANNELS           0x00020000L
#define MCI_WAVE_SET_SAMPLESPERSEC      0x00040000L
#define MCI_WAVE_SET_AVGBYTESPERSEC     0x00080000L
#define MCI_WAVE_SET_BLOCKALIGN         0x00100000L
#define MCI_WAVE_SET_BITSPERSAMPLE      0x00200000L

// flags for the dwFlags parameter of MCI_STATUS, MCI_SET command messages
#define MCI_WAVE_INPUT                  0x00400000L
#define MCI_WAVE_OUTPUT                 0x00800000L

// flags for the dwItem field of MCI_STATUS_PARMS parameter block
#define MCI_WAVE_STATUS_FORMATTAG       0x00004001L
#define MCI_WAVE_STATUS_CHANNELS        0x00004002L
#define MCI_WAVE_STATUS_SAMPLESPERSEC   0x00004003L
#define MCI_WAVE_STATUS_AVGBYTESPERSEC  0x00004004L
#define MCI_WAVE_STATUS_BLOCKALIGN      0x00004005L
#define MCI_WAVE_STATUS_BITSPERSAMPLE   0x00004006L
#define MCI_WAVE_STATUS_LEVEL           0x00004007L

// flags for the dwFlags parameter of MCI_SET command message
#define MCI_WAVE_SET_ANYINPUT           0x04000000L
#define MCI_WAVE_SET_ANYOUTPUT          0x08000000L

// flags for the dwFlags parameter of MCI_GETDEVCAPS command message
#define MCI_WAVE_GETDEVCAPS_INPUTS      0x00004001L
#define MCI_WAVE_GETDEVCAPS_OUTPUTS     0x00004002L

// parameter block for MCI_OPEN command message
typedef struct tagMCI_WAVE_OPEN_PARMSA {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCSTR    lpstrDeviceType;
    LPCSTR    lpstrElementName;
    LPCSTR    lpstrAlias;
    DWORD   dwBufferSeconds;
} MCI_WAVE_OPEN_PARMSA;
// parameter block for MCI_OPEN command message
typedef struct tagMCI_WAVE_OPEN_PARMSW {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCWSTR   lpstrDeviceType;
    LPCWSTR   lpstrElementName;
    LPCWSTR   lpstrAlias;
    DWORD   dwBufferSeconds;
} MCI_WAVE_OPEN_PARMSW;
#ifdef UNICODE
typedef MCI_WAVE_OPEN_PARMSW MCI_WAVE_OPEN_PARMS;
#else
typedef MCI_WAVE_OPEN_PARMSA MCI_WAVE_OPEN_PARMS;
#endif // UNICODE
typedef MCI_WAVE_OPEN_PARMSA     *PMCI_WAVE_OPEN_PARMSA;
typedef MCI_WAVE_OPEN_PARMSW     *PMCI_WAVE_OPEN_PARMSW;
#ifdef UNICODE
typedef PMCI_WAVE_OPEN_PARMSW PMCI_WAVE_OPEN_PARMS;
#else
typedef PMCI_WAVE_OPEN_PARMSA PMCI_WAVE_OPEN_PARMS;
#endif // UNICODE
typedef MCI_WAVE_OPEN_PARMSA     *LPMCI_WAVE_OPEN_PARMSA;
typedef MCI_WAVE_OPEN_PARMSW     *LPMCI_WAVE_OPEN_PARMSW;
#ifdef UNICODE
typedef LPMCI_WAVE_OPEN_PARMSW LPMCI_WAVE_OPEN_PARMS;
#else
typedef LPMCI_WAVE_OPEN_PARMSA LPMCI_WAVE_OPEN_PARMS;
#endif // UNICODE

// parameter block for MCI_DELETE command message
typedef struct tagMCI_WAVE_DELETE_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_WAVE_DELETE_PARMS;
typedef MCI_WAVE_DELETE_PARMS     *PMCI_WAVE_DELETE_PARMS;
typedef MCI_WAVE_DELETE_PARMS     *LPMCI_WAVE_DELETE_PARMS;

// parameter block for MCI_SET command message
typedef struct tagMCI_WAVE_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
    UINT    wInput;
    UINT    wOutput;
    WORD    wFormatTag;                // corresponds to WAVEFORMAT structure
    WORD    wReserved2;
    WORD    nChannels;                 // corresponds to WAVEFORMAT structure
    WORD    wReserved3;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    WORD    nBlockAlign;               // corresponds to WAVEFORMAT structure
    WORD    wReserved4;
    WORD    wBitsPerSample;            // corresponds to PCMWAVEFORMAT structure
    WORD    wReserved5;
} MCI_WAVE_SET_PARMS;
typedef MCI_WAVE_SET_PARMS     * PMCI_WAVE_SET_PARMS;
typedef MCI_WAVE_SET_PARMS     * LPMCI_WAVE_SET_PARMS;

/*****************************************************************************
*   Standard command parameters for CD audio drivers
*****************************************************************************/

// None

/*****************************************************************************
*        Standard command parameters for Sequencer drivers
*****************************************************************************/

// string resource ID's for sequencers
// return ids for status division type

// flags for the dwReturn field of MCI_STATUS_PARMS parameter block
// MCI_STATUS command, (dwItem == MCI_SEQ_STATUS_DIVTYPE)
#define     MCI_SEQ_DIV_PPQN              (0 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_24        (1 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_25        (2 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_30DROP    (3 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_30        (4 + MCI_SEQ_OFFSET)

// flags for the dwMaster field of MCI_SEQ_SET_PARMS parameter block
// MCI_SET command, (dwFlags == MCI_SEQ_SET_MASTER)
#define     MCI_SEQ_FORMAT_SONGPTR      0x4001
#define     MCI_SEQ_FILE                0x4002
#define     MCI_SEQ_MIDI                0x4003
#define     MCI_SEQ_SMPTE               0x4004
#define     MCI_SEQ_NONE                65533

#define     MCI_SEQ_MAPPER          65535

// flags for the dwItem field of MCI_STATUS_PARMS parameter block
#define MCI_SEQ_STATUS_TEMPO              0x00004002L
#define MCI_SEQ_STATUS_PORT              0x00004003L
#define MCI_SEQ_STATUS_SLAVE             0x00004007L
#define MCI_SEQ_STATUS_MASTER           0x00004008L
#define MCI_SEQ_STATUS_OFFSET             0x00004009L
#define MCI_SEQ_STATUS_DIVTYPE          0x0000400AL

// flags for the dwFlags parameter of MCI_SET command message
#define MCI_SEQ_SET_TEMPO                 0x00010000L
#define MCI_SEQ_SET_PORT                  0x00020000L
#define MCI_SEQ_SET_SLAVE               0x00040000L
#define MCI_SEQ_SET_MASTER              0x00080000L
#define MCI_SEQ_SET_OFFSET                0x01000000L

// parameter block for MCI_SET command message
typedef struct tagMCI_SEQ_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
    DWORD   dwTempo;
    DWORD   dwPort;
    DWORD   dwSlave;
    DWORD   dwMaster;
    DWORD   dwOffset;
} MCI_SEQ_SET_PARMS;
typedef MCI_SEQ_SET_PARMS     * PMCI_SEQ_SET_PARMS;
typedef MCI_SEQ_SET_PARMS     * LPMCI_SEQ_SET_PARMS;

/*****************************************************************************
*      Standard command parameters for animation
*****************************************************************************/


// flags for dwFlags parameter of MCI_OPEN command message
#define MCI_ANIM_OPEN_WS                0x00010000L
#define MCI_ANIM_OPEN_PARENT            0x00020000L
#define MCI_ANIM_OPEN_NOSTATIC          0x00040000L

// flags for dwFlags parameter of MCI_PLAY command message
#define MCI_ANIM_PLAY_SPEED             0x00010000L
#define MCI_ANIM_PLAY_REVERSE           0x00020000L
#define MCI_ANIM_PLAY_FAST              0x00040000L
#define MCI_ANIM_PLAY_SLOW              0x00080000L
#define MCI_ANIM_PLAY_SCAN              0x00100000L

// flags for dwFlags parameter of MCI_STEP command message
#define MCI_ANIM_STEP_REVERSE           0x00010000L
#define MCI_ANIM_STEP_FRAMES            0x00020000L

// flags for dwItem field of MCI_STATUS_PARMS parameter block
#define MCI_ANIM_STATUS_SPEED           0x00004001L
#define MCI_ANIM_STATUS_FORWARD         0x00004002L
#define MCI_ANIM_STATUS_HWND            0x00004003L
#define MCI_ANIM_STATUS_HPAL            0x00004004L
#define MCI_ANIM_STATUS_STRETCH         0x00004005L

// flags for the dwFlags parameter of MCI_INFO command message
#define MCI_ANIM_INFO_TEXT              0x00010000L

// flags for dwItem field of MCI_GETDEVCAPS_PARMS parameter block
#define MCI_ANIM_GETDEVCAPS_CAN_REVERSE 0x00004001L
#define MCI_ANIM_GETDEVCAPS_FAST_RATE   0x00004002L
#define MCI_ANIM_GETDEVCAPS_SLOW_RATE   0x00004003L
#define MCI_ANIM_GETDEVCAPS_NORMAL_RATE 0x00004004L
#define MCI_ANIM_GETDEVCAPS_PALETTES    0x00004006L
#define MCI_ANIM_GETDEVCAPS_CAN_STRETCH 0x00004007L
#define MCI_ANIM_GETDEVCAPS_MAX_WINDOWS 0x00004008L

// flags for the MCI_REALIZE command message
#define MCI_ANIM_REALIZE_NORM           0x00010000L
#define MCI_ANIM_REALIZE_BKGD           0x00020000L

// flags for dwFlags parameter of MCI_WINDOW command message
#define MCI_ANIM_WINDOW_HWND            0x00010000L
#define MCI_ANIM_WINDOW_STATE           0x00040000L
#define MCI_ANIM_WINDOW_TEXT            0x00080000L
#define MCI_ANIM_WINDOW_ENABLE_STRETCH  0x00100000L
#define MCI_ANIM_WINDOW_DISABLE_STRETCH 0x00200000L

// flags for hWnd field of MCI_ANIM_WINDOW_PARMS parameter block
// MCI_WINDOW command message, (dwFlags == MCI_ANIM_WINDOW_HWND)
#define MCI_ANIM_WINDOW_DEFAULT         0x00000000L

// flags for dwFlags parameter of MCI_PUT command message
#define MCI_ANIM_RECT                   0x00010000L
#define MCI_ANIM_PUT_SOURCE             0x00020000L  // also  MCI_WHERE
#define MCI_ANIM_PUT_DESTINATION        0x00040000L  // also  MCI_WHERE

// flags for dwFlags parameter of MCI_WHERE command message
#define MCI_ANIM_WHERE_SOURCE           0x00020000L
#define MCI_ANIM_WHERE_DESTINATION      0x00040000L

// flags for dwFlags parameter of MCI_UPDATE command message
#define MCI_ANIM_UPDATE_HDC             0x00020000L

// parameter block for MCI_OPEN command message
typedef struct tagMCI_ANIM_OPEN_PARMSA {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCSTR    lpstrDeviceType;
    LPCSTR    lpstrElementName;
    LPCSTR    lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
} MCI_ANIM_OPEN_PARMSA;
// parameter block for MCI_OPEN command message
typedef struct tagMCI_ANIM_OPEN_PARMSW {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCWSTR   lpstrDeviceType;
    LPCWSTR   lpstrElementName;
    LPCWSTR   lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
} MCI_ANIM_OPEN_PARMSW;
#ifdef UNICODE
typedef MCI_ANIM_OPEN_PARMSW MCI_ANIM_OPEN_PARMS;
#else
typedef MCI_ANIM_OPEN_PARMSA MCI_ANIM_OPEN_PARMS;
#endif // UNICODE
typedef MCI_ANIM_OPEN_PARMSA     *PMCI_ANIM_OPEN_PARMSA;
typedef MCI_ANIM_OPEN_PARMSW     *PMCI_ANIM_OPEN_PARMSW;
#ifdef UNICODE
typedef PMCI_ANIM_OPEN_PARMSW PMCI_ANIM_OPEN_PARMS;
#else
typedef PMCI_ANIM_OPEN_PARMSA PMCI_ANIM_OPEN_PARMS;
#endif // UNICODE
typedef MCI_ANIM_OPEN_PARMSA     *LPMCI_ANIM_OPEN_PARMSA;
typedef MCI_ANIM_OPEN_PARMSW     *LPMCI_ANIM_OPEN_PARMSW;
#ifdef UNICODE
typedef LPMCI_ANIM_OPEN_PARMSW LPMCI_ANIM_OPEN_PARMS;
#else
typedef LPMCI_ANIM_OPEN_PARMSA LPMCI_ANIM_OPEN_PARMS;
#endif // UNICODE

// parameter block for MCI_PLAY command message
typedef struct tagMCI_ANIM_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
    DWORD   dwSpeed;
} MCI_ANIM_PLAY_PARMS;
typedef MCI_ANIM_PLAY_PARMS     *PMCI_ANIM_PLAY_PARMS;
typedef MCI_ANIM_PLAY_PARMS     *LPMCI_ANIM_PLAY_PARMS;

// parameter block for MCI_STEP command message
typedef struct tagMCI_ANIM_STEP_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrames;
} MCI_ANIM_STEP_PARMS;
typedef MCI_ANIM_STEP_PARMS     *PMCI_ANIM_STEP_PARMS;
typedef MCI_ANIM_STEP_PARMS     *LPMCI_ANIM_STEP_PARMS;

// parameter block for MCI_WINDOW command message
typedef struct tagMCI_ANIM_WINDOW_PARMSA {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    nCmdShow;
    LPCSTR   lpstrText;
} MCI_ANIM_WINDOW_PARMSA;
// parameter block for MCI_WINDOW command message
typedef struct tagMCI_ANIM_WINDOW_PARMSW {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    nCmdShow;
    LPCWSTR  lpstrText;
} MCI_ANIM_WINDOW_PARMSW;
#ifdef UNICODE
typedef MCI_ANIM_WINDOW_PARMSW MCI_ANIM_WINDOW_PARMS;
#else
typedef MCI_ANIM_WINDOW_PARMSA MCI_ANIM_WINDOW_PARMS;
#endif // UNICODE
typedef MCI_ANIM_WINDOW_PARMSA     * PMCI_ANIM_WINDOW_PARMSA;
typedef MCI_ANIM_WINDOW_PARMSW     * PMCI_ANIM_WINDOW_PARMSW;
#ifdef UNICODE
typedef PMCI_ANIM_WINDOW_PARMSW PMCI_ANIM_WINDOW_PARMS;
#else
typedef PMCI_ANIM_WINDOW_PARMSA PMCI_ANIM_WINDOW_PARMS;
#endif // UNICODE
typedef MCI_ANIM_WINDOW_PARMSA     * LPMCI_ANIM_WINDOW_PARMSA;
typedef MCI_ANIM_WINDOW_PARMSW     * LPMCI_ANIM_WINDOW_PARMSW;
#ifdef UNICODE
typedef LPMCI_ANIM_WINDOW_PARMSW LPMCI_ANIM_WINDOW_PARMS;
#else
typedef LPMCI_ANIM_WINDOW_PARMSA LPMCI_ANIM_WINDOW_PARMS;
#endif // UNICODE

// parameter block for MCI_PUT, MCI_UPDATE, MCI_WHERE command messages
typedef struct tagMCI_ANIM_RECT_PARMS {
    DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else   //ifdef MCI_USE_OFFEXT
    RECT    rc;
#endif  //ifdef MCI_USE_OFFEXT
} MCI_ANIM_RECT_PARMS;
typedef MCI_ANIM_RECT_PARMS     * PMCI_ANIM_RECT_PARMS;
typedef MCI_ANIM_RECT_PARMS     * LPMCI_ANIM_RECT_PARMS;

// parameter block for MCI_UPDATE PARMS
typedef struct tagMCI_ANIM_UPDATE_PARMS {
    DWORD   dwCallback;
    RECT    rc;
    HDC     hDC;
} MCI_ANIM_UPDATE_PARMS;
typedef MCI_ANIM_UPDATE_PARMS     * PMCI_ANIM_UPDATE_PARMS;
typedef MCI_ANIM_UPDATE_PARMS     * LPMCI_ANIM_UPDATE_PARMS;


/*****************************************************************************

    Standard command parameters for overlay

*****************************************************************************/


// flags for dwFlags parameter of MCI_OPEN command message
#define MCI_OVLY_OPEN_WS                0x00010000L
#define MCI_OVLY_OPEN_PARENT            0x00020000L

// flags for dwFlags parameter of MCI_STATUS command message
#define MCI_OVLY_STATUS_HWND            0x00004001L
#define MCI_OVLY_STATUS_STRETCH         0x00004002L

// flags for dwFlags parameter of MCI_INFO command message
#define MCI_OVLY_INFO_TEXT              0x00010000L

// flags for dwItem field of MCI_GETDEVCAPS_PARMS parameter block
#define MCI_OVLY_GETDEVCAPS_CAN_STRETCH 0x00004001L
#define MCI_OVLY_GETDEVCAPS_CAN_FREEZE  0x00004002L
#define MCI_OVLY_GETDEVCAPS_MAX_WINDOWS 0x00004003L

// flags for dwFlags parameter of MCI_WINDOW command message
#define MCI_OVLY_WINDOW_HWND            0x00010000L
#define MCI_OVLY_WINDOW_STATE           0x00040000L
#define MCI_OVLY_WINDOW_TEXT            0x00080000L
#define MCI_OVLY_WINDOW_ENABLE_STRETCH  0x00100000L
#define MCI_OVLY_WINDOW_DISABLE_STRETCH 0x00200000L

// flags for hWnd parameter of MCI_OVLY_WINDOW_PARMS parameter block
#define MCI_OVLY_WINDOW_DEFAULT         0x00000000L

// flags for dwFlags parameter of MCI_PUT command message
#define MCI_OVLY_RECT                   0x00010000L
#define MCI_OVLY_PUT_SOURCE             0x00020000L
#define MCI_OVLY_PUT_DESTINATION        0x00040000L
#define MCI_OVLY_PUT_FRAME              0x00080000L
#define MCI_OVLY_PUT_VIDEO              0x00100000L

// flags for dwFlags parameter of MCI_WHERE command message
#define MCI_OVLY_WHERE_SOURCE           0x00020000L
#define MCI_OVLY_WHERE_DESTINATION      0x00040000L
#define MCI_OVLY_WHERE_FRAME            0x00080000L
#define MCI_OVLY_WHERE_VIDEO            0x00100000L


// parameter block for MCI_OPEN command message
typedef struct tagMCI_OVLY_OPEN_PARMSA {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCSTR    lpstrDeviceType;
    LPCSTR    lpstrElementName;
    LPCSTR    lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
 } MCI_OVLY_OPEN_PARMSA;
// parameter block for MCI_OPEN command message
typedef struct tagMCI_OVLY_OPEN_PARMSW {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCWSTR   lpstrDeviceType;
    LPCWSTR   lpstrElementName;
    LPCWSTR   lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
 } MCI_OVLY_OPEN_PARMSW;
#ifdef UNICODE
typedef MCI_OVLY_OPEN_PARMSW MCI_OVLY_OPEN_PARMS;
#else
typedef MCI_OVLY_OPEN_PARMSA MCI_OVLY_OPEN_PARMS;
#endif // UNICODE
typedef MCI_OVLY_OPEN_PARMSA     *PMCI_OVLY_OPEN_PARMSA;
typedef MCI_OVLY_OPEN_PARMSW     *PMCI_OVLY_OPEN_PARMSW;
#ifdef UNICODE
typedef PMCI_OVLY_OPEN_PARMSW PMCI_OVLY_OPEN_PARMS;
#else
typedef PMCI_OVLY_OPEN_PARMSA PMCI_OVLY_OPEN_PARMS;
#endif // UNICODE
typedef MCI_OVLY_OPEN_PARMSA     *LPMCI_OVLY_OPEN_PARMSA;
typedef MCI_OVLY_OPEN_PARMSW     *LPMCI_OVLY_OPEN_PARMSW;
#ifdef UNICODE
typedef LPMCI_OVLY_OPEN_PARMSW LPMCI_OVLY_OPEN_PARMS;
#else
typedef LPMCI_OVLY_OPEN_PARMSA LPMCI_OVLY_OPEN_PARMS;
#endif // UNICODE

// parameter block for MCI_WINDOW command message
typedef struct tagMCI_OVLY_WINDOW_PARMSA {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    nCmdShow;
    LPCSTR    lpstrText;
} MCI_OVLY_WINDOW_PARMSA;
// parameter block for MCI_WINDOW command message
typedef struct tagMCI_OVLY_WINDOW_PARMSW {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    nCmdShow;
    LPCWSTR   lpstrText;
} MCI_OVLY_WINDOW_PARMSW;
#ifdef UNICODE
typedef MCI_OVLY_WINDOW_PARMSW MCI_OVLY_WINDOW_PARMS;
#else
typedef MCI_OVLY_WINDOW_PARMSA MCI_OVLY_WINDOW_PARMS;
#endif // UNICODE
typedef MCI_OVLY_WINDOW_PARMSA     * PMCI_OVLY_WINDOW_PARMSA;
typedef MCI_OVLY_WINDOW_PARMSW     * PMCI_OVLY_WINDOW_PARMSW;
#ifdef UNICODE
typedef PMCI_OVLY_WINDOW_PARMSW PMCI_OVLY_WINDOW_PARMS;
#else
typedef PMCI_OVLY_WINDOW_PARMSA PMCI_OVLY_WINDOW_PARMS;
#endif // UNICODE
typedef MCI_OVLY_WINDOW_PARMSA     * LPMCI_OVLY_WINDOW_PARMSA;
typedef MCI_OVLY_WINDOW_PARMSW     * LPMCI_OVLY_WINDOW_PARMSW;
#ifdef UNICODE
typedef LPMCI_OVLY_WINDOW_PARMSW LPMCI_OVLY_WINDOW_PARMS;
#else
typedef LPMCI_OVLY_WINDOW_PARMSA LPMCI_OVLY_WINDOW_PARMS;
#endif // UNICODE

// parameter block for MCI_PUT, MCI_UPDATE, and MCI_WHERE command messages
typedef struct tagMCI_OVLY_RECT_PARMS {
    DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else   //ifdef MCI_USE_OFFEXT
    RECT    rc;
#endif  //ifdef MCI_USE_OFFEXT
} MCI_OVLY_RECT_PARMS;
typedef MCI_OVLY_RECT_PARMS     * PMCI_OVLY_RECT_PARMS;
typedef MCI_OVLY_RECT_PARMS     * LPMCI_OVLY_RECT_PARMS;

// parameter block for MCI_SAVE command message
typedef struct tagMCI_OVLY_SAVE_PARMSA {
    DWORD   dwCallback;
    LPCSTR    lpfilename;
    RECT    rc;
} MCI_OVLY_SAVE_PARMSA;
// parameter block for MCI_SAVE command message
typedef struct tagMCI_OVLY_SAVE_PARMSW {
    DWORD   dwCallback;
    LPCWSTR   lpfilename;
    RECT    rc;
} MCI_OVLY_SAVE_PARMSW;
#ifdef UNICODE
typedef MCI_OVLY_SAVE_PARMSW MCI_OVLY_SAVE_PARMS;
#else
typedef MCI_OVLY_SAVE_PARMSA MCI_OVLY_SAVE_PARMS;
#endif // UNICODE
typedef MCI_OVLY_SAVE_PARMSA     * PMCI_OVLY_SAVE_PARMSA;
typedef MCI_OVLY_SAVE_PARMSW     * PMCI_OVLY_SAVE_PARMSW;
#ifdef UNICODE
typedef PMCI_OVLY_SAVE_PARMSW PMCI_OVLY_SAVE_PARMS;
#else
typedef PMCI_OVLY_SAVE_PARMSA PMCI_OVLY_SAVE_PARMS;
#endif // UNICODE
typedef MCI_OVLY_SAVE_PARMSA     * LPMCI_OVLY_SAVE_PARMSA;
typedef MCI_OVLY_SAVE_PARMSW     * LPMCI_OVLY_SAVE_PARMSW;
#ifdef UNICODE
typedef LPMCI_OVLY_SAVE_PARMSW LPMCI_OVLY_SAVE_PARMS;
#else
typedef LPMCI_OVLY_SAVE_PARMSA LPMCI_OVLY_SAVE_PARMS;
#endif // UNICODE

// parameter block for MCI_LOAD command message
typedef struct tagMCI_OVLY_LOAD_PARMSA {
    DWORD   dwCallback;
    LPCSTR    lpfilename;
    RECT    rc;
} MCI_OVLY_LOAD_PARMSA;
// parameter block for MCI_LOAD command message
typedef struct tagMCI_OVLY_LOAD_PARMSW {
    DWORD   dwCallback;
    LPCWSTR   lpfilename;
    RECT    rc;
} MCI_OVLY_LOAD_PARMSW;
#ifdef UNICODE
typedef MCI_OVLY_LOAD_PARMSW MCI_OVLY_LOAD_PARMS;
#else
typedef MCI_OVLY_LOAD_PARMSA MCI_OVLY_LOAD_PARMS;
#endif // UNICODE
typedef MCI_OVLY_LOAD_PARMSA     * PMCI_OVLY_LOAD_PARMSA;
typedef MCI_OVLY_LOAD_PARMSW     * PMCI_OVLY_LOAD_PARMSW;
#ifdef UNICODE
typedef PMCI_OVLY_LOAD_PARMSW PMCI_OVLY_LOAD_PARMS;
#else
typedef PMCI_OVLY_LOAD_PARMSA PMCI_OVLY_LOAD_PARMS;
#endif // UNICODE
typedef MCI_OVLY_LOAD_PARMSA     * LPMCI_OVLY_LOAD_PARMSA;
typedef MCI_OVLY_LOAD_PARMSW     * LPMCI_OVLY_LOAD_PARMSW;
#ifdef UNICODE
typedef LPMCI_OVLY_LOAD_PARMSW LPMCI_OVLY_LOAD_PARMS;
#else
typedef LPMCI_OVLY_LOAD_PARMSA LPMCI_OVLY_LOAD_PARMS;
#endif // UNICODE

#endif  //ifndef MMNOMCI


/****************************************************************************

                        DISPLAY Driver extensions

****************************************************************************/

#ifndef C1_TRANSPARENT
#define CAPS1           94              // other caps
#define C1_TRANSPARENT  0x0001          // new raster cap
#define NEWTRANSPARENT  3               // use with SetBkMode()

#define QUERYROPSUPPORT 40              // use to determine ROP support
#endif  //ifndef C1_TRANSPARENT

/****************************************************************************

                        DIB Driver extensions

****************************************************************************/

#define SELECTDIB       41              // DIB.DRV select dib escape
#define DIBINDEX(n)     MAKELONG((n),0x10FF)


/****************************************************************************

                        ScreenSaver support

    The current application will receive a syscommand of SC_SCREENSAVE just
    before the screen saver is invoked.  If the app wishes to prevent a
    screen save, return non-zero value, otherwise call DefWindowProc().

****************************************************************************/

#ifndef SC_SCREENSAVE

#define SC_SCREENSAVE   0xF140

#endif  //ifndef SC_SCREENSAVE


#endif  /* __FLAT__ */

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#ifndef RC_INVOKED
#pragma option -a.      /* Revert to default packing */
#endif  /* RC_INVOKED */

#endif  /* __MMSYSTEM_H */
