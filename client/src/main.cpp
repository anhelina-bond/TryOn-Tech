// main.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "QMLManager.h"
#include <Qt3DRender/QRenderAspect>

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#include <QJniObject>
#include <QJniEnvironment>

// This function will be called by JNI when the application starts
extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
}
#endif

int main(int argc, char *argv[])
{
    qputenv("QT3D_RENDERER", "opengl");  // Force OpenGL backend
    qputenv("QSG_RHI_BACKEND", "opengl"); // Force Qt Quick to use OpenGL
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType(QUrl("qrc:/ARClothTryOn/qml/Style.qml"), "ARClothTryOn", 1, 0, "Style");
    // Register QMLManager
    qmlRegisterType<QMLManager>("ARClothTryOn", 1, 0, "QMLManager");
    qmlRegisterType<NetworkManager>("ARClothTryOn", 1, 0, "NetworkManager");

#ifdef Q_OS_ANDROID
    // Initialize Qt Android platform integration
    QJniEnvironment env;
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (activity.isValid()) {
        // Connect Qt activity to native code
        QJniObject::callStaticMethod<void>(
            "org/qtproject/qt/android/QtNative",
            "setActivity", "(Landroid/app/Activity;)V",
            activity.object<jobject>());
    }
#endif

    // Set application attributes
    QQuickStyle::setStyle("Material");

    // Create QML engine and load main QML file
    QQmlApplicationEngine engine;
    // const QUrl url(QStringLiteral("qrc:/ARClothTryOn/qml/AuthorizationPage.qml"));
    const QUrl url(QStringLiteral("qrc:/ARClothTryOn/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}




// int main(int argc, char *argv[]) {
//     QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//     QGuiApplication app(argc, argv);

//     // Verify SSL availability
//     if (!QSslSocket::supportsSsl()) {
//         qDebug() << "SSL NOT SUPPORTED. Available backends:"
//                  << QSslSocket::availableBackends();
//         exit(1);
//     }

//     // ... rest of your code
// }