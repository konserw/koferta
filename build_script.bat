@echo on
cmake -E make_directory build
IF %APPVEYOR_REPO_TAG% EQU true (
echo %APPVEYOR_REPO_TAG_NAME%
cmake -E chdir build cmake -DKOFERTA_VERSION=%APPVEYOR_REPO_TAG_NAME% ..
) ELSE (
echo %APPVEYOR_REPO_BRANCH%-%APPVEYOR_BUILD_NUMBER%
cmake -E chdir build cmake -DKOFERTA_VERSION=%APPVEYOR_REPO_BRANCH%-%APPVEYOR_BUILD_NUMBER% ..
)
cmake --build build --config Release
