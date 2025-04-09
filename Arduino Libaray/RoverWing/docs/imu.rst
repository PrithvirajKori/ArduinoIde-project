===========
IMU and GPS
===========
This section describes the functions for using the built-in Inertial Motion
Unit (IMU) and the external GPS sensor.

.. _imu:

Inertial Motion Unit (IMU)
--------------------------
RoverWing contains a built-in Inertial Motion Unit (IMU), which is based on
ICM42605 chip from Invensense. This chip combines a 3-axis accelerometer and a
3-axis gyro sensor, which provide information about acceleration and rotational
speed. RoverWing firmware combines the sensor data to provide information
about rover orientation in space, in the form of Yaw, Pitch, and Roll angles.
(RoverWing's firmware is based on the work of
`Kris Winer <https://github.com/kriswiner>`__ and uses data fusion
algorithm invented by Sebastian Madgwick.)

.. note::
   Current version of RoverWing firmware requires that the RoverWing be mounted
   horizontally.

Below is the description of functions related to IMU. You can also  check sample
code in :guilabel:`IMU` example sketch included with RoverWing library.



Initialization
~~~~~~~~~~~~~~

By default, the IMU is inactive. To start/stop  it, use the functions below.

.. function:: void  IMUbegin()

   Activate IMU


.. function:: void IMUend()

   Stop the IMU


.. function::  bool IMUisActive()

   Returns true if IMU is active. This function can be used to verify that IMU
   activation was successful.



Calibration
~~~~~~~~~~~

Before use, the IMU needs to be calibrated. The calibration process determines
and then applies corrections (offsets)  to the raw data; without these
corrections, the  data returned by the sensor is very inaccurate.

If you haven't  calibrated the sensor before (or want to recalibrate it),
use the following function:

.. function:: void IMUcalibrate()

       This function will determine and
       apply the corrections; it will also save these corrections in the
       flash storage of the RoverWing microcontroller, where they will be
       stored for future use.  This data is preserved even after you disconnect
       power from your RoverWing board (much like the usual USB flash drive).

       This function will take about 10  seconds to execute; during this time,
       the robot must be completely stationary on a flat horizontal surface.

If you had previously calibrated the sensor, you do not need to repeat the
calibration process - by default, upon initialization the IMU loads previously
saved calibration values.

Note that the IMU is somewhat sensitive to temperature changes, so if the
temperature changes (e.g., you moved your robot from indoors to the street for
testing), it is advised that you recalibrate the IMU.

Reading Values
~~~~~~~~~~~~~~

RoverWing allows you to read both the raw data (accelerometer and gyro readings)
and computed orientation, using the following functions:

.. function:: void getAccel()

   Fetches from the RoverWing raw acceleration data and saves it using member
   variables ``ax``, ``ay``, ``az``, which give the acceleration
   in x-, y-, and z- directions respectively in in units of 1g
   (9.81 m/:math:`sec^2`) as floats.

.. function:: void getGyro()

   Fetches from the RoverWing raw gyro data and saves it using member variables
   ``gx``, ``gy``, ``gz``, which give the angular rotation velocity around
   x-, y-, and z- axes respectively, in degree/s (as floats).

.. function:: float getYaw()

.. function:: float getPitch()

.. function:: float getRoll()

   These functions return yaw, pitch, and roll angles for the robot. These
   three angles describe the robot orientation as described below (this assumes
   that RoverWing is mounted horizontally on the robot, with power supply
   connector facing the back of the robot and the USB port on the right).

   * yaw is the rotation around the vertical axis (positive angle corresponds to
     clockwise rotation, i.e. right turns), relative to the starting position of
     the robot
   * pitch is the rotation around the horizontal line, running from
     left to right. Positive pitch angle corresponds to raising the front of the
     robot and lowering the back
   * roll is the rotation around the horizontal line running from front to back.
     Positive roll angle corresponds to raising the left side of the robot and
     lowering the right.
   For more information about yaw, pitch, and roll angles, please visit
   https://en.wikipedia.org/wiki/Aircraft_principal_axes

.. function:: void getOrientationQuat()

   Gets robot orientation as a unit quaternion (see
   https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation). The result
   can be accessed using member variable ``float quat[4]``, which contains the
   four components of the quaternion:

  ``q=quat[0]+i*quat[1]+j*quat[2]+k*quat[3]``

.. Compass
    RoverWing provides a connector for attaching a combined GPS and a compass
    (magnetometer) sensor. If you have such a sensor connected, you can activate
    it and use to determine absolute orientation using the functions below.

    Note:

    Most GPS/compass combination sensors used for drones provide power to the sensor via the GPS connector. Thus, you need to connect both GPS and compass connector, even if you only intend to use the compass.
    Initialization
    To activate/deactivate the compass sensor, use the following functions.

    void magBegin():
    Activate the compass (magnetometer) sensor.
    void magEnd():
    Stop the compass sensor.
    uint8_t magStatus():
    Returns the status of the compass sensor. You can compare it with one of predefined values:
    MAG_OFF: magnetometer is inactive or absent
    MAG_OK: magnetometer is active
    MAG_CALIBRATING: magnetometer calibration in progress
    Calibration
    As with the IMU, the compass sensor needs to be calibrated before use. You have two options:

    If you have not calibrated the sensor, or want to recalibrate it, you can run the calibration by using the function void magStartCalibration(). Calling this function starts the calibration process. It takes about 20 seconds, during which time you need to move the robot in a figure 8 pattern in 3d (not just rotating around vertical axis!). Make sure that the USB cable used to connect the feather board to the computer is long enough.

    To check when the calibration process is complete, use function magStatus() described above. After the calibration has completed, you can get the calibration data and save it or print to serial monitor for future use. Currently, the calibration process only determines one kind of calibration data, the magnetometer offsets, an array of three integer values. To get these values, use:

    void magGetOffsets(int16_t offsets[3]):
    If you had run the calibration before and have recorded the computed calibration data, you can skip the calibration, instead using the recorded values. To do that, use:

    void magSetCalData(int16_t offsets[3], float matrix[3][3])
    Magnetic declination
    By itself, the compass sensor can only determine robot heading relative to magnetic north, which does not coincide with the true geographic north. If you need heading relative to true north, you need apply correction known as magnetic declination. This correction depends on your geographic location. To learn more about it or find the magnetic declination for your location, you can visit, for example, http://www.magnetic-declination.com/what-is-magnetic-declination.php.

    To set magnetic declination, use the function below:

    void setDeclination(float d):
    which applies the declination: after that, the heading value returned by getHeading will be relative to true north.

    Reading compass data
    Once the compass has been calibrated, you can get the the readings by using the functions below.

    float getHeading():
    Returns current robot heading in degrees. Heading is zero when the robot is pointing north (true north if you have set the declination, magnetic north otherwise); positive values correspond to robot pointing east, negoative, robot pointing west. The returned value is between -180 and 180.
    void getMagData(int16_t m[3]):
    Fetches and saves to array m the raw magnetometer readings, i.e., the x-, y-, and z- components of the magnetic field, in units of 0.93 milligauss. Note that these values are not affected by the magnetic declination.

GPS
---

If you have connected a GPS sensor to RoverWing as described in |guide|, you
can use the functions below to access it.


Initialization
~~~~~~~~~~~~~~

.. function:: void GPSbegin()

   Start the GPS. Note that after starting, it can take the sensor a while to
   get GPS location fix: the time ranges from several seconds if the sensor
   had recently been used in a nearby location to several minutes if the sensor
   has been moved to a completely new location.

.. function:: void GPSend()

   Stops the GPS sensor.

.. function:: uint8_t GPSstatus()

   Gets current GPS status. Possible values are

   * ``GPS_OFF``: GPS is inactive
   * ``GPS_OK``: GPS is active and has a valid location fix
   * ``GPS_WAITING``: GPS is active, but is waiting to receive a location fix.
     The sensor switches to this status if it hasn't received a valid GPS
     signal for more than 3 seconds.

Usage
~~~~~
After the GPS has been initialized and received location fix, you can use the
following functions to access the GPS coordinates.

.. function:: void getGPSlocation()

   Gets from RoverWing and saves the latest GPS location data, which can later
   be accessed using the  functions below.

.. function:: double latitude()

.. function:: double longitude()

   Return the robot latitude and longitude in degrees, following the usual
   conventions: latitude ranges from -90 (South Pole) to 90 (North Pole);
   longitude ranges from  -180 (west of Greenwich) to 180 (east of Greenwich).
   Note that these coordinates refer to the location fetched at last call of
   :func:`getGPSlocation`.

.. function:: int32_t latitudeL()

.. function:: int32_t longitudeL()

   Return longitude and latitude of the robot, in units of :math:`10^{-7}`
   degree (about 10 cm).

.. function:: uint32_t GPStimestamp()

   Returns time when the last GPS location fix was received, in milliseconds
   since reboot of the RoverWing board.

Location Data
~~~~~~~~~~~~~

RoverWing library provides a type for storing GPS location and timestamp. It is
defined in :file:`RoverWing.h` as follows::

    struct location_t {
      int32_t latitude; //latitude, in units of 10^{-7} degree
      int32_t longitude;
      uint32_t timestamp; //in ms, as reported by millis()
    };

The functions below provide some tools for working with location data:

.. function:: void saveGPSlocation(location_t & loc)

   Saves current robot location to variable ``loc`` ot type ``location_t``.

.. function:: float distanceTo(const location_t & loc )

   Returns distance from current robot location to location ``loc``, in meters.

.. function:: float bearingTo(const location_t & loc )

   Returns bearing from current robot location to ``loc``. The bearing is
   measured in degrees and ranges from -180 to 180, with North being 0.

.. note::

   Functions :func:`distanceTo`, :func:`bearingTo` use flat map model. The
   results are accurate enough for distances up to 10 km, but if you want to
   find the distance between your robot and Mount Everest, you need to write
   your own code or google for existing solutions (unless you are within 10
   km of Mount Everest). 
