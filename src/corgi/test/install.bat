@setlocal enableextensions
@cd /d "%~dp0"

for %%i in (x86 x64) do (

    setlocal

    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %%i
    
    for %%j in (Debug Release RelWithDebInfo) do (
        setlocal
        mkdir build\build%%i\%%j
        cd build\build%%i\%%j

        if %%i == x86 (
            cmake -DCMAKE_BUILD_TYPE=%%j -DCMAKE_INSTALL_PREFIX="C:\\Program Files\\corgi-test" -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -G "Visual Studio 15 2017" ..\..\..
        ) else (
            cmake -DCMAKE_BUILD_TYPE=%%j -DCMAKE_INSTALL_PREFIX="C:\\Program Files\\corgi-test" -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -G "Visual Studio 15 2017 Win64" ..\..\..
        )
        
        msbuild INSTALL.vcxproj /property:Configuration=%%j    
        endlocal
    )
    endlocal
)
pause