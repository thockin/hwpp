#!/bin/bash

# Arguments to this script are passed through to upload.py.
#
# For example, use tools/do_review.sh -i [issuenum] to add a new patch set
# to an existing issue.

PROJECT_LIST=pp-devel@googlegroups.com

OFILE=.tmp.do_review.$$

if [ -n "$EDITOR" ]; then
	EDIT=$EDITOR
elif [ -n "$VISUAL" ]; then
	EDIT=$VISUAL
else
	EDIT=vi
fi

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

if ! $EDIT $OFILE; then
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
	echo -ne "Google username (e.g. user@gmail.com): "
	read GOOGLE_USER
fi

echo -ne "Subject for this review: "
read SUBJECT

cat > $OFILE << __EOM__
<describe your change set here>
__EOM__

if ! $EDIT $OFILE; then
	abort
fi

echo >> $OFILE
echo >> $OFILE
svn diff $FILES | diffstat >> $OFILE

echo -ne "Send this review?(y/N) "
read YESNO
if [ "$YESNO" != "y" -a "$YESNO" != "Y" ]; then
	abort
fi

./tools/upload.py "$@" \
	-e "$GOOGLE_USER" \
	-m "$SUBJECT" \
	-f "$OFILE" \
	-r "$PROJECT_LIST" \
	--cc "$GOOGLE_USER" \
	--send_mail \
	-- $FILES

cleanup
exit 0
