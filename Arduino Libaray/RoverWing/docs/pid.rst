.. _pid:

=============================
Advanced Motor Control
=============================

Closed Loop Motor control
-------------------------

If your motors are equipped with encoders, you can use encoder feedback to
control motor speed. RoverWing firmware does it using so-called PID
(Proportional-Integral-Differential) algorithm; you can read more on PID
`here <https://en.wikipedia.org/wiki/PID_controller>`__.

Before using the closed loop speed control, you need to provide the basic
information about the motor and encoder. At the very minimum, you need to set,
for each motor, the motor maximal RPM and number of encoder counts per
revolution of motor output shaft, as described in section :ref:`encoders`.

This will automatically configure the coefficients used by PID algorithm.
Advanced users can override this and set their own PID coefficients, as
described below.

After configuring the motors, you can use the following function to set motor speed:

.. function:: setMotorSpeed(motor_t m, float speed)

   Runs motor at given speed (in RPM), using  encoder feedback and PID
   algorithm to maintain constant speed.


Tank Drive
----------

RoverWing provides commands for higher level drive operations: instead of
controlling each motor separately, you can use commands such as "turn 90 degrees
right". To use these commands, the following conditions must be met:

* Your robot uses tank drive: one motor controls wheel(s) or tracks on the left
  side of the robot, the other, on the right side of the robot. (This is by far
  the most common way of driving small mobile robots.)

*  Both motors are identical. If they use encoders, the encoders must be
   identical, too. However, many of the drive functions can be used without
   encoders.

Tank drive configuration
~~~~~~~~~~~~~~~~~~~~~~~~

Before using the tank drive commands, you must:

1. initialize and calibrate the IMU (the drive uses IMU to control movement) as
   described in section :ref:`imu`.

2. configure each motor using :func:`configureMotor` function, as described in
   :ref:`encoders`.

3. configure the drivetrain, as shown in the example below::

    driveconfig_t drivetrain;
    drivetrain.leftMotor=MOTOR1;
    drivetrain.rightMotor=MOTOR2;
    drivetrain.leftMotorReversed=true;
    drivetrain.wheelDiameter=70;//in mm
    drivetrain.wheelBase=140;   //in mm
    drivetrain.minPower=0.05;   // need at least 5% to move
    //now apply these values
    r.configureDrive(drivetrain);
    r.setDriveRampTime(1000); //set ramping up time to be 1 sec

Here is the explanation of various configuration parameters.

drivetrain.leftMotor, drivetrain.rightMotor:
    Which motor controls left/right side of the robot

drivetrain.leftMotorReversed, drivetrain.rightMotorReversed:
    If one of the motors is set so that sending it positive power causes the
    corresponding side of the robot move backward, set this parameter to true.
    Do not use :func:`reverseMotor` function together with this.

drivetrain.wheelDiameter:
    Wheel/sprocket diameter, in mm.

drivetrain.wheelBase:
    Robot base, i.e. distance between left and right wheels, in mm.

``WheelDiameter`` and ``wheelBase`` parameters need not be exact - if they are
10% off, it is not going to change things much.

drivetrain.minPower:
    Minimal power needed for the robot to move (as fraction of the full power).
    This parameter is there to avoid the following common problem: when driving
    for fixed distance, the software reduces power to the motors to achieve
    smooth stop - but at some moment, power becomes so low that the robot is not
    moving at all. Setting this parameter avoids this problem: algorithm used by
    RoverWing will never reduce the power below that level. The value of this
    parameter depends on the robot weight, floor friction, and motor power.
    Suggested values are between 0.05 (for a robot on a low-friction surface
    such as hardwood floor) and 0.2 (for high-friction situations such as thick
    carpet). If not certain, begin with higher values (such as 0.3) and then
    decrease. If you find that the robot never completes drive operations such
    as :func:`turn` or func:`goForward`, it indicates that your ``minPower`` value
    is too low.  

setDriveRampTime(int t):
    When starting the motion, it is usually a bad idea to go from robot at rest
    to maximal power instantly, just as it is usually a bad idea to stop the
    robot by abruptly changing the motor power from 100% to 0: in both cases,
    this will likely result in the robot skidding on the floor, and probably
    slightly turning in an unpredictable way. To avoid this, when starting the
    robot drive, RoverWing software increases the speed from 0 to the requested
    motion speed gradually; this is called "ramping up" the speed. Similarly,
    when stopping, RoverWing gradually decreases the robot speed. Function
    ``setDriveRampTime()`` controls the speed of this ramping up process: calling
    ``setDriveTime(t)`` sets the ramping up time to go from 0 to maximal power
    to be   ``t`` milliseconds. For example, ``setDriveTime(1000)`` sets the
    ramp up time to be 1 second. Note that this sets the time to go from 0 to
    maximal power; if you are setting the robot power to be less than maximal,
    then the ramp up time will be proportionally decreased. For example, if you
    used ``setDriveTime(1000)`` and then used the command ``startForward(0.5)``
    to start the robot moving at 50% power (see below), then the ramp up time
    would be 0.5 sec.


Using Tank Drive
~~~~~~~~~~~~~~~~

After configuring the tank drive as described in the previous section, you can
use the following commands.

.. function:: void goForward(float power, int32_t distance)

    Go forward at given power (between 0 and 1.0) for given distance (in mm). This
    function requires encoders (this is the only way to measure distance) but uses
    the IMU - not encoders - to maintain robot direction. This function is blocking:
    it does not return until the robot has completed the movement. If this is not
    acceptable (for example, because this interferes with other parts of your
    program such as WiFi communication), use :func:`startForward` function
    below.

.. function:: void startForward(float power, int32_t distance)

   Non-blocking version of :func:`goForward`. This function starts the robot
   motion and immediately returns; the robot continues driving straight until
   it reaches the specified distance or receives another drive command. You
   can use function :func:`driveInProgress` described below to test whether
   the robot has completed the motion.

.. function:: void startForward(float power)

   Starts the robot motion forward, at given power between 0 and 1.0. The
   function returns immediately; the robot will continue driving straight
   until it receives another drive commands such as :func:`stop`. This function
   does not require encoders; it uses the IMU to maintain robot direction.

.. function:: void goBackward((float power, int32_t distance)

.. function:: void startBackward(float power, int32_t distance)

.. function:: void startBackward(float power)

    Similar to ``goForward``, ``startBackward``, but for moving backwards.
    Note that power and distance should be positive!

.. function:: void turn(float power, float degrees)

    Turns the robot by given angle at given power. Power should be between 0
    and 1.0; the angle must be between -180 and 180. Positive angle corresponds
    to clockwise (right) turn. This function does not require encoders; it uses
    the IMU to measure turn angle. This function is blocking: it does not
    return until the robot has completed the movement. If this is not
    acceptable (for example, because this interferes with other parts of your
    program such as WiFi communication), use :func:`startTurn` function below.

.. function:: void startTurn(float power, float degrees)

    Non-blocking version of :func:`turn`. This function starts the robot
    motion and immediately returns; the robot continues turning until it has
    turned by the specified angle or receives another drive command. You can
    use function :func:`driveInProgress` described below to test whether the
    robot has completed the motion.

.. function:: void stop()

   Stops the robot, ending any drive operation currently in process. Note that
   this function stops the robot immediately, without any ramping down of the
   speed.

..  function:: bool driveInProgress()

    Returns true if a drive operation is currently in progress. This function
    can be used to detect when the robot has completed a drive operation
    initiated by ``startForward(power, distance)`` or
    ``startTurn(power, angle)``, as illustrated in the following example::

        r.startForward(0.5, 800);//start motion at 50% power for 80 cm
        delay(10);
        while (r.driveInProgress()){
          //do something else, e.g. check for commands received via bluetooth
          delay(10);
        }
        //motion forward has completed!

.. warning::
   Setting motor speed manually using :func:`setMotorPwr` or similar functions
   while a drive operation is in progress can lead to unpredictable behavior.
   To avoid this,  use :func:`stop` to make sure that all drive operations
   are complete before starting manual control of the motors.



Setting PID Coefficients
------------------------

For advanced users, below is the detailed explanation of the PID algorithm used
by RoverWing firmware.

The motor power is determined by the standard formula of PID algorithm:

.. math::
   P=P_0+ K_p e+\frac{K_p}{T_i}\int e\, dt +K_p T_d \frac{d}{dt} e

where:

:math:`P` is motor power (ranging from -1.0 to 1.0)

:math:`P_0=v_{desired}/v_{max}` is the zero-level approximation; here
:math:`v_{desired}` is the requested speed and :math:`v_{max}` is the maximal
possible motor speed, which is determined by motor's no-load RPM.

:math:`e=v_{desired}-v_{actual}` is the error, i.e. the  difference of desired and actual motor
speeds (measured in encoder tics/sec)

:math:`\int e dt` and :math:`\frac{d}{dt}e` are the integral and derivative
of the error, measured in encoder tics and tics/:math:`sec^2` respectively

:math:`K_p, T_i, T_d` are the PID coefficients.


The behavior of the PID algorithm is determined by these coefficients; for
example, if the coefficient :math:`K_p` is too small, it will take the motor a
long time to stabilize to the desired speed; if the coefficient :math:`K_p` is
too large, you might get oscillations. Choosing the correct coefficients
requires significant experience and is certainly outside the scope of this user
guide.


RoverWing software sets some default values for the PID coefficients based on
motor maximal RPM and encoder tics per revolution. These default values are
somewhat conservative: they are almost certain to avoid instability and large
oscillations, but they take some time for the motor speed to stabilize.
Advanced users can change them and set their own PID coefficients as shown in
the sketch below.

::

    motorconfig_t myMotor;
    myMotor.encoderCPR = 1440;
    myMotor.noloadRPM = 250; // the motor RPM under maximal power; you can find it by running example sketch "Servos and Motors Basic"
    float maxspeed=myMotor.encoderCPR*myMotor.noloadRPM/60.0; //max motor speed in encoder counts/s
    myMotor.Kp=0.6/maxspeed;  //suggested proportional  gain. If the motor is too slow to achieve desired speed, increase; if the motor starts oscillating, decrease.
    myMotor.Ti=0.3;           // time constant for integral gain, in seconds. To disable I term, make Ti large, e.g. 100000.0
    myMotor.Td=0.1;           // time constant for differential gain, in seconds. To disable differential term, make Td=0.0
    myMotor.iLim = 1.0*myMotor.Ti/myMotor.Kp; // limit on integral error; this value guarantees that integral term will be at most 1.0*maxspeed
                                              // to disable integral limit, make iLim negative e.g. -1.0
    //finally, configure the motors
    r.configureMotor(MOTOR1, myMotor);
    r.configureMotor(MOTOR2, myMotor);

.. note::
   If you are manually setting PID coefficients, you must set all of them.
   Leaving any of the values unset will result in unpredictable results.
