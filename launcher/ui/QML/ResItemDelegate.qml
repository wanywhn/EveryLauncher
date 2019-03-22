import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0



Rectangle {
    property string css_style:"<style type=\"text/css\"> span{ font-weight:bold;color:#ff9955}</style> "
    

    MouseArea{
        anchors.fill: parent
        onClicked: {
            resListView.currentIndex=index
            //                        Qt.openUrlExternally(fileLocation)
            //desktopServices.openUrl(fileLocation)
            //queryModel.textextract(idx)
        }
    }
    
    Text{
        id: file_icon
        //                    source: ""
        text:"image"
        height: 60
        width: 60
        anchors{
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
    }
    Text {
        id: file_name
        text: fileName
        //                        wrapMode:
        anchors{
            left: file_icon.right
            top:parent.top
            //                            right:file_statu.left
        }
    }
    Text {
        id: file_location
        text: fileLocation
        //                        wrapMode:
        anchors{
            left: file_icon.right
            top:file_name.bottom
            right: parent.right
        }
    }
    Text{
        id: file_simple_content
        text:css_style+ fileSimpleContent
        
        
        textFormat: Text.RichText
        
        
        wrapMode:Text.WordWrap
        
        
        anchors{
            left: file_icon.right
            top:file_location.bottom
            right: parent.right
            bottom: parent.bottom
        }
    }
    //                    Text {
    //                        id: file_statu
    //                        text: "text"
    //                        anchors{
    //                            left: file_name.right
    //                            top:parent.top
    //                            bottom: file_location.top
    //                            right: short_key.left
    
    //                        }
    //                    }
    
    //                    Text {
    //                    anchors.fill: parent
    //                        id:short_key
    //                        height: 10
    //                        width: 10
    //                        text:"keyyyyyyyy"
    //                        anchors{
    //                            top:parent.top
    //                            right:parent.right
    //                            bottom:parent.bottom
    //                            bottom: file_location.top
    
    //                        }
    //                    }
}
