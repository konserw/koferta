start .\bin\plink.exe -v -ssh 46.105.27.6 -l sshUser -P 2292 -2 -4 -i sshUser.ppk -T -N -L 13306:46.105.27.6:3306
rem timeout /t 3
.\bin\koferta.exe
taskkill /im plink.exe
