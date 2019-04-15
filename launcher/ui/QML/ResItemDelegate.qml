import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0



Rectangle {
    property string css_style:"<style type=\"text/css\"> span{ font-weight:bold;color:#ff9955}</style> "
    

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


    Image{
        id: file_icon

        source:getIconLocation(fileName)
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
        text: getDisplayName(fileName)
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
}
