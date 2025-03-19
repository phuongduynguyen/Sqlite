import QtQuick 6.0
import QtQuick.Controls 6.0
//import MyApp 1.0

ApplicationWindow {
    visible: true
    width: 400
    height: 700
    title: "Contacts Manager"
    Column {
        anchors.centerIn: parent
        spacing: 10

        TextField {
            id: nameInput
            placeholderText: "Enter contact name"
        }

        Rectangle {
            id: addButton
            width: 32
            height: 16
            color: "lightblue"
            Text {
                id: addTxt
                text: "ADD"
            }
            MouseArea {
                id: mouse
                anchors.fill: parent
                onClicked: {
                    hmiIntf.addContactInterface(nameInput.text, ["123-456-7890"], "DuyGay", "/home/duynp/C++/SqlLite/build/meo.jpg");
                }
            }
        }
    }
}
