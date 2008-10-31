@echo off
@echo Setting environment for using FSC SDK.

@SET SIMCONNECT_SDK_INSTALLDIR=C:\Program Files\Microsoft Games\Microsoft Flight Simulator X SDK\SDK\Core Utilities Kit\SimConnect SDK

@if "%VS90COMNTOOLS%"=="" goto ERROR_NO_VCINSTALLDIR

@set PATH=%CD%\tools\ntx86;%PATH%
@set INCLUDE=%SIMCONNECT_SDK_INSTALLDIR%\inc;%INCLUDE%
@set LIB=%SIMCONNECT_SDK_INSTALLDIR%\lib;%LIB%

@rem Setting environment for using Microsoft Visual Studio.
"%VS90COMNTOOLS%\vsvars32.bat"

@goto end

:ERROR_NO_VCINSTALLDIR
@echo ERROR: VS90COMNTOOLS variable is not set. 
@goto end

:end