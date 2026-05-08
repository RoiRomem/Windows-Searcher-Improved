@echo off
setlocal
cls

cd /d "%~dp0"
set BUILD_DIR=build
set CLANG_PATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin

set BUILD_DIR=build

echo [!] Starting the build process

:: check if build folder exists, if not, create it
if not exist %BUILD_DIR% (
    echo [!] Creating build directory...
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

:: nuking the cache
if exist CMakeCache.txt del /q CMakeCache.txt

echo [!] Running CMake configuration..
cmake -G Ninja ^
-DCMAKE_CXX_COMPILER="%CLANG_PATH%/clang++.exe" ^
-DCMAKE_C_COMPILER="%CLANG_PATH%/clang.exe" ^
-DCMAKE_RC_COMPILER="%CLANG_PATH%/llvm-rc.exe" ..

if %errorlevel% neq 0 (
    echo [X] CMake configuration failed. Check your CMakeLists.txt.
    pause
    exit /b %errorlevel%
)

echo [!] Compiling project...
cmake --build .

if %errorlevel% neq 0 (
    echo [X] Compilation failed. Check the errors above.
    pause
    exit /b %errorlevel%
)

echo [!] W secured. Launching WSI.exe...
start bin/WSI.exe

endlocal