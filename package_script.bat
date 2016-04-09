mkdir koferta_x86
cd koferta_x86
cp ../build/Release/koferta.exe ./
cp ../build/MainWindow/Release/mainwindow.dll ./
cp ../build/MainWindow/*.qm ./
cp ../3rdParty/* ./
cp ../LICENSE.md ./
cp ../README.md ./
cp C:\Windows/System32/msvcp120.dll ./
cp C:\Windows/System32/msvcr120.dll ./
cp C:\Qt/5.5/msvc2013/bin/Qt5Core.dll ./
cp C:\Qt/5.5/msvc2013/bin/Qt5Gui.dll ./
cp C:\Qt/5.5/msvc2013/bin/Qt5PrintSupport.dll ./
cp C:\Qt/5.5/msvc2013/bin/Qt5Sql.dll ./
cp C:\Qt/5.5/msvc2013/bin/Qt5Widgets.dll ./
cp C:\Qt/5.5/msvc2013/bin/Qt5Network.dll ./
mkdir platforms
cp C:\Qt/5.5/msvc2013/plugins/platforms/qwindows.dll ./platforms/
mkdir printsupport
cp C:\Qt/5.5/msvc2013/plugins/printsupport/windowsprintersupport.dll ./printsupport/
mkdir sqldrivers
cp C:\Qt/5.5/msvc2013/plugins/sqldrivers/qsqlmysql.dll ./sqldrivers/
cd ..
7z a kOferta_x86.7z koferta_x86
