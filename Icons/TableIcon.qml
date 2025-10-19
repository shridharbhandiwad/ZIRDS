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
            
            // Table outline
            ctx.beginPath();
            ctx.roundRect(3, 5, 18, 14, 2, 2);
            ctx.stroke();
            
            // Header row
            ctx.beginPath();
            ctx.moveTo(3, 9);
            ctx.lineTo(21, 9);
            ctx.stroke();
            
            // Vertical dividers
            ctx.beginPath();
            ctx.moveTo(9, 5);
            ctx.lineTo(9, 19);
            ctx.moveTo(15, 5);
            ctx.lineTo(15, 19);
            ctx.stroke();
            
            // Row dividers
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.moveTo(3, 13);
            ctx.lineTo(21, 13);
            ctx.moveTo(3, 16);
            ctx.lineTo(21, 16);
            ctx.stroke();
            
            // Data dots (representing table content)
            ctx.fillStyle = root.iconColor;
            ctx.beginPath();
            ctx.arc(6, 11, 0.8, 0, 2 * Math.PI);
            ctx.arc(12, 11, 0.8, 0, 2 * Math.PI);
            ctx.arc(18, 11, 0.8, 0, 2 * Math.PI);
            ctx.arc(6, 14.5, 0.8, 0, 2 * Math.PI);
            ctx.arc(12, 14.5, 0.8, 0, 2 * Math.PI);
            ctx.arc(18, 14.5, 0.8, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
    
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
}