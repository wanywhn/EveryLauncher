import QtQuick.Controls 2.0
import QtQuick 2.0

Rectangle{
    height:btnFilter.height+btnText.height
    property alias text: btnText.text
    Rectangle{
        id:btnFilter
        anchors{
            top:parent.top
            left: parent.left
            right: parent.right
            topMargin: 16
            leftMargin: 16
            rightMargin: 16
        }
        TabBar {
             id: bar
             anchors.fill:parent
             TabButton {
                 text: qsTr("ALL")
             }
             TabButton {
                 text: qsTr("Programs")
             }
             TabButton {
                 text: qsTr("Picture")
             }
         }
        //TODO filter results


    }
    TextField{
        id:btnText
        anchors{
            top:btnFilter.bottom
            left: parent.left
            right: parent.right
            topMargin: 16
            leftMargin: 16
            rightMargin: 16
        }
        placeholderText: "query"
        Keys.onReturnPressed: {
            resListView.focus=true
        }
        focus: true
    }

}
