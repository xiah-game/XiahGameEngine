@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat"
if %errorlevel% neq 0 (
    echo Failed to load Visual Studio environment.
    exit /b %errorlevel%
)
echo Building XiahGameEngine in Debug / x86 configuration...
msbuild XiahGameEngine.sln /p:Configuration=Debug /p:Platform=x86
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)
echo Build succeeded!
