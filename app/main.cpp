#include <QtQml/QQmlApplicationEngine>
#include <QtGui/QGuiApplication>

#include <QQmlContext>
#include <QStringList>

#include <QDir>

#include <QtWidgets/QApplication>
#include <QIcon>
#include <QQuickStyle>
#include <QtQml/qqml.h>

#include <kdsingleapplication.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <stdlib.h>

#include <QLoggingCategory>

#include <fileio.h>

#if defined(Q_OS_MAC)
#include <CoreFoundation/CoreFoundation.h>
#include <QStyleFactory>
#include <QMenu>
#endif

static QString getNamedArgument(const QStringList &args, const QStringList &names, const QString &defaultValue = QString())
{
    for (const QString &name : names) {
        int index = args.indexOf(name);
        if (index != -1 && index + 1 < args.size())
            return args[index + 1];
    }
    return defaultValue;
}

// Options for the window opened by this launch. Everything after "-e" belongs
// to the command, so application options are only looked for before it.
// The map is passed to QML (appRoot.createWindow) and, when another instance
// is already running, sent to it as JSON.
static QVariantMap parseLaunchOptions(const QStringList &arguments, const QString &currentDir)
{
    QVariantMap options;

    QStringList appArgs = arguments;
    int commandIndex = arguments.indexOf("-e");
    if (commandIndex != -1) {
        appArgs = arguments.mid(0, commandIndex);
        QStringList commandLine = arguments.mid(commandIndex + 1);
        if (!commandLine.isEmpty()) {
            options["command"] = commandLine.takeFirst();
            options["args"] = commandLine;
        }
    }

    QString workdir = getNamedArgument(appArgs, {"--workdir"}, currentDir);
    options["workdir"] = QDir(currentDir).absoluteFilePath(workdir);

    QString profile = getNamedArgument(appArgs, {"-p", "--profile"});
    if (!profile.isEmpty())
        options["profile"] = profile;

    options["fullscreen"] = appArgs.contains("--fullscreen");
    options["defaultSettings"] = appArgs.contains("--default-settings");
    options["verbose"] = appArgs.contains("--verbose");

    return options;
}

int main(int argc, char *argv[])
{
    // Some environmental variable are necessary on certain platforms.
    // Disable Connections slot warnings
    QLoggingCategory::setFilterRules("qt.qml.connections.warning=false");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);

// #if defined (Q_OS_LINUX)
//     setenv("QSG_RENDER_LOOP", "threaded", 0);
// #endif

#if defined(Q_OS_MAC)
    // This allows UTF-8 characters usage in OSX.
    setenv("LC_CTYPE", "UTF-8", 1);

    // Ensure key repeat works for letter keys (disable macOS press-and-hold for this app).
    CFPreferencesSetAppValue(CFSTR("ApplePressAndHoldEnabled"), kCFBooleanFalse, kCFPreferencesCurrentApplication);
    CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);

    // Qt6 macOS default look is still lacking, so let's force Fusion for now
    QQuickStyle::setStyle(QStringLiteral("Fusion"));
#endif

    if (argc>1 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help"))) {
        QTextStream cout(stdout, QIODevice::WriteOnly);
        cout << "Usage: " << argv[0] << " [--default-settings] [--workdir <dir>] [-p|--profile <prof>] [--fullscreen] [--verbose] [-h|--help] [-v|--version] [-e <cmd> [args...]]" << Qt::endl;
        cout << "  --default-settings  Run cool-retro-term with the default settings" << Qt::endl;
        cout << "  --workdir <dir>     Change working directory to 'dir'" << Qt::endl;
        cout << "  -e <cmd>            Command to execute. This option will catch all following arguments, so use it as the last option." << Qt::endl;
        cout << "  --fullscreen        Run cool-retro-term in fullscreen." << Qt::endl;
        cout << "  -p|--profile <prof> Run cool-retro-term with the given profile." << Qt::endl;
        cout << "  -h|--help           Print this help." << Qt::endl;
        cout << "  -v|--version        Print the version." << Qt::endl;
        cout << "  --verbose           Print additional information such as profiles and settings." << Qt::endl;
        return 0;
    }

    QString appVersion(QStringLiteral(APP_VERSION));

    if (argc>1 && (!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version"))) {
        QTextStream cout(stdout, QIODevice::WriteOnly);
        cout << "cool-retro-term " << appVersion << Qt::endl;
        return 0;
    }

    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_MacDontSwapCtrlAndMeta, true);
    app.setApplicationName(QStringLiteral("cool-retro-term"));
    app.setOrganizationName(QStringLiteral("cool-retro-term"));
    app.setOrganizationDomain(QStringLiteral("cool-retro-term"));
    app.setApplicationVersion(appVersion);

    const QVariantMap launchOptions = parseLaunchOptions(app.arguments(), QDir::currentPath());

    KDSingleApplication singleApp(QStringLiteral("cool-retro-term"));

    if (!singleApp.isPrimaryInstance()) {
        QByteArray message = QJsonDocument(QJsonObject::fromVariantMap(launchOptions)).toJson(QJsonDocument::Compact);
        if (singleApp.sendMessage(message))
            return 0;
        qWarning() << "KDSingleApplication: primary not reachable, continuing as independent instance.";
    }

    QQmlApplicationEngine engine;
    FileIO fileIO;

#if !defined(Q_OS_MAC)
    app.setWindowIcon(QIcon::fromTheme("cool-retro-term", QIcon(":/icons/cool-retro-term.png")));
#if defined(Q_OS_LINUX)
    QGuiApplication::setDesktopFileName(QStringLiteral("cool-retro-term"));
#endif
#else
    app.setWindowIcon(QIcon(":/icons/cool-retro-term.png"));
#endif

    engine.rootContext()->setContextProperty("appVersion", appVersion);
    engine.rootContext()->setContextProperty("launchOptions", launchOptions);
    engine.rootContext()->setContextProperty("fileIO", &fileIO);

    // Manage import paths for Linux and OSX.
    QStringList importPathList = engine.importPathList();
    importPathList.append(QCoreApplication::applicationDirPath() + "/qmltermwidget");
    importPathList.append(QCoreApplication::applicationDirPath() + "/../PlugIns");
    importPathList.append(QCoreApplication::applicationDirPath() + "/../../../qmltermwidget");
    engine.setImportPathList(importPathList);

    engine.loadFromModule("CoolRetroTerm", "Main");

    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Cannot load QML interface";
        return EXIT_FAILURE;
    }

    // Quit the application when the engine closes.
    QObject::connect((QObject*) &engine, SIGNAL(quit()), (QObject*) &app, SLOT(quit()));

    auto requestNewWindow = [&engine](const QVariantMap &options) {
        if (engine.rootObjects().isEmpty())
            return;

        QObject *rootObject = engine.rootObjects().constFirst();
        QMetaObject::invokeMethod(rootObject, "openLaunchWindow", Qt::QueuedConnection,
                                  Q_ARG(QVariant, QVariant(options)));
    };

    QObject::connect(&singleApp, &KDSingleApplication::messageReceived, &app,
                     [&requestNewWindow](const QByteArray &message) {
        // Older versions send a bare "new-window"; that still opens a default window.
        QJsonDocument document = QJsonDocument::fromJson(message);
        requestNewWindow(document.isObject() ? document.object().toVariantMap() : QVariantMap());
    });

#if defined(Q_OS_MAC)
    QMenu *dockMenu = new QMenu(nullptr);
    dockMenu->addAction(QObject::tr("New Window"), [&requestNewWindow]() { requestNewWindow(QVariantMap()); });
    dockMenu->setAsDockMenu();
#endif

    return app.exec();
}
