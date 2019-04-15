#!/usr/bin/env python3
import sys
from pypinyin import lazy_pinyin

from xdg.DesktopEntry import DesktopEntry


def main(args):
    args =args[1:]
    if len(args )!=1:
        print("takes one argument")
        return
    try:
        desktop =DesktopEntry()
        desktop.parse(args[0])
    except:
        print("parse error" ,args[0])

    name =desktop.getName()
    name =''.join(lazy_pinyin(name))
    print('''
<html><head>
<title>''' +name+
          ''''</title>
<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
</head>
<body></body>
</html>

''')



if __name__=="__main__":
    main(sys.argv)
