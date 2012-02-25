#!/bin/sh
set -x
echo "hi" >> test.txt
remote_host="nelson.datamaniacs.net"
remote_user="diederick"
remote_dir="/media/data/backup/ted2012/"
dt=$(date +%Y-%m-%d-%H-%M-%S)
file="${dt}_twitter_db_backup.db"
if [ ! -f ${file} ] ; then
	cp "twitter.db" ${file}
	bzip2 ${file} 
	scp ${file}.bz2 ${remote_user}@${remote_host}:${remote_dir}
fi
echo ${dt}
