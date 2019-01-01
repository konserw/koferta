mkdir koferta
cd koferta
mkdir bin
cp ../build/src/Release/koferta.exe ./bin/
cp ../build/src/Release/logger.dll ./bin/
cp ../build/src/Release/database.dll ./bin/
cp ../build/src/Release/mainwindow.dll ./bin/
cp ../build/src/*.qm ./bin/
cp ../3rdParty/* ./bin/
cp ../LICENSE.md ./
cp ../README.md ./
cp ../koferta.bat ./
cd bin
REM cp "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll" ./
REM cp "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll" ./
cp C:\Qt/5.11/msvc2017_64/bin/Qt5Core.dll ./
cp C:\Qt/5.11/msvc2017_64/bin/Qt5Gui.dll ./
cp C:\Qt/5.11/msvc2017_64/bin/Qt5PrintSupport.dll ./
cp C:\Qt/5.11/msvc2017_64/bin/Qt5Sql.dll ./
cp C:\Qt/5.11/msvc2017_64/bin/Qt5Widgets.dll ./
cp C:\Qt/5.11/msvc2017_64/bin/Qt5Network.dll ./
mkdir platforms
cp C:\Qt/5.11/msvc2017_64/plugins/platforms/qwindows.dll ./platforms/
mkdir printsupport
cp C:\Qt/5.11/msvc2017_64/plugins/printsupport/windowsprintersupport.dll ./printsupport/
mkdir sqldrivers
cp C:\Qt/5.11/msvc2017_64/plugins/sqldrivers/qsqlmysql.dll ./sqldrivers/
cd ../..
7z a kOferta.7z koferta
