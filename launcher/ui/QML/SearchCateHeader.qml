import QtQuick.Controls 2.0
import QtQuick 2.0

Rectangle{
    CategroyFilter{
        id:categroyFilter
//            z:1
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin:16
            leftMargin: 16
            rightMargin: 16
        }
    }
TextField{

        //            width:listView.width
        placeholderText: "query"

        Keys.onReturnPressed: {
            resListView.focus=true
        }

    }

}
