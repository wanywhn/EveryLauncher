import QtQuick.Controls 2.3
import QtQuick 2.0
import QtQuick.Controls.Material 2.12

Rectangle{
    id:root
    height:btnText.height
    signal sreturnPressed
    signal preferenceClicked
    property alias text: btnText.text
    property string modifiedText: ""

   // Rectangle{
   //     id:btnFilter
   //     height: bar.height
   //     anchors{
   //         top:parent.top
   //         left: parent.left
   //         right: parent.right
   //         topMargin: 16
   //         leftMargin: 16
   //         rightMargin: 16
   //     }
   //     TabBar {
   //          id: bar
   //          anchors.fill:parent
   //          TabButton {
   //              text: qsTr("ALL")
   //          }
   //          TabButton {
   //              text: qsTr("Programs")
   //          }
   //          TabButton {
   //              text: qsTr("Picture")
   //          }
   //      }
   //     //TODO filter results
   // }
    TextField{
        id:btnText
        anchors{
            top:parent.top
            left: parent.left
            right: btnSettings.left
//            right: parent.right
            topMargin: 16
            leftMargin: 16
            rightMargin: 16
        }
        placeholderText: "query"
        Keys.onReturnPressed: {
            root.sreturnPressed()
        }

        Component.onCompleted: {
            btnText.forceActiveFocus()
        }

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
        background: Rectangle{
            radius: 10
            color:  "#FF9800"
        }

        id:btnSettings
        anchors{
            top:parent.top
//            left: btnText.right
            right: parent.right
            topMargin: 16
            leftMargin: 16
            rightMargin: 16
        }
        text: "Settings"
        onClicked: {
            preferenceClicked()
        }
    }

}
