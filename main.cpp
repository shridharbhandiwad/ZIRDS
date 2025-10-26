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
#include <QPainter>
#include <QFont>
#include "qgsapplication.h"
#include "MapDisplay/cgismapcontroller.h"
#include "globalmacros.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QgsApplication::initQgis();

    // Set application properties
    app.setApplicationName("ZIRDS - Zoppler Integrated Radar Display System");
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("Zoppler Systems");
    app.setOrganizationDomain("zoppler.com");
    
    // Set application icon
    QIcon appIcon(":/images/resources/zoppler_logo.png");
    app.setWindowIcon(appIcon);

    qDebug() << "🎯 ZIRDS - Zoppler Integrated Radar Display System";
    qDebug() << "📦 APP VERSION:" << APP_VERSION;
    qDebug() << "📅 APP BUILT DATE:" << APP_BUILD_DATE;
    qDebug() << "©️  COPYRIGHT: Zoppler Systems";
    qDebug() << "🖥️  DUAL MONITOR SUPPORT: ENABLED";

    // Create splash screen with Zoppler branding
    QPixmap splashPixmap(600, 400);
    splashPixmap.fill(Qt::black);
    
    // Try to load Zoppler logo
    QPixmap logo(":/images/resources/zoppler_logo.png");
    if (!logo.isNull()) {
        QPainter painter(&splashPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        
        // Draw logo centered at top
        int logoWidth = 300;
        int logoHeight = logo.height() * logoWidth / logo.width();
        QRect logoRect((splashPixmap.width() - logoWidth) / 2, 30, logoWidth, logoHeight);
        painter.drawPixmap(logoRect, logo);
        
        // Draw application name
        QFont titleFont("Segoe UI", 20, QFont::Bold);
        painter.setFont(titleFont);
        painter.setPen(Qt::white);
        painter.drawText(QRect(0, logoRect.bottom() + 30, splashPixmap.width(), 40), 
                        Qt::AlignCenter, "ZIRDS");
        
        // Draw subtitle
        QFont subtitleFont("Segoe UI", 12, QFont::Normal);
        painter.setFont(subtitleFont);
        painter.drawText(QRect(0, logoRect.bottom() + 70, splashPixmap.width(), 30), 
                        Qt::AlignCenter, "Zoppler Integrated Radar Display System");
        
        // Draw copyright
        QFont copyrightFont("Segoe UI", 10, QFont::Normal);
        painter.setFont(copyrightFont);
        painter.setPen(QColor("#999999"));
        painter.drawText(QRect(0, splashPixmap.height() - 60, splashPixmap.width(), 20), 
                        Qt::AlignCenter, "© Zoppler Systems");
    } else {
        // Fallback: Text-based splash screen
        QPainter painter(&splashPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw ZOPPLER in large text
        QFont titleFont("Segoe UI", 32, QFont::Bold);
        painter.setFont(titleFont);
        painter.setPen(Qt::white);
        painter.drawText(QRect(0, 80, splashPixmap.width(), 50), 
                        Qt::AlignCenter, "ZOPPLER");
        
        // Draw ZIRDS
        QFont subtitleFont("Segoe UI", 20, QFont::Bold);
        painter.setFont(subtitleFont);
        painter.drawText(QRect(0, 140, splashPixmap.width(), 40), 
                        Qt::AlignCenter, "ZIRDS");
        
        // Draw full name
        QFont nameFont("Segoe UI", 11, QFont::Normal);
        painter.setFont(nameFont);
        painter.drawText(QRect(0, 185, splashPixmap.width(), 30), 
                        Qt::AlignCenter, "Zoppler Integrated Radar Display System");
        
        // Draw copyright
        QFont copyrightFont("Segoe UI", 10, QFont::Normal);
        painter.setFont(copyrightFont);
        painter.setPen(QColor("#999999"));
        painter.drawText(QRect(0, splashPixmap.height() - 60, splashPixmap.width(), 20), 
                        Qt::AlignCenter, "© Zoppler Systems. All rights reserved.");
    }
    
    QSplashScreen splash(splashPixmap);
    splash.setStyleSheet(
        "QSplashScreen {"
        "   background-color: #000000;"
        "   color: white;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "   font-size: 14px;"
        "   font-weight: 500;"
        "}"
    );
    
    splash.show();
    splash.showMessage("🎯 Initializing ZIRDS...\n© Zoppler Systems", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
    app.processEvents();

    // Initialize main window manager
    CMapMainWindow mainWindow;
    
    // Update splash
    QTimer::singleShot(1000, [&]() {
        splash.showMessage("🖥️  Setting up dual monitor layout...\n© Zoppler Systems", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
    });
    
    QTimer::singleShot(2000, [&]() {
        splash.showMessage("✅ ZIRDS System Ready!\n© Zoppler Systems", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
    });
    
    // Close splash and show main system
    QTimer::singleShot(5000, [&]() {
        splash.close();
        
        // Keep window manager minimized, don't show at startup
        mainWindow.showMinimized();
        mainWindow.resize(400, 200);
        mainWindow.move(100, 100);
        
        qDebug() << "🚀 ZIRDS started successfully!";
        qDebug() << "💡 Use F1/F2 to show windows, F3 for dual monitor arrangement, F4 for fullscreen";
        qDebug() << "©️  Copyright © Zoppler Systems";
    });

    return app.exec();
}
