#include "cmapmainwindow.h"
#include "MapDisplay/cgismapcontroller.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDir>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include "qgsapplication.h"
#include "MapDisplay/cgismapcontroller.h"
#include "globalmacros.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QgsApplication::initQgis();

    // Set application properties
    app.setApplicationName("Radar Display");
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("RadarDisplay");
    app.setOrganizationDomain("radardisplay.com");

    qDebug() << "🎯 RADAR DISPLAY - DUAL MONITOR SYSTEM";
    qDebug() << "📦 APP VERSION:" << APP_VERSION;
    qDebug() << "📅 APP BUILT DATE:" << APP_BUILD_DATE;
    qDebug() << "🖥️  DUAL MONITOR SUPPORT: ENABLED";

    // Create splash screen
    QPixmap splashPixmap(400, 300);
    splashPixmap.fill(QColor("#f8fafc"));
    
    QSplashScreen splash(splashPixmap);
    splash.setStyleSheet(
        "QSplashScreen {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "   font-size: 16px;"
        "   font-weight: 600;"
        "}"
    );
    
    splash.show();
    splash.showMessage("🎯 Initializing Radar Display System...", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
    app.processEvents();

    // Initialize main window manager
    CMapMainWindow mainWindow;
    
    // Update splash
    QTimer::singleShot(1000, [&]() {
        splash.showMessage("🖥️  Setting up dual monitor layout...", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
    });
    
    QTimer::singleShot(2000, [&]() {
        splash.showMessage("✅ System ready!", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
    });
    
    // Close splash and show main system
    QTimer::singleShot(3000, [&]() {
        splash.close();
        
        // Show small control window for window management
        mainWindow.show();
        mainWindow.resize(400, 200);
        mainWindow.move(100, 100);
        
        qDebug() << "🚀 Dual monitor radar display system started successfully!";
        qDebug() << "💡 Use F1/F2 to show windows, F3 for dual monitor arrangement, F4 for fullscreen";
    });

    return app.exec();
}
