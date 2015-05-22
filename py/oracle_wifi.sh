#!/bin/sh

# usging newfox
content=`python /home/llfeng/oracle_wifi.py 2>/dev/null`
file=`mktemp`
cat >$file <<EOF
To:sps_BJ_DEV_CN_GRP@oracle.com
Subject:WiFi guest

$content

EOF
cat $file | /usr/sbin/sendmail -t -F "God" -r "li.feng@oracle.com"

cat $file | grep Password | head -n1 | sed -e 's/Password: //g'  | tr -d '\n' | /home/llfeng/bin/mproxy python /home/llfeng/weixin_post.py 2>/dev/null

curl -x http://140.83.73.9:80 -F filedata=@${file}  http://52.68.197.47:81/upload.php
rm $file
