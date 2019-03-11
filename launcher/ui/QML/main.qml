import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0



ApplicationWindow {
    id: root

    width: 640
    height: 480
    visible: true
    title: "CPU Load"
    flags: Qt.FramelessWindowHint


    Page{
        anchors.fill:parent

        header:SearchCateHeader{
            id:queryHeader
        }

        contentItem:
            ListView {
            id:resListView
            anchors{
                fill: parent
                topMargin:70
                leftMargin: 16
                rightMargin: 16
            }

           // model: EveryQueryModel{
           //     id:every_query_model;
           //     queryText:queryHeader.text
           // }
           model:filterModel
           Binding{
               target:queryModel;
               property:"queryText";
               value:queryHeader.text
           }

            delegate:Rectangle {
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
            ScrollBar.vertical:ScrollBar{}
            highlightFollowsCurrentItem: true
        }
    }
}
