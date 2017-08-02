mkdir koferta_x86
cd koferta_x86
mkdir bin
cp ../build/Release/koferta.exe ./bin/
cp ../build/Logger/Release/logger.dll ./bin/
cp ../build/MainWindow/Release/mainwindow.dll ./bin/
cp ../build/MainWindow/*.qm ./bin/
cp ../3rdParty/* ./bin/
cp ../LICENSE.md ./
cp ../README.md ./
cp ../koferta.bat ./
cd bin
cp "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll" ./
cp "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll" ./
cp C:\Qt/5.7/msvc2013/bin/Qt5Core.dll ./
cp C:\Qt/5.7/msvc2013/bin/Qt5Gui.dll ./
cp C:\Qt/5.7/msvc2013/bin/Qt5PrintSupport.dll ./
cp C:\Qt/5.7/msvc2013/bin/Qt5Sql.dll ./
cp C:\Qt/5.7/msvc2013/bin/Qt5Widgets.dll ./
cp C:\Qt/5.7/msvc2013/bin/Qt5Network.dll ./
mkdir platforms
cp C:\Qt/5.7/msvc2013/plugins/platforms/qwindows.dll ./platforms/
mkdir printsupport
cp C:\Qt/5.7/msvc2013/plugins/printsupport/windowsprintersupport.dll ./printsupport/
mkdir sqldrivers
cp C:\Qt/5.7/msvc2013/plugins/sqldrivers/qsqlmysql.dll ./sqldrivers/
cd ../..
7z a kOferta_x86.7z koferta_x86
