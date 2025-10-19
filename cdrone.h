#ifndef CDRONE_H
#define CDRONE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include "globalstructs.h"

/**
 * @brief Flight modes for the drone
 */
enum eDroneFlightMode {
    FLIGHT_MODE_IDLE = 0,
    FLIGHT_MODE_TAKEOFF = 1,
    FLIGHT_MODE_CRUISE = 2,
    FLIGHT_MODE_LOITER = 3,
    FLIGHT_MODE_RETURN_TO_BASE = 4,
    FLIGHT_MODE_LANDING = 5,
    FLIGHT_MODE_EMERGENCY = 6
};

/**
 * @brief Sensor status for the drone
 */
struct stDroneSensorStatus {
    bool gpsActive;          //!< GPS sensor active
    bool imuActive;          //!< IMU sensor active
    bool cameraActive;       //!< Camera active
    bool radarActive;        //!< Radar sensor active
    bool lidarActive;        //!< LIDAR sensor active
    int gpsQuality;          //!< GPS signal quality (0-100%)
    int linkQuality;         //!< Communication link quality (0-100%)
};

/**
 * @brief Drone internal state information
 */
struct stDroneInternalState {
    // Power and battery
    float batteryLevel;      //!< Battery level (0-100%)
    float batteryVoltage;    //!< Battery voltage in volts
    float powerConsumption;  //!< Current power consumption in watts
    float estimatedFlightTime; //!< Estimated remaining flight time in minutes
    
    // Flight dynamics
    float pitch;             //!< Pitch angle in degrees
    float roll;              //!< Roll angle in degrees
    float yaw;               //!< Yaw angle in degrees
    float verticalSpeed;     //!< Vertical speed in m/s
    float groundSpeed;       //!< Ground speed in m/s
    float acceleration;      //!< Current acceleration in m/s²
    
    // Mission and status
    eDroneFlightMode flightMode;  //!< Current flight mode
    QString missionId;       //!< Mission identifier
    int waypointIndex;       //!< Current waypoint index
    int totalWaypoints;      //!< Total number of waypoints
    float missionProgress;   //!< Mission progress (0-100%)
    
    // Sensors
    stDroneSensorStatus sensors; //!< Sensor status
    
    // Environmental
    float temperature;       //!< Temperature in Celsius
    float windSpeed;         //!< Wind speed in m/s
    float windDirection;     //!< Wind direction in degrees
    
    // System health
    bool healthOk;           //!< Overall system health status
    QString statusMessage;   //!< Current status message
    QDateTime lastUpdateTime; //!< Last update timestamp
};

/**
 * @brief CDrone class - Represents a drone with internal state and dynamics
 */
class CDrone : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param trackId Track ID associated with this drone
     * @param parent Parent QObject
     */
    explicit CDrone(int trackId, QObject *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~CDrone();
    
    /**
     * @brief Get the track ID
     * @return Track ID
     */
    int getTrackId() const { return m_nTrackId; }
    
    /**
     * @brief Get drone internal state
     * @return Drone internal state structure
     */
    stDroneInternalState getInternalState() const { return m_internalState; }
    
    /**
     * @brief Update drone position and dynamics
     * @param trackInfo Updated track display information
     */
    void updateDynamics(const stTrackDisplayInfo &trackInfo);
    
    /**
     * @brief Update drone internal state (battery, sensors, etc.)
     * @param state New internal state
     */
    void updateInternalState(const stDroneInternalState &state);
    
    /**
     * @brief Calculate bearing change rate (degrees per second)
     * @return Bearing change rate
     */
    double getBearingChangeRate() const;
    
    /**
     * @brief Calculate direction of movement
     * @return Direction in degrees
     */
    double getDirection() const;
    
    /**
     * @brief Get human-readable flight mode string
     * @return Flight mode string
     */
    QString getFlightModeString() const;
    
    /**
     * @brief Get drone health status color
     * @return QColor representing health (Green=Good, Yellow=Warning, Red=Critical)
     */
    QColor getHealthStatusColor() const;
    
    /**
     * @brief Simulate realistic drone behavior (for testing)
     */
    void simulateRealisticBehavior();

signals:
    /**
     * @brief Emitted when drone internal state changes
     */
    void internalStateChanged();
    
    /**
     * @brief Emitted when drone enters emergency mode
     */
    void emergencyAlert(int trackId, QString reason);

private:
    int m_nTrackId;                      //!< Associated track ID
    stDroneInternalState m_internalState; //!< Current internal state
    
    // Previous state for dynamics calculation
    double m_prevHeading;                //!< Previous heading
    double m_prevVelocity;               //!< Previous velocity
    double m_prevAltitude;               //!< Previous altitude
    QDateTime m_prevUpdateTime;          //!< Previous update timestamp
    
    // Calculated dynamics
    double m_bearingChangeRate;          //!< Bearing change rate (deg/s)
    double m_acceleration;               //!< Acceleration (m/s²)
    double m_climbRate;                  //!< Climb rate (m/s)
    
    /**
     * @brief Calculate dynamics based on position updates
     * @param trackInfo Current track information
     */
    void calculateDynamics(const stTrackDisplayInfo &trackInfo);
    
    /**
     * @brief Update battery level based on flight dynamics
     * @param deltaTime Time elapsed since last update (seconds)
     */
    void updateBatteryLevel(double deltaTime);
    
    /**
     * @brief Check system health and update status
     */
    void checkSystemHealth();
};

#endif // CDRONE_H
