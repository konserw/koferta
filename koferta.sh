#!/bin/sh
mkdir ./build/log
ssh -M -S ssh.sock -v 46.105.27.6 -p 2292 -l konserw -i ~/.ssh/koferta_rsa -fN -L 13306:46.105.27.6:3306 -o ExitOnForwardFailure=yes -o StrictHostKeyChecking=no localhost >./build/log/ssh.out 2>./build/log/ssh.err 
ssh -S ssh.sock -O check localhost 
netstat -tulpn |grep 3306
./build/src/koferta
echo closing tunnel
ssh -S ssh.sock -O exit localhost

