import QtQuick 2.15

Item {
    id: root
    width: 24
    height: 24
    
    property color iconColor: "#ffffff"
    property real opacity: 1.0
    property real rotation: 0
    
    Canvas {
        id: canvas
        anchors.fill: parent
        rotation: root.rotation
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
            
            // Center circle
            ctx.beginPath();
            ctx.arc(12, 12, 3, 0, 2 * Math.PI);
            ctx.stroke();
            
            // Gear teeth
            var numTeeth = 8;
            var outerRadius = 9;
            var innerRadius = 6;
            
            ctx.beginPath();
            for (var i = 0; i < numTeeth; i++) {
                var angle1 = (i * 2 * Math.PI) / numTeeth;
                var angle2 = ((i + 0.3) * 2 * Math.PI) / numTeeth;
                var angle3 = ((i + 0.7) * 2 * Math.PI) / numTeeth;
                var angle4 = ((i + 1) * 2 * Math.PI) / numTeeth;
                
                var x1 = 12 + innerRadius * Math.cos(angle1);
                var y1 = 12 + innerRadius * Math.sin(angle1);
                var x2 = 12 + outerRadius * Math.cos(angle2);
                var y2 = 12 + outerRadius * Math.sin(angle2);
                var x3 = 12 + outerRadius * Math.cos(angle3);
                var y3 = 12 + outerRadius * Math.sin(angle3);
                var x4 = 12 + innerRadius * Math.cos(angle4);
                var y4 = 12 + innerRadius * Math.sin(angle4);
                
                if (i === 0) {
                    ctx.moveTo(x1, y1);
                }
                ctx.lineTo(x2, y2);
                ctx.lineTo(x3, y3);
                ctx.lineTo(x4, y4);
            }
            ctx.closePath();
            ctx.stroke();
        }
    }
    
    // Subtle rotation animation
    RotationAnimation {
        id: rotationAnim
        target: root
        property: "rotation"
        from: 0
        to: 360
        duration: 8000
        loops: Animation.Infinite
        running: false
    }
    
    MouseArea {
        anchors.fill: parent
        onEntered: rotationAnim.running = true
        onExited: rotationAnim.running = false
    }
    
    onIconColorChanged: canvas.requestPaint()
    onOpacityChanged: canvas.requestPaint()
}