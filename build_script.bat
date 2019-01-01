@echo on
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
set PATH=%PATH%;%QTDIR%\bin
IF %APPVEYOR_REPO_TAG% EQU true (
    set ver=%APPVEYOR_REPO_TAG_NAME%
) ELSE (
    set ver=%APPVEYOR_REPO_BRANCH%-%APPVEYOR_BUILD_NUMBER%
)
echo Setting version: %ver%

cmake -E make_directory build
cmake -E chdir build cmake -DKOFERTA_VERSION=%ver% -DKOFERTA_SQL_USER=%KOFERTA_SQL_USER% -DKOFERTA_SQL_PWD=%KOFERTA_SQL_PWD% -DCMAKE_GENERATOR_PLATFORM=x64 -DDISABLE_UNIT_TESTS=OFF ..
cmake --build build --config Release
