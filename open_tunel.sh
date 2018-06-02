#!/bin/sh
ssh -M -v 46.105.27.6 -p 2292 -l konserw -i ~/.ssh/koferta_rsa -fN -L 13306:46.105.27.6:3306 -o ExitOnForwardFailure=yes -o StrictHostKeyChecking=no localhost 
netstat -tulpn |grep 3306

