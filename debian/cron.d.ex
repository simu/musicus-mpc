#
# Regular cron jobs for the musicus package
#
0 4	* * *	root	[ -x /usr/bin/musicus_maintenance ] && /usr/bin/musicus_maintenance
