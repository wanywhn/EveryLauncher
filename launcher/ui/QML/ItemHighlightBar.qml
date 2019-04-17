import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.12



    Rectangle {
        radius: 10
        //TODO highlight the name
        width:resListView.width; height: 60
        color: "#FFFF88"
        //FIXME !!!
        y:resListView.currentItem.y;
        Material.elevation: 5
        Behavior on y {SmoothedAnimation{ velocity: 350} }
    }
