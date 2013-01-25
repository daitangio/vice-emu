# Microsoft Developer Studio Generated NMAKE File, Based on xscpu64.dsp
!IF "$(CFG)" == ""
CFG=xscpu64 - Win32 Release
!MESSAGE No configuration specified. Defaulting to xscpu64 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "xscpu64 - Win32 Release" && "$(CFG)" != "xscpu64 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xscpu64.mak" CFG="xscpu64 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xscpu64 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xscpu64 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\xscpu64\Release
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\xscpu64.exe"

!ELSE 

ALL : "platform - Win32 Release" "p64 - Win32 Release" "userport - Win32 Release" "rtc - Win32 Release" "iecbus - Win32 Release" "core - Win32 Release" "imagecontents - Win32 Release" "diskimage - Win32 Release" "c64cart - Win32 Release" "c64cartsystem - Win32 Release" "c64commoncart - Win32 Release" "monitor - Win32 Release" "video - Win32 Release" "vdrive - Win32 Release" "raster - Win32 Release" "rtc - Win32 Release" "viciisc - Win32 Release" "sounddrv - Win32 Release" "sid - Win32 Release" "serial - Win32 Release" "rs232drv - Win32 Release" "printerdrv - Win32 Release" "parallel - Win32 Release" "ieee - Win32 Release" "iecieee - Win32 Release" "c64exp - Win32 Release" "iec - Win32 Release" "gfxoutputdrv - Win32 Release" "fsdevice - Win32 Release" "fileio - Win32 Release" "drive - Win32 Release" "c64scpu64 - Win32 Release" "scpu64 - Win32 Release" "base - Win32 Release" "arch - Win32 Release" "$(OUTDIR)\xscpu64.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"arch - Win32 ReleaseCLEAN" "base - Win32 ReleaseCLEAN" "scpu64 - Win32 ReleaseCLEAN" "c64scpu64 - Win32 ReleaseCLEAN" "drive - Win32 ReleaseCLEAN" "fileio - Win32 ReleaseCLEAN" "fsdevice - Win32 ReleaseCLEAN" "gfxoutputdrv - Win32 ReleaseCLEAN" "iec - Win32 ReleaseCLEAN" "c64exp - Win32 ReleaseCLEAN" "iecieee - Win32 ReleaseCLEAN" "ieee - Win32 ReleaseCLEAN" "parallel - Win32 ReleaseCLEAN" "printerdrv - Win32 ReleaseCLEAN" "rs232drv - Win32 ReleaseCLEAN" "serial - Win32 ReleaseCLEAN" "sid - Win32 ReleaseCLEAN" "sounddrv - Win32 ReleaseCLEAN" "viciisc - Win32 ReleaseCLEAN" "rtc - Win32 ReleaseCLEAN" "raster - Win32 ReleaseCLEAN" "vdrive - Win32 ReleaseCLEAN" "video - Win32 ReleaseCLEAN" "monitor - Win32 ReleaseCLEAN" "c64cart - Win32 ReleaseCLEAN" "c64cartsystem - Win32 ReleaseCLEAN" "c64commoncart - Win32 ReleaseCLEAN" "diskimage - Win32 ReleaseCLEAN" "imagecontents - Win32 ReleaseCLEAN" "core - Win32 ReleaseCLEAN" "iecbus - Win32 ReleaseCLEAN" "rtc - Win32 ReleaseCLEAN" "userport - Win32 ReleaseCLEAN" "p64 - Win32 ReleaseCLEAN" "platform - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\xscpu64.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\platform" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\xscpu64.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

.c{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xscpu64.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib opengl32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\xscpu64.pdb" /machine:MIPS /out:"$(OUTDIR)\xscpu64.exe" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	".\libs\arch\Release\arch.lib" \
	".\libs\base\Release\base.lib" \
	".\libs\c64scpu64\Release\c64scpu64.lib" \
	".\libs\scpu64\Release\scpu64.lib" \
	".\libs\drive\Release\drive.lib" \
	".\libs\fileio\Release\fileio.lib" \
	".\libs\fsdevice\Release\fsdevice.lib" \
	".\libs\gfxoutputdrv\Release\gfxoutputdrv.lib" \
	".\libs\iec\Release\iec.lib" \
	".\libs\c64exp\Release\c64exp.lib" \
	".\libs\iecieee\Release\iecieee.lib" \
	".\libs\ieee\Release\ieee.lib" \
	".\libs\parallel\Release\parallel.lib" \
	".\libs\printerdrv\Release\printerdrv.lib" \
	".\libs\rs232drv\Release\rs232drv.lib" \
	".\libs\serial\Release\serial.lib" \
	".\libs\sid\Release\sid.lib" \
	".\libs\sounddrv\Release\sounddrv.lib" \
	".\libs\viciisc\Release\viciisc.lib" \
	".\libs\rtc\Release\rtc.lib" \
	".\libs\raster\Release\raster.lib" \
	".\libs\vdrive\Release\vdrive.lib" \
	".\libs\video\Release\video.lib" \
	".\libs\monitor\Release\monitor.lib" \
	".\libs\c64cart\Release\c64cart.lib" \
	".\libs\c64cartsystem\Release\c64cartsystem.lib" \
	".\libs\c64commoncart\Release\c64commoncart.lib" \
	".\libs\diskimage\Release\diskimage.lib" \
	".\libs\imagecontents\Release\imagecontents.lib" \
	".\libs\core\Release\core.lib" \
	".\libs\userport\Release\userport.lib" \
	".\libs\rtc\Release\rtc.lib" \
	".\libs\iecbus\Release\iecbus.lib" \
	".\libs\p64\Release\p64.lib" \
	".\libs\platform\Release\platform.lib"

"$(OUTDIR)\xscpu64.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\xscpu64\Debug
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\xscpu64.exe"

!ELSE 

ALL : "platform - Win32 Debug" "p64 - Win32 Debug" "userport - Win32 Debug" "rtc - Win32 Debug" "iecbus - Win32 Debug" "core - Win32 Debug" "imagecontents - Win32 Debug" "diskimage - Win32 Debug" "c64cart - Win32 Debug" "c64cartsystem - Win32 Debug" "c64commoncart - Win32 Debug" "monitor - Win32 Debug" "video - Win32 Debug" "vdrive - Win32 Debug" "raster - Win32 Debug" "rtc - Win32 Debug" "viciisc - Win32 Debug" "sounddrv - Win32 Debug" "sid - Win32 Debug" "serial - Win32 Debug" "rs232drv - Win32 Debug" "printerdrv - Win32 Debug" "parallel - Win32 Debug" "ieee - Win32 Debug" "iecieee - Win32 Debug" "c64exp - Win32 Debug" "iec - Win32 Debug" "gfxoutputdrv - Win32 Debug" "fsdevice - Win32 Debug" "fileio - Win32 Debug" "drive - Win32 Debug" "c64scpu64 - Win32 Debug" "scpu64 - Win32 Debug" "base - Win32 Debug" "arch - Win32 Debug" "$(OUTDIR)\xscpu64.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"arch - Win32 DebugCLEAN" "base - Win32 DebugCLEAN" "scpu64 - Win32 DebugCLEAN" "c64scpu64 - Win32 DebugCLEAN" "drive - Win32 DebugCLEAN" "fileio - Win32 DebugCLEAN" "fsdevice - Win32 DebugCLEAN" "gfxoutputdrv - Win32 DebugCLEAN" "iec - Win32 DebugCLEAN" "c64exp - Win32 DebugCLEAN" "iecieee - Win32 DebugCLEAN" "ieee - Win32 DebugCLEAN" "parallel - Win32 DebugCLEAN" "printerdrv - Win32 DebugCLEAN" "rs232drv - Win32 DebugCLEAN" "serial - Win32 DebugCLEAN" "sid - Win32 DebugCLEAN" "sounddrv - Win32 DebugCLEAN" "viciisc - Win32 DebugCLEAN" "rtc - Win32 DebugCLEAN" "raster - Win32 DebugCLEAN" "vdrive - Win32 DebugCLEAN" "video - Win32 DebugCLEAN" "monitor - Win32 DebugCLEAN" "c64cart - Win32 DebugCLEAN" "c64cartsystem - Win32 DebugCLEAN" "c64commoncart - Win32 DebugCLEAN" "diskimage - Win32 DebugCLEAN" "imagecontents - Win32 DebugCLEAN" "core - Win32 DebugCLEAN" "iecbus - Win32 DebugCLEAN" "rtc - Win32 DebugCLEAN" "userport - Win32 DebugCLEAN" "p64 - Win32 DebugCLEAN" "platform - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\xscpu64.exe"
	-@erase "$(OUTDIR)\xscpu64.ilk"
	-@erase "$(OUTDIR)\xscpu64.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\platform" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\xscpu64.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

.c{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xscpu64.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib opengl32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\xscpu64.pdb" /debug /machine:MIPS /nodefaultlib:"msvcrt.lib" /out:"$(OUTDIR)\xscpu64.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	".\libs\arch\Debug\arch.lib" \
	".\libs\base\Debug\base.lib" \
	".\libs\c64scpu64\Debug\c64scpu64.lib" \
	".\libs\scpu64\Debug\scpu64.lib" \
	".\libs\drive\Debug\drive.lib" \
	".\libs\fileio\Debug\fileio.lib" \
	".\libs\fsdevice\Debug\fsdevice.lib" \
	".\libs\gfxoutputdrv\Debug\gfxoutputdrv.lib" \
	".\libs\iec\Debug\iec.lib" \
	".\libs\c64exp\Debug\c64exp.lib" \
	".\libs\iecieee\Debug\iecieee.lib" \
	".\libs\ieee\Debug\ieee.lib" \
	".\libs\parallel\Debug\parallel.lib" \
	".\libs\printerdrv\Debug\printerdrv.lib" \
	".\libs\rs232drv\Debug\rs232drv.lib" \
	".\libs\serial\Debug\serial.lib" \
	".\libs\sid\Debug\sid.lib" \
	".\libs\sounddrv\Debug\sounddrv.lib" \
	".\libs\viciisc\Debug\viciisc.lib" \
	".\libs\rtc\Debug\rtc.lib" \
	".\libs\raster\Debug\raster.lib" \
	".\libs\vdrive\Debug\vdrive.lib" \
	".\libs\video\Debug\video.lib" \
	".\libs\monitor\Debug\monitor.lib" \
	".\libs\c64cart\Debug\c64cart.lib" \
	".\libs\c64cartsystem\Debug\c64cartsystem.lib" \
	".\libs\c64commoncart\Debug\c64commoncart.lib" \
	".\libs\diskimage\Debug\diskimage.lib" \
	".\libs\imagecontents\Debug\imagecontents.lib" \
	".\libs\core\Debug\core.lib" \
	".\libs\userport\Debug\userport.lib" \
	".\libs\rtc\Debug\rtc.lib" \
	".\libs\iecbus\Debug\iecbus.lib" \
	".\libs\p64\Debug\p64.lib" \
	".\libs\platform\Debug\platform.lib"

"$(OUTDIR)\xscpu64.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "xscpu64 - Win32 Release" || "$(CFG)" == "xscpu64 - Win32 Debug"

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"arch - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Release" 
   cd "."

"arch - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"arch - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Debug" 
   cd "."

"arch - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"c64scpu64 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64scpu64.mak" CFG="c64scpu64 - Win32 Release" 
   cd "."

"c64scpu64 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64scpu64.mak" CFG="c64scpu64 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"c64scpu64 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64scpu64.mak" CFG="c64scpu64 - Win32 Debug" 
   cd "."

"c64scpu64 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64scpu64.mak" CFG="c64scpu64 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"scpu64 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\scpu64.mak" CFG="scpu64 - Win32 Release" 
   cd "."

"scpu64 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\scpu64.mak" CFG="scpu64 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"scpu64 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\scpu64.mak" CFG="scpu64 - Win32 Debug" 
   cd "."

"scpu64 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\scpu64.mak" CFG="scpu64 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"drive - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Release" 
   cd "."

"drive - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"drive - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Debug" 
   cd "."

"drive - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"fileio - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fileio.mak" CFG="fileio - Win32 Release" 
   cd "."

"fileio - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fileio.mak" CFG="fileio - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"fileio - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fileio.mak" CFG="fileio - Win32 Debug" 
   cd "."

"fileio - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fileio.mak" CFG="fileio - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"fsdevice - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fsdevice.mak" CFG="fsdevice - Win32 Release" 
   cd "."

"fsdevice - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fsdevice.mak" CFG="fsdevice - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"fsdevice - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fsdevice.mak" CFG="fsdevice - Win32 Debug" 
   cd "."

"fsdevice - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\fsdevice.mak" CFG="fsdevice - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"gfxoutputdrv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gfxoutputdrv.mak" CFG="gfxoutputdrv - Win32 Release" 
   cd "."

"gfxoutputdrv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gfxoutputdrv.mak" CFG="gfxoutputdrv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"gfxoutputdrv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gfxoutputdrv.mak" CFG="gfxoutputdrv - Win32 Debug" 
   cd "."

"gfxoutputdrv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gfxoutputdrv.mak" CFG="gfxoutputdrv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"iec - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Release" 
   cd "."

"iec - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"iec - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Debug" 
   cd "."

"iec - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"c64exp - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Release" 
   cd "."

"c64exp - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"c64exp - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Debug" 
   cd "."

"c64exp - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"iecieee - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Release" 
   cd "."

"iecieee - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"iecieee - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Debug" 
   cd "."

"iecieee - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"ieee - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Release" 
   cd "."

"ieee - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"ieee - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Debug" 
   cd "."

"ieee - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"parallel - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Release" 
   cd "."

"parallel - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"parallel - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Debug" 
   cd "."

"parallel - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"printerdrv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\printerdrv.mak" CFG="printerdrv - Win32 Release" 
   cd "."

"printerdrv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\printerdrv.mak" CFG="printerdrv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"printerdrv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\printerdrv.mak" CFG="printerdrv - Win32 Debug" 
   cd "."

"printerdrv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\printerdrv.mak" CFG="printerdrv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"rs232drv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Release" 
   cd "."

"rs232drv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"rs232drv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Debug" 
   cd "."

"rs232drv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"serial - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Release" 
   cd "."

"serial - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"serial - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Debug" 
   cd "."

"serial - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"sid - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Release" 
   cd "."

"sid - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"sid - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Debug" 
   cd "."

"sid - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"sounddrv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Release" 
   cd "."

"sounddrv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"sounddrv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Debug" 
   cd "."

"sounddrv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"viciisc - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\viciisc.mak" CFG="viciisc - Win32 Release" 
   cd "."

"viciisc - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\viciisc.mak" CFG="viciisc - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"viciisc - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\viciisc.mak" CFG="viciisc - Win32 Debug" 
   cd "."

"viciisc - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\viciisc.mak" CFG="viciisc - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"rtc - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Release" 
   cd "."

"rtc - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"rtc - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Debug" 
   cd "."

"rtc - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"raster - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Release" 
   cd "."

"raster - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"raster - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Debug" 
   cd "."

"raster - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"vdrive - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Release" 
   cd "."

"vdrive - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"vdrive - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Debug" 
   cd "."

"vdrive - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"video - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Release" 
   cd "."

"video - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"video - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Debug" 
   cd "."

"video - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"monitor - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Release" 
   cd "."

"monitor - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"monitor - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Debug" 
   cd "."

"monitor - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"c64cart - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cart.mak" CFG="c64cart - Win32 Release" 
   cd "."

"c64cart - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cart.mak" CFG="c64cart - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"c64cart - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cart.mak" CFG="c64cart - Win32 Debug" 
   cd "."

"c64cart - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cart.mak" CFG="c64cart - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"c64cartsystem - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cartsystem.mak" CFG="c64cartsystem - Win32 Release" 
   cd "."

"c64cartsystem - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cartsystem.mak" CFG="c64cartsystem - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"c64cartsystem - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cartsystem.mak" CFG="c64cartsystem - Win32 Debug" 
   cd "."

"c64cartsystem - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64cartsystem.mak" CFG="c64cartsystem - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"c64commoncart - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64commoncart.mak" CFG="c64commoncart - Win32 Release" 
   cd "."

"c64commoncart - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64commoncart.mak" CFG="c64commoncart - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"c64commoncart - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64commoncart.mak" CFG="c64commoncart - Win32 Debug" 
   cd "."

"c64commoncart - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64commoncart.mak" CFG="c64commoncart - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"diskimage - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Release" 
   cd "."

"diskimage - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"diskimage - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Debug" 
   cd "."

"diskimage - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"imagecontents - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\imagecontents.mak" CFG="imagecontents - Win32 Release" 
   cd "."

"imagecontents - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\imagecontents.mak" CFG="imagecontents - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"imagecontents - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\imagecontents.mak" CFG="imagecontents - Win32 Debug" 
   cd "."

"imagecontents - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\imagecontents.mak" CFG="imagecontents - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"core - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Release" 
   cd "."

"core - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"core - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Debug" 
   cd "."

"core - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"iecbus - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Release" 
   cd "."

"iecbus - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"iecbus - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Debug" 
   cd "."

"iecbus - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"userport - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\userport.mak" CFG="userport - Win32 Release" 
   cd "."

"userport - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\userport.mak" CFG="userport - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"userport - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\userport.mak" CFG="userport - Win32 Debug" 
   cd "."

"userport - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\userport.mak" CFG="userport - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"rtc - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Release" 
   cd "."

"rtc - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"rtc - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Debug" 
   cd "."

"rtc - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rtc.mak" CFG="rtc - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"p64 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\p64.mak" CFG="p64 - Win32 Release" 
   cd "."

"p64 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\p64.mak" CFG="p64 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"p64 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\p64.mak" CFG="p64 - Win32 Debug" 
   cd "."

"p64 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\p64.mak" CFG="p64 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "xscpu64 - Win32 Release"

"platform - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\platform.mak" CFG="platform - Win32 Release" 
   cd "."

"platform - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\platform.mak" CFG="platform - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "xscpu64 - Win32 Debug"

"platform - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\platform.mak" CFG="platform - Win32 Debug" 
   cd "."

"platform - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\platform.mak" CFG="platform - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\main.c

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

