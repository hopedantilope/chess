import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: boardItem
    width: 480
    height: 480

    property var ranks: ["8", "7", "6", "5", "4", "3", "2", "1"]
    property var files: ["A", "B", "C", "D", "E", "F", "G", "H"]
    property var highlightedSquares: []

    // Grid for the chessboard squares
    Grid {
        id: grid
        rows: 8
        columns: 8
        anchors.fill: parent

        Repeater {
            model: 64
            Rectangle {
                id: square
                width: boardItem.width / 8
                height: boardItem.height / 8
                property int rankIndex: Math.floor(index / 8)
                property int fileIndex: index % 8
                color: {
                    if (highlightedSquares.some(function(elem) { return elem.x === fileIndex && elem.y === rankIndex; })) {
                        return "red";
                    } else {
                        return ((index % 2 === ((Math.floor(index / 8) % 2 === 0) ? 0 : 1)) ? "white" : "#D2B48C");
                    }
                }
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

                property real originalX: pieceImage.x
                property real originalY: pieceImage.y

                onPressed: {
                    // Store the original position when the drag starts
                    originalX = pieceImage.x;
                    originalY = pieceImage.y;

                    // Clear previous highlights
                    boardItem.highlightedSquares = [];

                    // Get legal moves from the C++ backend
                    var moves = chessBoard.getLegalMoves(rankIndex, fileIndex);
                    boardItem.highlightedSquares = moves;
                }

                onReleased: {
                    // Clear highlights when the piece is released
                    boardItem.highlightedSquares = [];

                    var newFileIndex = Math.round(pieceImage.x / (boardItem.width / 8));
                    var newRankIndex = Math.round(pieceImage.y / (boardItem.height / 8));

                    // Update the model in C++
                    if (chessBoard.updatePiecePosition(index, newFileIndex, newRankIndex)) {
                        pieceImage.x = newFileIndex * (boardItem.width / 8);
                        pieceImage.y = newRankIndex * (boardItem.height / 8);
                    } else {
                        // Snap the image back to its original position
                        pieceImage.x = originalX;
                        pieceImage.y = originalY;
                    }
                }
            }
        }
    }

    // Labels for files (A-H)
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
