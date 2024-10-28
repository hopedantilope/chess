#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

class MainWindow : public QObject
{
    Q_OBJECT

public:
    explicit MainWindow(QObject *parent = nullptr);
    ~MainWindow();

private:
    QQmlApplicationEngine engine;  // QML engine to load and manage QML files
};
