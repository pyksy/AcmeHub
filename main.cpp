#include <QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include <QTcpServer>

#define LISTEN_ADDRESS "0.0.0.0"
#define LISTEN_PORT 4080

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    QHttpServer httpServer;
    httpServer.route("/", []() {
        return "Hello World\n";
    });

    auto tcpServer = std::make_unique<QTcpServer>();

    if (!tcpServer->listen(QHostAddress(LISTEN_ADDRESS), LISTEN_PORT)) {
        qDebug() << "Error: Cannot bind TCP address" << LISTEN_ADDRESS << "port" << LISTEN_PORT;
        return 1;
    }

    if (!httpServer.bind(tcpServer.get())) {
        qDebug() << "Error: Cannot bind http server to TCP port";
        return 1;
    }

    tcpServer.release();

    qInfo() << "Listening for incoming http connections on host" << LISTEN_ADDRESS << "port" << LISTEN_PORT;

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.
    return a.exec();
}
