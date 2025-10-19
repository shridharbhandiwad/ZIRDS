import QtQuick 2.15

Item {
    id: root
    width: 24
    height: 24
    
    property color iconColor: "#ffffff"
    property real opacity: 1.0
    property bool isEnabled: true
    
    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            
            ctx.globalAlpha = root.opacity;
            ctx.strokeStyle = root.iconColor;
            ctx.fillStyle = "transparent";
            ctx.lineWidth = 2;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";
            
            var scale = Math.min(width, height) / 24;
            ctx.scale(scale, scale);
            
            if (root.isEnabled) {
                // Eye with slash (disabled)
                // Eye outline
                ctx.beginPath();
                ctx.ellipse(4, 10, 16, 4);
                ctx.stroke();
                
                // Eye pupil
                ctx.beginPath();
                ctx.arc(12, 12, 2, 0, 2 * Math.PI);
                ctx.stroke();
                
                // Slash line
                ctx.beginPath();
                ctx.moveTo(4, 20);
                ctx.lineTo(20, 4);
                ctx.stroke();
            } else {
                // Eye (enabled)
                ctx.beginPath();
                ctx.ellipse(4, 10, 16, 4);
                ctx.stroke();
                
                ctx.beginPath();
                ctx.arc(12, 12, 2, 0, 2 * Math.PI);
                ctx.fill();
            }
        }
    }
    
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
    onIsEnabledChanged: canvas.requestPaint()
}