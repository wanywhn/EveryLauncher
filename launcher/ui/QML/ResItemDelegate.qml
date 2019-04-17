import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.12


Rectangle {
    property string css_style:"<style type=\"text/css\"> span{ font-weight:bold;color:#ff9955}</style> "
    Material.elevation: 5
    

    MouseArea{
        anchors.fill: parent
        onClicked: {
            resListView.currentIndex=index
        }
    }
//        Keys.onReturnPressed:{
//            console.log("test:",fileExec)
//        }

    function oc(a)
    {
      var o = {}; //相当于var o = new Object();
      for(var i=0;i<a.length;i++)
      {
        o[a[i]]=''; //注意该写法，不能写成o.a[i]
      }
      return o;
    }
    //TODO 添加默认图片
    function getIconLocation(fn){
        var suffix=fn.split(".").pop()
        if(suffix==="desktop"){
            return fileIcon
        }
        var image_suffix=["jpg","jpeg","png","gif"]
        var location=suffix in oc(image_suffix)?fileLocation:""
        return location
    }
    function getDisplayName(fn){
        var suffix=fn.split(".").pop()
        if(suffix==="desktop"){
            return desktopName
        }
        return fn
    }

    function getFileContent(fn){
        var suffix=fn.split(".").pop()
        if(suffix!=="desktop"){
        return css_style+ fileSimpleContent
        }
        return desktopDesc
    }
    function getContentFontSize(fn){
        var suffix=fn.split(".").pop()
        if(suffix!=="desktop"){
        return 11
        }
        return 15

    }

    Image{
        id: file_icon

        source:getIconLocation(fileName)
//        height: parent.height-4
        width: parent.height-8
        anchors{
            left: parent.left
            leftMargin: 8
            top: parent.top
            topMargin: 4
            bottom: parent.bottom
            bottomMargin:4
        }
    }
    Text {
        id: file_name
        text: getDisplayName(fileName)
        //                        wrapMode:
        anchors{
            left: file_icon.right
            leftMargin: 8
            top:parent.top
            topMargin: 4
            //                            right:file_statu.left
        }
    }
    Text {
        id: file_location
        text: fileLocation
        elide: Text.ElideLeft

        //                        wrapMode:
        anchors{
            left: file_icon.right
            leftMargin: 8
            top:file_name.bottom
            right: parent.right
        }
    }
    Text{
        id: file_simple_content
        text:getFileContent(fileName)
        wrapMode: Text.WordWrap
        elide: Text.ElideMiddle

        maximumLineCount: 2
        textFormat: Text.RichText
        
        font.pixelSize:getContentFontSize(fileName)
        
        anchors{
            left: file_icon.right
            leftMargin: 4
            top:file_location.bottom
            rightMargin: 4
            right: parent.right
//            bottom: parent.bottom
//            bottomMargin: 4
        }
    }
}
