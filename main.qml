import QtQuick 6.0
import QtQuick.Controls 6.0

ApplicationWindow {
    visible: true
    width: 400
    height: 700
    title: "Contacts Manager"
    Rectangle {
        id: addButton
        width: 32
        height: 16
        anchors.centerIn: parent
        color: "lightblue"
        Text {
            id: addTxt
            text: "ADD"
        }
        MouseArea{
            id:mouse
            anchors.fill: parent
            onClicked:{
                test.showMessage("Hello from QML!")
            }
        }
    }
}
