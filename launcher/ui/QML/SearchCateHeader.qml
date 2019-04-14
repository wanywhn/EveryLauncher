import QtQuick.Controls 2.0
import QtQuick 2.0

Rectangle{
    height:btnFilter.height+btnText.height
    property alias text: btnText.text
    property string modifiedText: ""

    id:root

    signal preferenceClicked
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
            right: btnSettings.left
//            right: parent.right
            topMargin: 16
            leftMargin: 16
            rightMargin: 16
        }
        placeholderText: "query"
        Keys.onReturnPressed: {
            resListView.focus=true
        }
        focus: true
        onTextChanged: {
            var str=""
            var t=btnText.text
            for (var i=0;i<t.length;++i){
                str+=t.charAt(i)
                str+="*"
            }

            root.modifiedText=str
        }
    }
    Button{
        id:btnSettings
        anchors{
            top:btnFilter.bottom
            right: parent.right
        }
        text: "Settings"
        onClicked: {
            preferenceClicked()
        }
    }

}
