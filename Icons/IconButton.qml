import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root
    
    property string iconType: "home"
    property color iconColor: isToggled ? "#10b981" : "#ffffff"
    property color hoverColor: "#60a5fa"
    property real iconOpacity: 1.0
    property bool isToggled: false
    
    signal clicked()
    
    width: 36
    height: 36
    
    onClicked: root.clicked()
    
    background: Rectangle {
        color: root.isToggled ? Qt.rgba(16/255, 185/255, 129/255, 0.2) : "transparent"
        radius: 8
        border.width: (root.hovered || root.isToggled) ? 2 : 0
        border.color: root.isToggled ? "#10b981" : root.hoverColor
        
        Rectangle {
            anchors.fill: parent
            color: root.iconColor
            opacity: root.hovered ? 0.1 : 0
            radius: parent.radius
            
            Behavior on opacity {
                NumberAnimation { duration: 150 }
            }
        }
        
        Behavior on border.width {
            NumberAnimation { duration: 150 }
        }
        
        Behavior on color {
            ColorAnimation { duration: 200 }
        }
        
        Behavior on border.color {
            ColorAnimation { duration: 200 }
        }
    }
    
    contentItem: Item {
        anchors.fill: parent
        
        Loader {
            id: iconLoader
            anchors.centerIn: parent
            width: 24
            height: 24
            
            source: {
                switch(root.iconType) {
                    case "loadmap": return "LoadMapIcon.qml"
                    case "disablemap": return "DisableMapIcon.qml"
                    case "zoomfit": return "ZoomFitIcon.qml"
                    case "home": return "HomeIcon.qml"
                    case "grid": return "GridIcon.qml"
                    case "compass": return "CompassIcon.qml"
                    case "table": return "TableIcon.qml"
                    case "settings": return "SettingsIcon.qml"
                    default: return "HomeIcon.qml"
                }
            }
            
            onLoaded: {
                if (item) {
                    item.iconColor = Qt.binding(function() { return root.iconColor })
                    item.opacity = Qt.binding(function() { return root.iconOpacity })
                    
                    // Special properties for specific icons
                    if (root.iconType === "disablemap" && item.hasOwnProperty("isEnabled")) {
                        item.isEnabled = Qt.binding(function() { return !root.isToggled })
                    }
                }
            }
        }
        
        // Hover scale effect
        scale: root.hovered ? 1.05 : 1.0
        
        Behavior on scale {
            NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
        }
    }
    
    // Ripple effect on press
    Rectangle {
        id: ripple
        anchors.centerIn: parent
        width: 0
        height: width
        radius: width / 2
        color: root.iconColor
        opacity: 0
        
        ParallelAnimation {
            id: rippleAnimation
            NumberAnimation {
                target: ripple
                property: "width"
                from: 0
                to: root.width * 1.5
                duration: 300
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                target: ripple
                property: "opacity"
                from: 0.3
                to: 0
                duration: 300
                easing.type: Easing.OutQuad
            }
        }
    }
    
    onPressed: rippleAnimation.start()
}