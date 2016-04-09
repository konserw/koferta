cmake -E make_directory build
cmake -E chdir build cmake ..
IF DEFINED %APPVEYOR_REPO_TAG_NAME% (
echo %APPVEYOR_REPO_TAG_NAME%
cmake --build build --config Release -DKOFRTA_VERSION=%APPVEYOR_REPO_TAG_NAME%
) ELSE (
cmake --build build --config Release -DKOFRTA_VERSION=%APPVEYOR_REPO_BRANCH%-%APPVEYOR_BUILD_NUMBER%
echo %APPVEYOR_REPO_BRANCH%-%APPVEYOR_BUILD_NUMBER%
)
