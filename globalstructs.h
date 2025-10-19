#ifndef GLOBALSTRUCTS_H
#define GLOBALSTRUCTS_H
#include <QString>
#include <QList>
#include <QPointF>

// Forward declaration
class CDrone;

enum eTrackIdentity {
    TRACK_IDENTITY_DEFAULT = 0,
    TRACK_IDENTITY_UNKNOWN = 1,
    TRACK_IDENTITY_FRIEND = 2,
    TRACK_IDENTITY_HOSTILE = 3,
};

// Structure to store a single history point
struct stTrackHistoryPoint {
    double lat;
    double lon;
    double alt;
    long long timestamp;
};

#pragma pack(1)
// Replace with your actual structure definition
struct stTrackRecvInfo {
    unsigned short usMsgId;    //!< Msg ID
    int nTrkId;                //!< Track ID
    float x;                   //!< X-coordinate
    float y;                   //!< Y-coordinate
    float z;                   //!< Z-coordinate
    float heading;             //!< Heading
    float velocity;            //!< Velocity
    int nTrackIden;
};

struct stTrackDisplayInfo {
    int nTrkId;                 //!< Track ID
    float x;                    //!< X-coordinate
    float y;                    //!< Y-coordinate
    float z;                    //!< Z-coordinate
    double lat;                 //!< Latitude
    double lon;                 //!< Longitude
    double alt;                 //!< Altitude
    double range;               //!< Range
    double azimuth;             //!< Azimuth
    double elevation;           //!< Elevation
    double heading;             //!< Heading
    double velocity;            //!< Velocity
    double snr;                 //!< SNR
    int nTrackIden;
    long long nTrackTime;
    QString tooltip;  // ADD THIS LINE
    QString imagePath;          //!< Optional image path for custom track/drone icon
    QList<stTrackHistoryPoint> historyPoints;  //!< Track history points
    bool showHistory;           //!< Flag to show/hide history trail
    CDrone* pDrone;             //!< Pointer to associated drone object (nullptr if not a drone)
};

#pragma pack()

#endif // GLOBALSTRUCTS_H
