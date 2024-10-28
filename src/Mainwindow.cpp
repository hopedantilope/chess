#include "Mainwindow.h"

MainWindow::MainWindow(QObject *parent)
    : QObject(parent)
{
    // Load the QML file
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    // Check if QML loading was successful
    if (engine.rootObjects().isEmpty()) {
        qFatal("Error: Your QML file could not be loaded!");
    }
}

MainWindow::~MainWindow() {}

