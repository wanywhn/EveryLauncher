import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0



    Rectangle {
        width:resListView.width; height: 60
        color: "#FFFF88"
        y:resListView.currentItem.y;
        Behavior on y { SpringAnimation{ spring: 2; damping: 0.1 } }
    }
