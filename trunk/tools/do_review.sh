#!/bin/sh

OFILE=.tmp.do_review.$$

function cleanup
{
	rm -f $OFILE
}

function abort
{
	echo "code review aborted"
	cleanup
	exit 1
}


cat > $OFILE << __EOM__
The following files will become part of this code review.  Any files that
you do not want to include should be removed now.

If you want to cancel this review, simply remove all of the files.


===--- file list only below this line ---===
__EOM__

svn status | grep -v "^?" >> $OFILE

if [ -n "$EDITOR" ]; then
	ED=$EDITOR
elif [ -n "$VISUAL" ]; then
	ED=$VISUAL
else
	ED=vi
fi
if ! $ED $OFILE; then
	abort
fi

FILES=$(awk -v MAGIC="$MAGIC" '
	{
		if (go) {
			print $2;
		}
	}
	/^===--- .* ---===/ {
		echo YES;
		go = 1;
	}' $OFILE)
if [ -z "$FILES" ]; then
	abort
fi

if [ -z "$GOOGLE_USER" ]; then
	echo -ne "what is your google username (e.g. user@gmail.com):  "
	read GOOGLE_USER
fi
./tools/upload.py -e $GOOGLE_USER -- $FILES

cleanup
exit 0
