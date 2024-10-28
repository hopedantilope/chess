#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Board.h"

#include <QQmlEngine>
#include "PiecesModel.h" // Make sure to include the header
#include <QtQml> // Include this header to use qmlRegisterType

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Register the PiecesModel type with QML
    qmlRegisterUncreatableType<PiecesModel>("Chess", 1, 0, "PiecesModel", "Cannot create PiecesModel in QML");

    Board chessBoard;
    QString fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    chessBoard.setupBoardFromFEN(fen);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("chessBoard", &chessBoard);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    return app.exec();
}
