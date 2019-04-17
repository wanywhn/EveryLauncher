import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.12


Rectangle{
    id: root

    width: 640
//    height:mpage.height+32

    Keys.onDownPressed: {
        var o=resListView.currentIndex
        resListView.incrementCurrentIndex()
        if (o===resListView.currentIndex){
            resListView.currentIndex=0
        }

    }
    Keys.onUpPressed: {
        resListView.decrementCurrentIndex()
    }

    Behavior on height{
        PropertyAnimation{duration: 100}
    }

    Page{
        id:mpage
        anchors{
            left:parent.left
            top:parent.top
            right:parent.right
        }

//        height: queryHeader.height+resListView.height
        header:SearchCateHeader{
            id:queryHeader
            onPreferenceClicked: {
                systemTray.preferenceActionTriggered()
            }
        }

        Connections{
            target: queryHeader
            onSreturnPressed:{
                resListView.model.openFile(resListView.currentIndex)
            }
        }

        contentItem:
            ListView {
            id:resListView

            add: Transition {
                   NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
//                   NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
               }
            remove: Transition {
                   NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 400 }
            }

               displaced: Transition {
                   id:dispTrans
                   SequentialAnimation{
                       PauseAnimation{
                           duration: (dispTrans.ViewTransition.index-
                                      dispTrans.ViewTransition.targetIndex[0])*100
                       }
                   NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
                   }
               }

            anchors{
                top:parent.top
                left:parent.left
                right:parent.right
                topMargin:queryHeader.height+32
                leftMargin: 16
                rightMargin: 16
            }
            highlight:ItemHighlightBar{

            }

            onCountChanged: {
                /* calculate ListView dimensions based on content */

                // get QQuickItem which is a root element which hosts delegate items
                var fchild=resListView.visibleChildren[0]
                var count=resListView.count>7?7:resListView.count
                var mheight=count*80

                resListView.height =mheight
                root.height=resListView.height+queryHeader.height+80+16
                mpage.height=resListView.height
            }

            // model: EveryQueryModel{
            //     id:every_query_model;
            //     queryText:queryHeader.text
            // }
            model:queryModel
            //            model:filterModel
            Binding{
                target:queryModel;
                property:"queryText";
                value:queryHeader.modifiedText
            }


            delegate:ResItemDelegate{
                color: "transparent"
                height: 80
                width: parent.width
            }


            ScrollBar.vertical:ScrollBar{}
            highlightFollowsCurrentItem: true
        }
    }
}
