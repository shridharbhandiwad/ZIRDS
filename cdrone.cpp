#include "cdrone.h"
#include <QtMath>
#include <QDebug>
#include <QRandomGenerator>
#include <QColor>

CDrone::CDrone(int trackId, QObject *parent)
    : QObject(parent)
    , m_nTrackId(trackId)
    , m_prevHeading(0.0)
    , m_prevVelocity(0.0)
    , m_prevAltitude(0.0)
    , m_bearingChangeRate(0.0)
    , m_acceleration(0.0)
    , m_climbRate(0.0)
{
    // Initialize internal state with default values
    m_internalState.batteryLevel = 100.0f;
    m_internalState.batteryVoltage = 12.6f;
    m_internalState.powerConsumption = 150.0f;
    m_internalState.estimatedFlightTime = 30.0f;
    
    m_internalState.pitch = 0.0f;
    m_internalState.roll = 0.0f;
    m_internalState.yaw = 0.0f;
    m_internalState.verticalSpeed = 0.0f;
    m_internalState.groundSpeed = 0.0f;
    m_internalState.acceleration = 0.0f;
    
    m_internalState.flightMode = FLIGHT_MODE_CRUISE;
    m_internalState.missionId = QString("MISSION-%1").arg(trackId, 3, 10, QChar('0'));
    m_internalState.waypointIndex = 0;
    m_internalState.totalWaypoints = 10;
    m_internalState.missionProgress = 0.0f;
    
    m_internalState.sensors.gpsActive = true;
    m_internalState.sensors.imuActive = true;
    m_internalState.sensors.cameraActive = true;
    m_internalState.sensors.radarActive = true;
    m_internalState.sensors.lidarActive = true;
    m_internalState.sensors.gpsQuality = 95;
    m_internalState.sensors.linkQuality = 98;
    
    m_internalState.temperature = 25.0f;
    m_internalState.windSpeed = 5.0f;
    m_internalState.windDirection = 270.0f;
    
    m_internalState.healthOk = true;
    m_internalState.statusMessage = "OPERATIONAL";
    m_internalState.lastUpdateTime = QDateTime::currentDateTime();
    
    m_prevUpdateTime = QDateTime::currentDateTime();
}

CDrone::~CDrone()
{
}

void CDrone::updateDynamics(const stTrackDisplayInfo &trackInfo)
{
    calculateDynamics(trackInfo);
    
    // Update internal state based on dynamics
    m_internalState.groundSpeed = trackInfo.velocity;
    m_internalState.yaw = trackInfo.heading;
    m_internalState.acceleration = m_acceleration;
    m_internalState.verticalSpeed = m_climbRate;
    
    // Calculate pitch and roll based on dynamics
    // Pitch: related to acceleration and climb rate
    if (m_acceleration != 0) {
        m_internalState.pitch = qBound(-15.0f, static_cast<float>(m_acceleration * 5.0), 15.0f);
    } else {
        m_internalState.pitch = 0.0f;
    }
    
    // Roll: related to bearing change rate (turning)
    if (m_bearingChangeRate != 0) {
        m_internalState.roll = qBound(-30.0f, static_cast<float>(m_bearingChangeRate * 3.0), 30.0f);
    } else {
        m_internalState.roll = 0.0f;
    }
    
    // Update time since last update
    QDateTime currentTime = QDateTime::currentDateTime();
    double deltaTime = m_prevUpdateTime.msecsTo(currentTime) / 1000.0;
    
    // Update battery level
    updateBatteryLevel(deltaTime);
    
    // Update mission progress
    if (m_internalState.totalWaypoints > 0) {
        m_internalState.missionProgress = 
            (m_internalState.waypointIndex * 100.0f) / m_internalState.totalWaypoints;
    }
    
    // Check system health
    checkSystemHealth();
    
    m_internalState.lastUpdateTime = currentTime;
    
    emit internalStateChanged();
}

void CDrone::updateInternalState(const stDroneInternalState &state)
{
    m_internalState = state;
    m_internalState.lastUpdateTime = QDateTime::currentDateTime();
    
    checkSystemHealth();
    emit internalStateChanged();
}

void CDrone::calculateDynamics(const stTrackDisplayInfo &trackInfo)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    double deltaTime = m_prevUpdateTime.msecsTo(currentTime) / 1000.0;
    
    if (deltaTime <= 0.001) {
        deltaTime = 0.001; // Prevent division by zero
    }
    
    // Calculate bearing change rate (degrees per second)
    double headingDiff = trackInfo.heading - m_prevHeading;
    
    // Normalize heading difference to [-180, 180]
    while (headingDiff > 180.0) headingDiff -= 360.0;
    while (headingDiff < -180.0) headingDiff += 360.0;
    
    m_bearingChangeRate = headingDiff / deltaTime;
    
    // Calculate acceleration (m/s²)
    double velocityDiff = trackInfo.velocity - m_prevVelocity;
    m_acceleration = velocityDiff / deltaTime;
    
    // Calculate climb rate (m/s)
    double altitudeDiff = trackInfo.alt - m_prevAltitude;
    m_climbRate = altitudeDiff / deltaTime;
    
    // Store current values for next iteration
    m_prevHeading = trackInfo.heading;
    m_prevVelocity = trackInfo.velocity;
    m_prevAltitude = trackInfo.alt;
    m_prevUpdateTime = currentTime;
}

void CDrone::updateBatteryLevel(double deltaTime)
{
    if (deltaTime <= 0) return;
    
    // Battery consumption model
    // Base consumption: 2.5 Wh per minute at cruise
    // Additional consumption based on speed and altitude changes
    float baseDrain = 2.5f / 60.0f * deltaTime; // Per second
    
    // Increase drain with speed
    float speedFactor = 1.0f + (m_internalState.groundSpeed / 100.0f);
    
    // Increase drain with vertical movement
    float verticalFactor = 1.0f + qAbs(m_internalState.verticalSpeed) / 10.0f;
    
    // Increase drain with maneuvers (acceleration, turning)
    float maneuverFactor = 1.0f + qAbs(m_internalState.acceleration) / 5.0f + 
                          qAbs(m_bearingChangeRate) / 50.0f;
    
    float totalDrain = baseDrain * speedFactor * verticalFactor * maneuverFactor;
    
    m_internalState.batteryLevel -= totalDrain;
    m_internalState.batteryLevel = qMax(0.0f, m_internalState.batteryLevel);
    
    // Update battery voltage (12.6V full, 9.6V empty for 3S LiPo)
    m_internalState.batteryVoltage = 9.6f + (m_internalState.batteryLevel / 100.0f) * 3.0f;
    
    // Calculate estimated flight time
    if (totalDrain > 0) {
        m_internalState.estimatedFlightTime = (m_internalState.batteryLevel / totalDrain) / 60.0f;
    }
    
    // Update power consumption
    m_internalState.powerConsumption = totalDrain * 150.0f; // Convert to watts
}

void CDrone::checkSystemHealth()
{
    // Check battery level
    if (m_internalState.batteryLevel < 15.0f) {
        m_internalState.flightMode = FLIGHT_MODE_RETURN_TO_BASE;
        m_internalState.healthOk = false;
        m_internalState.statusMessage = "LOW BATTERY - RTB";
        emit emergencyAlert(m_nTrackId, "Low Battery");
        return;
    }
    
    if (m_internalState.batteryLevel < 25.0f) {
        m_internalState.healthOk = false;
        m_internalState.statusMessage = "BATTERY WARNING";
        return;
    }
    
    // Check sensor health
    if (!m_internalState.sensors.gpsActive || m_internalState.sensors.gpsQuality < 50) {
        m_internalState.healthOk = false;
        m_internalState.statusMessage = "GPS DEGRADED";
        return;
    }
    
    if (m_internalState.sensors.linkQuality < 30) {
        m_internalState.healthOk = false;
        m_internalState.statusMessage = "WEAK LINK";
        return;
    }
    
    // All checks passed
    m_internalState.healthOk = true;
    
    // Set status based on flight mode
    switch (m_internalState.flightMode) {
        case FLIGHT_MODE_IDLE:
            m_internalState.statusMessage = "IDLE";
            break;
        case FLIGHT_MODE_TAKEOFF:
            m_internalState.statusMessage = "TAKEOFF";
            break;
        case FLIGHT_MODE_CRUISE:
            m_internalState.statusMessage = "CRUISE";
            break;
        case FLIGHT_MODE_LOITER:
            m_internalState.statusMessage = "LOITER";
            break;
        case FLIGHT_MODE_RETURN_TO_BASE:
            m_internalState.statusMessage = "RTB";
            break;
        case FLIGHT_MODE_LANDING:
            m_internalState.statusMessage = "LANDING";
            break;
        case FLIGHT_MODE_EMERGENCY:
            m_internalState.healthOk = false;
            m_internalState.statusMessage = "EMERGENCY";
            break;
        default:
            m_internalState.statusMessage = "OPERATIONAL";
    }
}

double CDrone::getBearingChangeRate() const
{
    return m_bearingChangeRate;
}

double CDrone::getDirection() const
{
    return m_internalState.yaw;
}

QString CDrone::getFlightModeString() const
{
    switch (m_internalState.flightMode) {
        case FLIGHT_MODE_IDLE:
            return "IDLE";
        case FLIGHT_MODE_TAKEOFF:
            return "TAKEOFF";
        case FLIGHT_MODE_CRUISE:
            return "CRUISE";
        case FLIGHT_MODE_LOITER:
            return "LOITER";
        case FLIGHT_MODE_RETURN_TO_BASE:
            return "RTB";
        case FLIGHT_MODE_LANDING:
            return "LANDING";
        case FLIGHT_MODE_EMERGENCY:
            return "EMERGENCY";
        default:
            return "UNKNOWN";
    }
}

QColor CDrone::getHealthStatusColor() const
{
    if (!m_internalState.healthOk) {
        // Critical issues
        if (m_internalState.batteryLevel < 15.0f || 
            m_internalState.flightMode == FLIGHT_MODE_EMERGENCY) {
            return QColor(231, 76, 60); // Red
        }
        // Warning issues
        return QColor(241, 196, 15); // Yellow
    }
    
    // All good
    return QColor(46, 204, 113); // Green
}

void CDrone::simulateRealisticBehavior()
{
    // Simulate random variations in drone behavior for testing
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Simulate battery drain
    m_internalState.batteryLevel = qMax(0.0f, m_internalState.batteryLevel - 0.1f);
    
    // Simulate sensor variations
    m_internalState.sensors.gpsQuality = 85 + rng->bounded(15);
    m_internalState.sensors.linkQuality = 90 + rng->bounded(10);
    
    // Simulate environmental changes
    m_internalState.temperature = 20.0f + rng->bounded(15);
    m_internalState.windSpeed = rng->bounded(20);
    m_internalState.windDirection = rng->bounded(360);
    
    // Simulate mission progress
    if (m_internalState.waypointIndex < m_internalState.totalWaypoints) {
        if (rng->bounded(100) < 5) { // 5% chance to advance waypoint
            m_internalState.waypointIndex++;
        }
    }
    
    // Randomly change flight mode (rarely)
    if (rng->bounded(1000) < 2) { // 0.2% chance
        int mode = rng->bounded(7);
        m_internalState.flightMode = static_cast<eDroneFlightMode>(mode);
    }
    
    checkSystemHealth();
    emit internalStateChanged();
}
