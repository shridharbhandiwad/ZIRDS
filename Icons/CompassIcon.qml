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
            ctx.fillStyle = root.iconColor;
            ctx.lineWidth = 2;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";
            
            var scale = Math.min(width, height) / 24;
            ctx.scale(scale, scale);
            
            // Outer circle
            ctx.beginPath();
            ctx.arc(12, 12, 9, 0, 2 * Math.PI);
            ctx.stroke();
            
            // North arrow (filled)
            ctx.beginPath();
            ctx.moveTo(12, 4);
            ctx.lineTo(10, 10);
            ctx.lineTo(12, 8);
            ctx.lineTo(14, 10);
            ctx.closePath();
            ctx.fill();
            
            // South arrow (outline)
            ctx.fillStyle = "transparent";
            ctx.beginPath();
            ctx.moveTo(12, 20);
            ctx.lineTo(10, 14);
            ctx.lineTo(12, 16);
            ctx.lineTo(14, 14);
            ctx.closePath();
            ctx.stroke();
            
            // Center dot
            ctx.fillStyle = root.iconColor;
            ctx.beginPath();
            ctx.arc(12, 12, 1.5, 0, 2 * Math.PI);
            ctx.fill();
            
            // Cardinal direction markers
            ctx.strokeStyle = root.iconColor;
            ctx.lineWidth = 1;
            ctx.beginPath();
            // N, E, S, W markers
            ctx.moveTo(12, 2);
            ctx.lineTo(12, 4);
            ctx.moveTo(22, 12);
            ctx.lineTo(20, 12);
            ctx.moveTo(12, 22);
            ctx.lineTo(12, 20);
            ctx.moveTo(2, 12);
            ctx.lineTo(4, 12);
            ctx.stroke();
        }
    }
    
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
}