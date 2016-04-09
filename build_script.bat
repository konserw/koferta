@echo on
cmake -E make_directory build
cmake -E chdir build cmake ..
IF %APPVEYOR_REPO_TAG% EQU true (
echo %APPVEYOR_REPO_TAG_NAME%
cmake --build build --config Release -DKOFERTA_VERSION=%APPVEYOR_REPO_TAG_NAME%
) ELSE (
cmake --build build --config Release -DKOFERTA_VERSION=%APPVEYOR_REPO_BRANCH%-%APPVEYOR_BUILD_NUMBER%
echo %APPVEYOR_REPO_BRANCH%-%APPVEYOR_BUILD_NUMBER%
)
