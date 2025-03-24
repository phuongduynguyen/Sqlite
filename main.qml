import QtQuick 6.0
import QtQuick.Controls 6.0
//import MyApp 1.0

ApplicationWindow {
    id:main
    visible: true
    width: 400
    height: 700
    title: "Contacts Manager"
    Column{
        anchors.centerIn: parent
        spacing: 10
        TextField {
            id: input
            width: 138
            placeholderText: "Enter contact name"
        }
        Row {
            spacing: 10
            Button{
                id: addButton
                width: 64
                height: 32
                text: "Add"
                onClicked: {
                    hmiIntf.addContactInterface(input.text, ["123-456-7890"], "DuyGay", "/home/duynp/C++/SqlLite/build/meo.jpg");
                    input.text = ""
                }
            }

            Button{
                id: delButton
                width: 64
                height: 32
                text: "Delete"
                onClicked: {
                    hmiIntf.deleteContactInterface(input.text);
                    input.text = ""
                }
            }
        }


    }



}
