import QtQuick 2.15

Item {
    id: root
    width: 24
    height: 24
    
    property color iconColor: "#ffffff"
    property real opacity: 1.0
    
    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            
            ctx.globalAlpha = root.opacity;
            ctx.strokeStyle = root.iconColor;
            ctx.fillStyle = "transparent";
            ctx.lineWidth = 1.5;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";
            
            var scale = Math.min(width, height) / 24;
            ctx.scale(scale, scale);
            
            // Grid lines
            ctx.beginPath();
            
            // Vertical lines
            for (var x = 4; x <= 20; x += 4) {
                ctx.moveTo(x, 4);
                ctx.lineTo(x, 20);
            }
            
            // Horizontal lines
            for (var y = 4; y <= 20; y += 4) {
                ctx.moveTo(4, y);
                ctx.lineTo(20, y);
            }
            
            ctx.stroke();
            
            // Corner dots for emphasis
            ctx.fillStyle = root.iconColor;
            ctx.beginPath();
            ctx.arc(4, 4, 1, 0, 2 * Math.PI);
            ctx.arc(20, 4, 1, 0, 2 * Math.PI);
            ctx.arc(4, 20, 1, 0, 2 * Math.PI);
            ctx.arc(20, 20, 1, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
    
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
}