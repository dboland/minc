#!/bin/sh

REVERT=
VERBOSE=
TARGET=
SOURCE=/mnt/d/openbsd-master
PATTERN='*.[chS]'
NOCHOP=

diff_file()
{
	if [ "$VERBOSE" ]; then
		echo "$1"
	fi
	# insert ^M with vi (Ctrl+V, followed by Ctrl+M)
	if [ "$NOCHOP" ]; then
		diff -Naur "$SOURCE/$1" "$1" | sed "s:$SOURCE/::"
	elif [ -n "$REVERT" ]; then
		sed 's///' "$1" | diff -Naur - "$SOURCE/$1" | sed "s:$SOURCE/::"
	else
		sed 's///' "$1" | diff -Naur "$SOURCE/$1" - | sed "s:$SOURCE/::"
	fi
}
diff_dir()
{
	IFS="
"
	if [ "$VERBOSE" ]; then
		echo "find $1 -type f -name $PATTERN"
	fi
#	for file in $(find "$1" -type f \! \( -name '*.o' \)); do
#	for file in $(find "$1" -type f -name "$PATTERN"); do
	for file in $(find "$1" -type f); do
		if [[ $file == *.o ]]; then
			continue
		elif ! diff_file "$file"; then
			exit 2
		fi
	done
}
do_usage()
{
	echo "Usage: $0: [options] PATH"
	echo
	echo "Options"
	echo " -s,--source\t\t\tspecify source directory"
	echo " -v,--verbose\t\t\tbe verbose"
	echo " -r,--revert\t\t\trevert target/source"
	echo " -p,--pattern PATTERN\t\tuse PATTERN for file extension"
	echo " -C,--nochop\t\t\tdon't chop carrage returns"
}

while [ -n "$1" ]; do
        case "$1" in
		-s|--source)
			SOURCE="$2"
			shift;
			;;
		-v|--verbose)
			VERBOSE=yes
			;;
		-r|--revert)
			REVERT=yes
			;;
		-p|--pattern)
			PATTERN="$2"
			shift;
			;;
		-C|--nochop)
			NOCHOP=yes
			;;
		*)
			TARGET="$1"
			;;
	esac
	shift
done

if [ -z "$TARGET" ]; then
	do_usage
elif [ -d "$TARGET" ]; then
	diff_dir "$TARGET"
elif [ -f "$TARGET" ]; then
	diff_file "$TARGET"
fi
