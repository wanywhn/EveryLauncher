import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0



Rectangle {
    height: 60
    Image {
        id: file_icon
        //                    source: ""
        height: 60
        width: 60
        anchors{
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
    Text {
        id: file_name
        text: fileName
        anchors{
            left: file_icon.right
            top:parent.top
        }
    }
    Text {
        id: file_location
        text: fileLocation
        anchors{
            left: file_icon.right
            top:file_name.bottom
            right: parent.right
        }
    }
    Text {
        //TODO Text browser
        id: file_simple_content
        text:fileSimpleContent
        
        anchors{
            left: file_icon.right
            top:file_location.bottom
            right: parent.right
        }
    }
    Text {
        id: file_statu
        text: qsTr("text")
        anchors{
            left: file_name.right
            top:parent.top
            bottom: file_location.top
            
        }
    }
    
    Text {
        //                    anchors.fill: parent
        id:short_key
        text:"key"
        anchors{
            top:parent.top
            right:parent.right
            
        }
    }
}
