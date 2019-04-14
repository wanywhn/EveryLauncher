#!/bin/sh
# @(#$Id: rclscribus,v 1.4 2007-06-08 13:51:09 dockes Exp $  (C) 2004 J.F.Dockes
# There may still be code from Estraier in here:
#================================================================
# Estraier: a personal full-text search system
# Copyright (C) 2003-2004 Mikio Hirabayashi
#================================================================
#================================================================
# Convert a scribus file to recoll HTML. This only handles the newer .sla
# files until I can have a look at an older .scd.
#
# We just hack into the scribus XML, taking advantage that the tag of
# interest is apparently always output on a single line.
# The text seems to be found in attribute CH of tag ITEXT, it is utf-8
#
# Tried to convert this to xsltproc but it seems that quite a few
# Scribus document are not actually proper xml

# set variables
LANG=C ; export LANG
LC_ALL=C ; export LC_ALL
progname="rcldesktop"
filetype=desktop



#RECFILTCOMMONCODE
##############################################################################
# !! Leave the previous line unmodified!! Code imported from the
# recfiltcommon file

# Utility code common to all shell filters. This could be sourced at run
# time, but it's slightly more efficient to include the code in the
# filters at build time (with a sed script).

# Describe error in a way that can be interpreted by our caller
senderror()
{
    echo RECFILTERROR $*
    # Also alert on stderr just in case
    echo ":2:$progname::: $*" 1>&2
    exit 1
}

iscmd()
{
    cmd=$1
    case $cmd in
    */*)
	if test -x $cmd -a ! -d $cmd ; then return 0; else return 1; fi ;;
    *)
      oldifs=$IFS; IFS=":"; set -- $PATH; IFS=$oldifs
      for d in $*;do test -x $d/$cmd -a ! -d $d/$cmd && return 0;done
      return 1 ;;
    esac
}

checkcmds()
{
    for cmd in $*;do
      if iscmd $cmd 
      then 
        a=1
      else 
        senderror HELPERNOTFOUND $cmd
      fi
    done
}

# show help message
if test $# -ne 1 -o "$1" = "--help" 
then
  echo "Convert a $filetype file to HTML text for Recoll indexing."
  echo "Usage: $progname [infile]"
  exit 1
fi

infile="$1"

# check the input file existence (may be '-' for stdin)
if test "X$infile" != X- -a ! -f "$infile"
then
  senderror INPUTNOSUCHFILE "$infile"
fi

# protect access to our temp files and directories
umask 77

##############################################################################
# !! Leave the following line unmodified !
#ENDRECFILTCOMMONCODE

checkcmds grep awk sed

# A small sed program to join lines where they are broken inside an
# attribute value. The idea is that all scribus tag are apparently on one
# line except when there are embedded new lines in an attribute lie
# 'comments'. The first version of the sed script joins line which does not
# end with > with the next. It doesn't guard against an embedded '>'. The
# seconf joins line not beginning with '<' with the previous. It is much
# slower for some reason.
sedjoinprog=':a
/[^>] *$/N; s/\n/ /; ta'
#sedjoinprog1=':a
#$!N;/^ *[^<]/s/\n/ /;ta
#P;D'

# TODO assume the first is the correct entry
# Extract description title author and keywords
name=`cat $infile |\
awk -F'=' '
BEGIN{
find=0
}
/Name/ {
if(find==0){
       printf("%s", $2);
       find=1;
       }
}
'
`

icon=`cat $infile|\
awk -F'=' '
BEGIN{
find=0
}
/Icon/ {
if(find==0){
       printf("%s", $2);
       find=1;
       }
}
'
`

ec=`cat $infile|\
awk -F'=' '
BEGIN{
find=0
}
/Exec/{
if(find==0){
    printf("%s",$2);
    find=1;
    }
}
'
`


comment=`cat $infile|\
awk -F'=' '
BEGIN{
find=0
}
/Comment/{
if(find==0){
    printf("%s",$2);
    find=1;
    }
}
'
`

#echo description: [$description];echo title: [$title];
#echo author: [$author];echo keywords: [$keywords]

cat <<EOF
<html><head>
<title>$name</title>
<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
<meta name="exec" content="$ec">
<meta name="icon" content="$icon">
<meta name="comment" content="$comment">
</head>
<body></body>
</html>
EOF


