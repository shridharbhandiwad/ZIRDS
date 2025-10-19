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
            
            // Set icon color and opacity
            ctx.globalAlpha = root.opacity;
            ctx.strokeStyle = root.iconColor;
            ctx.fillStyle = root.iconColor;
            ctx.lineWidth = 2;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";
            
            // Draw folder icon
            var scale = Math.min(width, height) / 24;
            ctx.scale(scale, scale);
            
            // Folder back
            ctx.beginPath();
            ctx.moveTo(3, 6);
            ctx.lineTo(3, 18);
            ctx.quadraticCurveTo(3, 20, 5, 20);
            ctx.lineTo(19, 20);
            ctx.quadraticCurveTo(21, 20, 21, 18);
            ctx.lineTo(21, 8);
            ctx.quadraticCurveTo(21, 6, 19, 6);
            ctx.lineTo(11, 6);
            ctx.lineTo(9, 4);
            ctx.lineTo(5, 4);
            ctx.quadraticCurveTo(3, 4, 3, 6);
            ctx.closePath();
            ctx.stroke();
            
            // Map lines inside folder
            ctx.beginPath();
            ctx.moveTo(7, 10);
            ctx.lineTo(17, 10);
            ctx.moveTo(7, 13);
            ctx.lineTo(15, 13);
            ctx.moveTo(7, 16);
            ctx.lineTo(13, 16);
            ctx.stroke();
        }
    }
    
    // Trigger repaint when properties change
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
}