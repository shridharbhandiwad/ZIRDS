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
            
            // House outline
            ctx.beginPath();
            ctx.moveTo(3, 12);
            ctx.lineTo(12, 3);
            ctx.lineTo(21, 12);
            ctx.moveTo(5, 12);
            ctx.lineTo(5, 20);
            ctx.lineTo(19, 20);
            ctx.lineTo(19, 12);
            ctx.stroke();
            
            // Door
            ctx.beginPath();
            ctx.moveTo(10, 16);
            ctx.lineTo(10, 20);
            ctx.lineTo(14, 20);
            ctx.lineTo(14, 16);
            ctx.closePath();
            ctx.stroke();
            
            // Door handle
            ctx.beginPath();
            ctx.arc(13, 18, 0.5, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
    
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
}