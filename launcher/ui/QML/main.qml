import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0



Rectangle{
    id: root

    width: 640
    height:mpage.height
    //minimumHeight: 100
    //visible: systemTray.mainWindowVisiable
    //title: "everyLauncher"
    //flags: Qt.FramelessWindowHint

    //Component.onCompleted: {
    //setX(Screen.width/2-width/2);
    //setY(Screen.height/5-height)
    //}

    //onActiveChanged: {
    //if(!active){
    //            systemTray.mainWindowVisiable=false
    //systemTray.showMainWindow(false)
    //}
    //}

    Page{

        id:mpage
        anchors{
            left:parent.left
            top:parent.top
            right:parent.right
        }
        //        height:resListView.height


        header:SearchCateHeader{
            id:queryHeader
            focus: true
            //TODO get focus
            onPreferenceClicked: {
                systemTray.preferenceActionTriggered()
            }
        }
        Keys.onDownPressed: {
            resListView.incrementCurrentIndex()
        }
        Keys.onUpPressed: {
            resListView.decrementCurrentIndex()

        }
        contentItem:
        ListView {
            id:resListView


            anchors{
                top:parent.top
                left:parent.left
                right:parent.right
                topMargin:70+16
                leftMargin: 16
                rightMargin: 16
            }
            highlight:ItemHighlightBar{

            }

            onCountChanged: {
                /* calculate ListView dimensions based on content */

                // get QQuickItem which is a root element which hosts delegate items
                var fchild=resListView.visibleChildren[0]
                var count=resListView.count>10?10:resListView.count
                var mheight=count*60

                resListView.height =mheight
                //                resListView.width = listViewWidth
                root.height=resListView.height+queryHeader.height+70+16
                //console.log("resListView height:",resListView.height)
                //console.log("queryHeader height:",queryHeader.height)
                mpage.height=resListView.height
            }

            // model: EveryQueryModel{
            //     id:every_query_model;
            //     queryText:queryHeader.text
            // }
            //model:queryModel
            model:filterModel
            Binding{
                target:queryModel;
                property:"queryText";
                value:queryHeader.text
            }


            delegate:ResItemDelegate{
                color: "transparent"
                height: 60
                width: parent.width
            }


            ScrollBar.vertical:ScrollBar{}
            highlightFollowsCurrentItem: true
        }
    }
}
