#!/bin/bash


usage() {
    echo "USAGE: clangformatdiff.sh [OPTIONS] <filenames>"
    echo "  <filenames> is a whitespace-separated list of files to be checked."
    echo ""
    echo "OPTIONS:"
    echo "  -h          Print this message and exit"
    echo "  -f          Use .clang-format style file"
    echo "  -s <style>  Use <style> (default: LLVM)"
    echo ""
    echo "  The -f and -s options are exclusive."
    echo ""
    echo "RETURNS:"
    echo "  0  if all files conform to the formatting style"
    echo "  1  if any difference is found"
    exit 1
}


while getopts "fs:h" arg; do
    case "${arg}" in
        f)
            usefile="TRUE"
            ;;
        s)
            style="${OPTARG}"
            ;;
        h)
            usage
            ;;
    esac
done

shift $((OPTIND-1))

if [ $# -eq 0 ] ; then
    >&2 echo "Error: missing filenames"
    >&2 echo ""
    usage
fi

if [ "$usefile" -a "$style" ] ; then
    >&2 echo "Error: -f and -s options are exclusive"
    >&2 echo ""
    usage
fi

if [ "$usefile" ] ; then
    cmd='clang-format -style=file "$filename" | diff "$filename" -'
elif [ "$style" ] ; then
    cmd='clang-format -style=$style "$filename" | diff "$filename" -'
else
    cmd='clang-format -style=LLVM "$filename" | diff "$filename" -'
fi


result=0
output=""

for filename in $@; do
    if [ -d "$filename" ] ; then
        echo "Error: cannot process directories"
        echo "  $filename"
        exit 2
    fi

    if [ ! -f "$filename" ] ; then
        echo "Error: no such file"
        echo "  $filename"
        exit 2
    fi

    current_output=$(clang-format -style=LLVM "$filename" | diff "$filename" -)

    # Escape '%' chars, as we are using printf
    current_output=$(echo "$current_output" | sed -e 's/%/%%/g')

    if [ ! -z "$current_output" ] ; then
        if [ -z "$output" ] ; then
            output=$(printf "${filename}:")
        else
            output=$(printf "${output}\n\n${filename}:")
        fi
        output=$(printf "${output}\n${current_output}")
        output=$(printf "${output}")
        result=1
    fi
done

if [ $result -eq 0 ] ; then
    echo "clangformatdiff.sh: files conform to format"
else
    >&2 echo "Error: differences found"
    >&2 echo ""
fi

printf "$output\n"


exit $result
