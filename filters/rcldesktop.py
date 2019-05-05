#!/usr/bin/env python3
import sys
from pypinyin import lazy_pinyin
import os

from xdg.DesktopEntry import DesktopEntry
from xdg.IconTheme import getIconPath


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
    AppName=desktop.getName()
    NoDisplay=False;
    if len(AppName)==0:
        AppName=desktop.getGenericName
    AppComment=desktop.getComment()
    AppIcon=desktop.getIcon()
    if not os.path.exists(AppIcon):
        ip=getIconPath(AppIcon,theme="deepin")
        # if ip is None:
        #     ip=getIconPath(AppIcon,theme="hicolor")
    else:
        ip=AppIcon

    if ip is None:
        #TODO add defauult icon
        ip="test"
        pass
    AppIcon=ip


    NoDisplay=desktop.getNoDisplay()
    onlydi=desktop.getOnlyShowIn()

    if NoDisplay is True or len(onlydi)>0:
        AppIcon=""
        AppName=""
        AppComment=""
        NoDisplay="true"
    else:
        NoDisplay="false"




    print('''
<html><head>
<title>''' +name+
          ''''</title>
<meta http-equiv="Content-Type" content="text/html;charset=UTF-8" >
<meta name="AppName" content="'''+AppName+'''" />
<meta name="AppComment" content="'''+AppComment+'''" />
<meta name="AppIcon" content="'''+AppIcon+'''" />
<meta name="AppNoDisplay" content="'''+NoDisplay+'''" />

</head>
<body></body>
</html>

''')



if __name__=="__main__":
    main(sys.argv)
