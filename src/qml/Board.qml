import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: boardItem
    width: 480
    height: 480

    property var ranks: ["8", "7", "6", "5", "4", "3", "2", "1"]
    property var files: ["A", "B", "C", "D", "E", "F", "G", "H"]


    // Grid for the chessboard squares (same as before)
    Grid {
        id: grid
        rows: 8
        columns: 8
        anchors.fill: parent

        Repeater {
            model: 64
            Rectangle {
                width: boardItem.width / 8
                height: boardItem.height / 8
                color: (index % 2 === ((Math.floor(index / 8) % 2 === 0) ? 0 : 1)) ? "white" : "#D2B48C"
                border.color: "gray"
            }
        }
    }

    Repeater {
        model: chessBoard.piecesModel
        delegate: Image {
            id: pieceImage
            source: "qrc:/img/" + type + ".png"
            width: boardItem.width / 8
            height: boardItem.height / 8
            x: fileIndex * (boardItem.width / 8)
            y: rankIndex * (boardItem.height / 8)

            MouseArea {
                anchors.fill: parent
                drag.target: pieceImage
                drag.axis: Drag.XAndYAxis
                drag.minimumX: 0
                drag.maximumX: boardItem.width - pieceImage.width
                drag.minimumY: 0
                drag.maximumY: boardItem.height - pieceImage.height

                onReleased: {
                    var newFileIndex = Math.round(pieceImage.x / (boardItem.width / 8));
                    var newRankIndex = Math.round(pieceImage.y / (boardItem.height / 8));

                    // Snap to grid
                    pieceImage.x = newFileIndex * (boardItem.width / 8);
                    pieceImage.y = newRankIndex * (boardItem.height / 8);

                    // Update the model in C++
                    chessBoard.updatePiecePosition(index, newFileIndex, newRankIndex);
                }
            }
        }
    }

    // Labels for files (A-H)
    Row {
        spacing: 0
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater {
            model: boardItem.files
            Text {
                text: modelData
                font.bold: true
                color: "black"
                font.pixelSize: 18
                width: boardItem.width / 8
                height: 30
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    // Labels for ranks (1-8)
    Column {
        spacing: 0
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        Repeater {
            model: boardItem.ranks
            Text {
                text: modelData
                font.bold: true
                color: "black"
                font.pixelSize: 18
                width: 30
                height: boardItem.height / 8
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
