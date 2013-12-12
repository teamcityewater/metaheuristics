@echo off

echo. 
echo Copying NuGet package spec from CSIRO.Metaheuristics to other assemblies.
echo.

set THIS_DIR=%~d0%~p0
REM set THIS_DIR=F:\src\github_jm\metaheuristics\build\
set src_nuspec=%THIS_DIR%..\CSIRO.Metaheuristics\CSIRO.Metaheuristics.nuspec
if not exist "%src_nuspec%" goto noSrcNuspec
set COPYOPTIONS=/Y /R /D

xcopy %src_nuspec% %THIS_DIR%..\CSIRO.Utilities\CSIRO.Utilities.nuspec %COPYOPTIONS%
xcopy %src_nuspec% %THIS_DIR%..\CSIRO.Metaheuristics.DataModel\CSIRO.Metaheuristics.DataModel.nuspec %COPYOPTIONS%
xcopy %src_nuspec% %THIS_DIR%..\CSIRO.Metaheuristics.Logging\CSIRO.Metaheuristics.Logging.nuspec %COPYOPTIONS%
xcopy %src_nuspec% %THIS_DIR%..\CSIRO.Metaheuristics.Parallel\CSIRO.Metaheuristics.Parallel.nuspec %COPYOPTIONS%
xcopy %src_nuspec% %THIS_DIR%..\CSIRO.Metaheuristics.R\CSIRO.Metaheuristics.R.nuspec %COPYOPTIONS%

nuget pack %THIS_DIR%..\CSIRO.Utilities\CSIRO.Utilities.csproj -IncludeReferencedProjects
nuget pack %THIS_DIR%..\CSIRO.Metaheuristics\CSIRO.Metaheuristics.csproj -IncludeReferencedProjects
nuget pack %THIS_DIR%..\CSIRO.Metaheuristics.DataModel\CSIRO.Metaheuristics.DataModel.csproj -IncludeReferencedProjects
nuget pack %THIS_DIR%..\CSIRO.Metaheuristics.Logging\CSIRO.Metaheuristics.Logging.csproj -IncludeReferencedProjects
nuget pack %THIS_DIR%..\CSIRO.Metaheuristics.Parallel\CSIRO.Metaheuristics.Parallel.csproj -IncludeReferencedProjects
nuget pack %THIS_DIR%..\CSIRO.Metaheuristics.R\CSIRO.Metaheuristics.R.csproj -IncludeReferencedProjects

if errorlevel 1 goto bailOut

goto success 

:noSrcNuspec
echo not found: source nuspec not found
exit /B 1

:bailOut
echo ERROR
exit /B 1

:success
echo Done!
