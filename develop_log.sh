#!/bin/sh


LOG_FILE=


###########################################################
# While using this shell script, I can record all my action
# for my projects and record the schedule.
###########################################################


log_schedule ()
{
	echo "[ Schedule ]" >> $LOG_FILE
	flag=0

    while [ 1 ]
	do
		echo ">> Input the schedule"
		read ret

		if [[ "y" != "$ret" ]]
		then
			if [ 0 -eq $flag ]
			then
	    		echo ">> No schedule"
				echo "** No schedule **" >> $LOG_FILE
			fi
			return 0
		fi

		vim .tmp.log
		SIZE=`ls -l .tmp.log | awk '{print $5}'`

		if [[ 1 -ge $SIZE ]]
    	then
			if [ 0 -eq $flag ]
			then
	    		echo ">> No schedule"
				echo "** No schedule **" >> $LOG_FILE
			fi
			return 0
    	fi

		flag=`expr $flag + 1`
		echo -e "$flag : \c" >> $LOG_FILE
		cat .tmp.log >> $LOG_FILE

		echo > .tmp.log
	done
}


LOG_FILE=$1

if [ ! -n "$LOG_FILE" ]
then
	LOG_FILE=".develop.log"
fi

if [ ! -e $LOG_FILE ]
then
	touch $LOG_FILE
	echo "[Create] .develop.log"
fi

echo "" >> $LOG_FILE

TIME=`date`
echo "-- $TIME --" >> $LOG_FILE

log_schedule
