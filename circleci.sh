#!/bin/bash
set -x #display command being executed
. /opt/qt511/bin/qt511-env.sh
set -e #break script on non-zero exitcode from any command

DISPLAY=:99
export DISPLAY=$DISPLAY

startXvfb () {
    sudo Xvfb $DISPLAY -screen 0 640x480x8 -nolisten tcp > /dev/null 2>&1 &
    XVFBPID=$!
    sleep 5 
}

killXvfb () {
    if [ -n "${XVFBPID:-}" ]; then
        # Stop virtual X display server
        sudo kill $XVFBPID
        wait
    fi
}

CTEST_OUTPUT_ON_FAILURE=ON
export CTEST_OUTPUT_ON_FAILURE

cmake -E make_directory build
cmake -E chdir build cmake \
    -DKOFERTA_VERSION="${CIRCLE_BRANCH}-${CIRCLE_BUILD_NUM}" \
    -GNinja \
    -DKOFERTA_SQL_USER="${KOFERTA_SQL_USER}" \
    -DKOFERTA_SQL_PWD="${KOFERTA_SQL_PWD}" \
    ${COVERALLS_SERVICE_NAME:+"-DCMAKE_BUILD_TYPE=Debug"} \
    ${COVERALLS_SERVICE_NAME:+"-DCMAKE_CXX_FLAGS='--coverage'"} \
    ${VALGRIND_TESTS:+"-DVALGRIND_TESTS=${VALGRIND_TESTS}"} \
    ..
cmake --build build

#prepare test sql database
sudo bash ./database_scripts/run_db.sh
startXvfb # Start virtual X display server
#echo "Show databases;" | mysql
#echo "use kOferta_test; Show tables;" | mysql 
#and run tests
cmake --build build --target test

#build/kofertaSteps >/dev/null &
#cucumber features 

if [ -n "${COVERALLS_SERVICE_NAME}" ]
then
  lcov --rc lcov_branch_coverage=1 --directory build --capture --output-file coverage.info
  lcov --rc lcov_branch_coverage=1 --quiet --remove coverage.info \
      "${PWD}/build/*" \
      "${PWD}/tests/*" \
      "/usr/*" \
      "/opt/*" \
      --output-file coverage.info
  lcov --rc lcov_branch_coverage=1 --list coverage.info
  coveralls-lcov coverage.info
fi
