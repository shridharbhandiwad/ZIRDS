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
            ctx.lineWidth = 2;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";
            
            var scale = Math.min(width, height) / 24;
            ctx.scale(scale, scale);
            
            // Magnifying glass
            ctx.beginPath();
            ctx.arc(10, 10, 6, 0, 2 * Math.PI);
            ctx.stroke();
            
            // Handle
            ctx.beginPath();
            ctx.moveTo(15, 15);
            ctx.lineTo(20, 20);
            ctx.stroke();
            
            // Expand arrows inside
            ctx.beginPath();
            // Top-left arrow
            ctx.moveTo(7, 9);
            ctx.lineTo(6, 8);
            ctx.lineTo(7, 7);
            ctx.moveTo(6, 8);
            ctx.lineTo(8, 8);
            ctx.moveTo(6, 8);
            ctx.lineTo(6, 10);
            
            // Bottom-right arrow
            ctx.moveTo(13, 11);
            ctx.lineTo(14, 12);
            ctx.lineTo(13, 13);
            ctx.moveTo(14, 12);
            ctx.lineTo(12, 12);
            ctx.moveTo(14, 12);
            ctx.lineTo(14, 10);
            
            ctx.stroke();
        }
    }
    
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
}