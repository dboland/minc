#!/bin/sh

VERBOSE=
TARGET=
SOURCE=/mnt/c/minc-devel/src-master
PATTERN='*.[chS]'

diff_file()
{
	if [ "$VERBOSE" ]; then
		echo "$1"
	fi
	diff -Naur "$SOURCE/$1" "$1" | sed "s:$SOURCE/::"
}
diff_dir()
{
	IFS="
"
	if [ "$VERBOSE" ]; then
		echo "find $1 -type f -name $PATTERN"
	fi
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
	echo " -p,--pattern PATTERN\t\tuse PATTERN for file extension"
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
		-p|--pattern)
			PATTERN="$2"
			shift;
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
