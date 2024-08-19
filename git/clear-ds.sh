#!/bin/bash
cd ..
if find . -iname ".ds_store" -print0 | xargs -0 git rm -f --ignore-unmatch; then
    echo -e "\nremoved all DS_Store files"
fi
: <<'END'

"find .":
This command starts the search from the current directory ("."). 
Replace "." with a specific directory path to search in a different location.

-iname ".ds_store":
This option specifies the name of the file to search for. 
The `-iname` flag performs a case-insensitive search for the file name ".DS_Store". 
If you want a case-sensitive search, use `-name` instead.

-print0:
This option tells `find` to print the file names followed by a null character (\0). 
It is useful when dealing with files that have special characters or spaces in their names.
A null character is used as a delimiter to separate the file names.

| (Pipe symbol):
Used to redirect the output of the `find` command to the input of the next command.

xargs -0:
The `xargs` command reads the input from the pipe and converts it into arguments for the subsequent command. 
The `-0` option tells `xargs` to expect null-separated input, which matches the output format of `find -print0`.

git rm -f --ignore-unmatch:
This command is executed for each file found by `find`. 
It removes the file from the Git repository using the `git rm` command. 
The `-f` option forces the removal even if the file is not currently tracked by Git. 
The `--ignore-unmatch` option prevents `git rm` from displaying an error message if the file is not found.

Essentially this searches for files named ".ds_store" case-insensitive, recursively, and for each file found, it removes 
it from the Git repository.

END|