import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("chess")

    // Load another QML component (board.qml)
    Board {
        anchors.centerIn: parent
    }
}
