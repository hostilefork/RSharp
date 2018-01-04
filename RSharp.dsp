# Microsoft Developer Studio Project File - Name="RSharp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=RSharp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RSharp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RSharp.mak" CFG="RSharp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RSharp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "RSharp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RSharp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "./includes" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /profile /incremental:yes /map

!ELSEIF  "$(CFG)" == "RSharp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Za /W3 /Gm /GX /ZI /Od /I "./includes" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /map /debug /machine:I386
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "RSharp - Win32 Release"
# Name "RSharp - Win32 Debug"
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\includes\core_boot.h
# End Source File
# Begin Source File

SOURCE=.\includes\core_console.h
# End Source File
# Begin Source File

SOURCE=.\includes\core_debug.h
# End Source File
# Begin Source File

SOURCE=.\includes\core_eval.h
# End Source File
# Begin Source File

SOURCE=.\includes\core_globals.h
# End Source File
# Begin Source File

SOURCE=.\includes\core_mem.h
# End Source File
# Begin Source File

SOURCE=.\includes\core_symbol.h
# End Source File
# Begin Source File

SOURCE=.\includes\core_tokenizer.h
# End Source File
# Begin Source File

SOURCE=.\includes\natives.h
# End Source File
# Begin Source File

SOURCE=.\includes\os.h
# End Source File
# Begin Source File

SOURCE=.\includes\refinement_defines.h
# End Source File
# Begin Source File

SOURCE=.\includes\rsharp.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_action.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_anyfunc.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_block.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_char.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_context.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_datatype.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_error.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_function.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_integer.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_misc.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_native.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_op.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_paren.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_path.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_refinement.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_series.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_string.h
# End Source File
# Begin Source File

SOURCE=.\includes\type_word.h
# End Source File
# End Group
# Begin Group "types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base\types\action.c
# End Source File
# Begin Source File

SOURCE=.\base\types\anyfunc.c
# End Source File
# Begin Source File

SOURCE=.\base\types\block.c
# End Source File
# Begin Source File

SOURCE=.\base\types\char.c
# End Source File
# Begin Source File

SOURCE=.\base\types\context.c
# End Source File
# Begin Source File

SOURCE=.\base\types\datatype.c
# End Source File
# Begin Source File

SOURCE=.\base\types\error.c
# End Source File
# Begin Source File

SOURCE=.\base\types\function.c
# End Source File
# Begin Source File

SOURCE=.\base\types\integer.c
# End Source File
# Begin Source File

SOURCE=.\base\types\misc.c
# End Source File
# Begin Source File

SOURCE=.\base\types\native.c
# End Source File
# Begin Source File

SOURCE=.\base\types\op.c
# End Source File
# Begin Source File

SOURCE=.\base\types\paren.c
# End Source File
# Begin Source File

SOURCE=.\base\types\path.c
# End Source File
# Begin Source File

SOURCE=.\base\types\refinement.c
# End Source File
# Begin Source File

SOURCE=.\base\types\series.c
# End Source File
# Begin Source File

SOURCE=.\base\types\string.c
# End Source File
# Begin Source File

SOURCE=.\base\types\word.c
# End Source File
# End Group
# Begin Group "scripts"

# PROP Default_Filter "*.rs"
# Begin Source File

SOURCE=.\boot.rs
# End Source File
# End Group
# Begin Source File

SOURCE=.\base\boot.c
# End Source File
# Begin Source File

SOURCE=.\base\console.c
# End Source File
# Begin Source File

SOURCE=.\base\debug.c
# End Source File
# Begin Source File

SOURCE=.\base\eval.c
# End Source File
# Begin Source File

SOURCE=.\base\mem.c
# End Source File
# Begin Source File

SOURCE=.\base\symbol.c
# End Source File
# Begin Source File

SOURCE=.\base\tokenizer.c
# End Source File
# Begin Source File

SOURCE=.\base\os\unix.c
# End Source File
# End Group
# End Target
# End Project
