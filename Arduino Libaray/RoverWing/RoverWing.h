#include "Arduino.h"
#include "Wire.h"
#include "regmap.h" //register map definitions

#define ROVERWING_ADDRESS 0x11
/* ***************************************
 * Motors and servo related definitions
 *******************************************/
enum motor_t {
  MOTOR1=0, MOTOR2
};
enum servo_t {
  SERVO1=0, SERVO2, SERVO3, SERVO4
};
enum sonar_t {
  SONAR1=1, SONAR2=2, SONAR3=4
};
/*// servo codes used in activation bitmask
#define SONAR1_ACT (0x01u)
#define SONAR2_ACT (0x02u)
#define SONAR3_ACT (0x04u) */
struct motorconfig_t{
  uint16_t encoderCPR; //encoder counts per revolution of output shaft
  uint16_t noloadRPM;
  float Kp;            // PID coefficicients for speed control. They will be used as follows
                       // motor power = maxpower * Kp * (error+1/T_i errorInt + Td*errorDer)
                       // where time is in seconds, and error is speed error  in encoder tick/s
  float Ti;
  float Td;
  float iLim;        // this limits the contribution of integral term: |errorInt|<=iLim
};
// Motor configuration modes
#define MOTOR_MODE_POWER 0 //includes brake
#define MOTOR_MODE_COAST 1
#define MOTOR_MODE_SPEEDPID 2
// For coasting a motor:
// Normally, motor power is an int between -500...500
// this special value indicates that the motor should be stopped in coast state
#define POWER_COAST 1000
#define MOTOR_MAX_POWER 500

/* ***************************************
 * IMU
 *******************************************/
//statuses
#define IMU_OFF 0x00
#define IMU_OK  0x01
#define IMU_CALIBRATING 0x02
//configuration mode used in communication with feather
#define IMU_CONFIG_BEGIN 0x01
#define IMU_CONFIG_CALIBRATE 0x02
#define IMU_CONFIG_END 0x00


/* ***************************************
 * Neopixel named colors
 *******************************************/
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xAA9900
#define ORANGE 0xAA5500
#define WHITE 0xFFFFFF
#define OFF 0x000000
/* ***************************************
 * GPS
 *******************************************/
//gps statuses
#define GPS_OFF 0
#define GPS_WAITING 1 //waiting for fix
#define GPS_OK 2      // has fix
//location
struct location_t {
  int32_t latitude; //latitude, in units of 10^{-7} degree
  int32_t longitude;
  uint32_t timestamp; //in ms, as reported by millis()
};
//other constants for GPS
#define EARTH_RADIUS_KM  (6371.0f)
#define  RAD_PER_DEG  (PI / 180.0f)
#define   DEG_PER_RAD  (180.0f / PI)
const double  LOC_SCALE = 1.0e-7;
// Magnetomoter
//statuses
#define MAG_OFF 0x00
#define MAG_OK 0x01
#define MAG_CALIBRATING 0x02
//configuration modes
#define MAG_CONFIG_BEGIN 0x01
#define MAG_CONFIG_CALIBRATE 0x02
#define MAG_CONFIG_END 0x00
//drive modes
#define DRIVE_OFF 0x00
#define DRIVE_STRAIGHT 0x01
#define DRIVE_STRAIGHT_DISTANCE 0x02
#define DRIVE_TURN 0x03
//drive statuses
#define DRIVE_STATUS_COMPLETE 0x00
#define DRIVE_STATUS_INPROGRESS 0x01



/********************************************
   ROVER DRIVING
***********************************************/
struct driveconfig_t {
  //define defaults
  driveconfig_t():  minPower(0.0f), leftMotorReversed(false), rightMotorReversed(false), Kp(-1.0) {}
  motor_t leftMotor;
  motor_t rightMotor;
  bool leftMotorReversed;
  bool rightMotorReversed;
  //bool withEncoders;
  uint16_t wheelDiameter; //in mm
  uint16_t wheelBase; //in mm
  float minPower; //between 0-1.0; the minimal power necessary for the robot to move
  float Kp;
  float Ti;
  float Td;
  float iLim;
};


class Rover {
  public:
    /////////////////
    // Class members
    ////////////////
    uint8_t fwVersionMajor; //major version #
    uint8_t fwVersionMinor;
    float analog[7];      //averaged readings of analog sensors, in volts
    //uint16_t analogRaw[7];//raw readings of analog sensors, 0-1023
    float sonar[3];       //averaged distances in mm
    //uint16_t sonarRaw[4]; //raw distances in mm
    float position[2];    //current motor positions (since last encoder reset), in revolutions
    float speed[2];       //current motor speed, in RPM
    float voltage;        //current voltage, in volts
    float quat[4];        //current orientation as a quaternion
    float yaw;
    float pitch;
    float roll;
    float ax;             //Acceleration, in g
    float ay;
    float az;
    float gx;             //gyro rotation speeds, in deg/s
    float gy;
    float gz;
    int16_t debug[3];   //misc debugging info
    /////////////////
    // Public functions
    ////////////////

    //general functions
    bool begin(); //returns true if init was successfull
    String fwVersion();
    void beginVerbose();//starts roverwing printing basic info such as fw version to Serial
    //analog inputs
    void setLowVoltage(float v);
    float getAnalog(uint8_t input);
    float getVoltage();
    void getAllAnalog();
    //sonars
    void activateSonars(uint8_t bitmask, int maxDistance=6000); //max distance in mm
    void stopSonars();
    void getAllSonar();
    float getSonar(sonar_t s);

    //servos
    void setServo(servo_t s, float pos); // -1<= pos <=1
    void setAllServo(float* pos);
    void setServoRange(servo_t s, int minPulse, int maxPulse);

    //motors
    void setMotorPwr(motor_t m, float pwr);
    void setAllMotorPwr(float pwr1, float pwr2);
    void stopMotors();
    void configureMotor(motor_t m, motorconfig_t c);
    void reverseMotor(motor_t m);
    void setMotorSpeed(motor_t m, float speed);//speed in RPM

    //encoders
    float getPosition(motor_t m);
    void getAllPosition();
    float getSpeed(motor_t m);
    void getAllSpeed();
    void resetEncoder(motor_t m);
    void resetAllEncoder();

    //imu
    void  IMUbegin(); //initializes IMU. Note: part of initialization is calibration
                      // of gyro and accelerometer sensors, which takes some time (about 1sec)
                      //During this time, robot should be absolutely still and even
                      //Avoid using other commnads that communicate with the robot during this time
    void IMUend();    //deactivate IMU
    void IMUcalibrate();
                                          //calibrates IMU; saves found offsets for gyro in array
                                          // offsets, for future use
    //oid IMUsetOffsets(int16_t * aOffsets, int16_t * gOffsets);
    bool IMUisActive();
    void getOrientationQuat();
    void getAccel();
    void getGyro();
    float getYaw();    //return yaw, pitch, and roll angles, in degrees
    float getPitch();
    float getRoll();
    //GPS
    void GPSbegin();        //start GPS
    void GPSend();
    uint8_t GPSstatus();   //get current GPS status from the wing
    void getGPSlocation(); //get current GPS location
    void saveGPSlocation(location_t & l); //save current GPS location to l
    double latitude() const {return (double)location.latitude*LOC_SCALE;} //latitude, as a double
    double longitude() const {return (double)location.longitude*LOC_SCALE;}//longitude, as double
    int32_t latitudeL() const {return location.latitude;}   //latitude as an int, in untis of 10^{-7} deg
    int32_t longitudeL() const {return location.longitude;} //longitude as an int, in untis of 10^{-7} deg
    uint32_t GPStimestamp() const {return location.timestamp;}
    float  distanceTo(const location_t & l );  //distance form currrent location to l, in meters
    float  bearingTo(const location_t & l );   //bearing from current position to l, in degrees relative to true north
    //magnetometer
    void magBegin();
    void magEnd();
    void magSetCalData(int16_t  offsets[3], float matrix[3][3]);
    void magStartCalibration();
    void magGetOffsets(int16_t offsets[3]);
    uint8_t magStatus();
    void setDeclination(float d); //set magentic declination for current location
    float getHeading(); // current robot heading in degrees, relative to true north
    void getMagData(int16_t  m[3]);

    //neopixels

    void setPixelCount(uint8_t n);     //configure how many neopixels we have connected (not counting the internal one )
    void setPixelBrightness(uint8_t b); //0-255. Usually brightness of 32 (1/8 of maximum) is bright enough
    void setPixelColor(uint8_t pixel_index, uint32_t c); // C is a hex color: c=0xRRGGBB
    void setPixelRGB(uint8_t pixel_index, uint8_t R, uint8_t G, uint8_t B);
    void setPixelHSV(uint8_t pixel_index, uint8_t H, uint8_t S, uint8_t V); //pixel color, in hue-saturation-value form
    void showPixel();   //after setting individual pixel colors, you need to use this to make the changes effective
    void setTopLED(uint32_t color);
    void setTopLED(uint32_t color1,uint32_t color2, uint32_t color3);

    //Rover driving
    // before using these functions, you need to set rover.drive confiuration
    void configureDrive(driveconfig_t d);
    void setDriveRampTime(uint16_t t);//time in ms to go from 0 to full power
    //forward motion
    void startForward(float power); // between 0..1.0
    void startForward(float power, int32_t distance); // power between 0..1.0, disatnce in mm
    void goForward(float power, int32_t distance); //distance in mm
    //backward motion
    void startBackward(float power);//between  0..1.0
    void startBackward(float power, int32_t distance);//power between  0..1.0
    void goBackward(float power, int32_t distance);   //distance in mm
    //turns
    void startTurn(float power, float degrees);
    void turn(float power, float degrees); //power between 0..1.0
                                           // positive angle is for clockwise rotation

    //general drive control
    bool driveInProgress();
    int32_t distanceTravelled();    //returns distance in mm, signed
    void stop();
    float getTargetHeading() const {return targetHeading;}
    //debug
    void getDebug();

  private:
    /////////////////////////////////////////////
    // members
    ////////////////////////////////////////////
    uint8_t activeSonarsBitmask;
    motorconfig_t motorsConfig[2];
    bool motorIsReversed[2];
    // records servo center position and half range for each servo
    // full range is [center-halfrange, center+halfrange]
    uint16_t servoCenterPos[4];
    uint16_t servoHalfRange[4];
    //current location
    location_t location;
    float declination; //magnetic declination, in degrees
    driveconfig_t drive;
    // target heading in degrees, as yaw angle. Must be  between -180.0 and 180.0
    float targetHeading;
    bool headingSet;
    // helper function for staright movement
    void prepareDrive(float power);
    //////////////////////////////////////////////
    //     I2C helper functions
    /////////////////////////////////////////////
    //  BYTE LEVEL FUNCTIONS
    ////////////////////////
    // read single byte at given register address
    uint8_t readByte(uint8_t regAddress);
    // read multiple bytes and save to dest[] array
    void readByte(uint8_t regAddress, uint8_t count, uint8_t * dest);

    //write single byte
    void writeByte(uint8_t regAddress, uint8_t data);
    // write multiple bytes  from data[] array
    void writeByte(uint8_t regAddress, uint8_t count, uint8_t * data);
    /////////////////////////////////
    // OPERATIONS ON 2-BYTE (16 bit) values
    // note that register offset is still measured in 1-byte units
    uint16_t read16(uint8_t regAddress);
    void read16(uint8_t regAddress, uint8_t count, uint16_t * dest);
    void write16(uint8_t regAddress, uint16_t data);
    void write16(uint8_t regAddress, uint8_t count,  uint16_t * data);
    /////////////////////////////////
    // OPERATIONS ON 4-BYTE (32 bit) values
    uint32_t read32(uint8_t regAddress);
    void read32(uint8_t regAddress, uint8_t count, uint32_t * dest);
    void write32(uint8_t regAddress, uint32_t data);
    void write32(uint8_t regAddress, uint8_t count,  uint32_t * data);
};
