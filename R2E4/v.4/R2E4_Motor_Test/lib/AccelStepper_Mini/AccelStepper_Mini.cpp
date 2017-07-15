// AccelStepper_Mini.cpp
//
// Copyright (C) 2009-2013 Mike McCauley
// $Id: AccelStepper.cpp,v 1.23 2016/08/09 00:39:10 mikem Exp mikem $

#include "AccelStepper_Mini.h"

#if 0
// Some debugging assistance
void dump(uint8_t* p, int l)
{
    int i;

    for (i = 0; i < l; i++)
    {
	Serial.print(p[i], HEX);
	Serial.print(" ");
    }
    Serial.println("");
}
#endif

void AccelStepper_Mini::moveTo(long absolute)
{
    if (_targetPos != absolute)
    {
	_targetPos = absolute;
	computeNewSpeed();
	// compute new n?
    }
}

void AccelStepper_Mini::move(long relative)
{
    moveTo(_currentPos + relative);
}

// Implements steps according to the current step interval
// You must call this at least once per step
// returns true if a step occurred
boolean AccelStepper_Mini::runSpeed()
{
    // Dont do anything unless we actually have a step interval
    if (!_stepInterval)
	return false;

    unsigned long time = micros();
    if (time - _lastStepTime >= _stepInterval)
    {
	if (_direction == DIRECTION_CW)
	{
	    // Clockwise
	    _currentPos += 1;
	}
	else
	{
	    // Anticlockwise
	    _currentPos -= 1;
	}
	step(_currentPos);

	_lastStepTime = time;
	return true;
    }
    else
    {
	return false;
    }
}

long AccelStepper_Mini::distanceToGo()
{
    return _targetPos - _currentPos;
}

long AccelStepper_Mini::targetPosition()
{
    return _targetPos;
}

long AccelStepper_Mini::currentPosition()
{
    return _currentPos;
}

// Useful during initialisations or after initial positioning
// Sets speed to 0
void AccelStepper_Mini::setCurrentPosition(long position)
{
    _targetPos = _currentPos = position;
    _n = 0;
    _stepInterval = 0;
    _speed = 0;
}

void AccelStepper_Mini::computeNewSpeed()
{
    long distanceTo = distanceToGo(); // +ve is clockwise from curent location

    long stepsToStop = (long)((_speed * _speed) / (2 * _acceleration)); // Equation 16

    if (distanceTo == 0 && stepsToStop <= 1)
    {
	// We are at the target and its time to stop
	_stepInterval = 0;
	_speed = 0;
	_n = 0;
	return;
    }

    if (distanceTo > 0)
    {
	// We are anticlockwise from the target
	// Need to go clockwise from here, maybe decelerate now
	if (_n > 0)
	{
	    // Currently accelerating, need to decel now? Or maybe going the wrong way?
	    if ((stepsToStop >= distanceTo) || _direction == DIRECTION_CCW)
		_n = -stepsToStop; // Start deceleration
	}
	else if (_n < 0)
	{
	    // Currently decelerating, need to accel again?
	    if ((stepsToStop < distanceTo) && _direction == DIRECTION_CW)
		_n = -_n; // Start accceleration
	}
    }
    else if (distanceTo < 0)
    {
	// We are clockwise from the target
	// Need to go anticlockwise from here, maybe decelerate
	if (_n > 0)
	{
	    // Currently accelerating, need to decel now? Or maybe going the wrong way?
	    if ((stepsToStop >= -distanceTo) || _direction == DIRECTION_CW)
		_n = -stepsToStop; // Start deceleration
	}
	else if (_n < 0)
	{
	    // Currently decelerating, need to accel again?
	    if ((stepsToStop < -distanceTo) && _direction == DIRECTION_CCW)
		_n = -_n; // Start accceleration
	}
    }

    // Need to accelerate or decelerate
    if (_n == 0)
    {
	// First step from stopped
	_cn = _c0;
	_direction = (distanceTo > 0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    else
    {
	// Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
	_cn = _cn - ((2.0 * _cn) / ((4.0 * _n) + 1)); // Equation 13
	_cn = max(_cn, _cmin);
    }
    _n++;
    _stepInterval = _cn;
    _speed = 1000000.0 / _cn;
    if (_direction == DIRECTION_CCW)
	_speed = -_speed;

#if 0
    Serial.println(_speed);
    Serial.println(_acceleration);
    Serial.println(_cn);
    Serial.println(_c0);
    Serial.println(_n);
    Serial.println(_stepInterval);
    Serial.println(distanceTo);
    Serial.println(stepsToStop);
    Serial.println("-----");
#endif
}

// Run the motor to implement speed and acceleration in order to proceed to the target position
// You must call this at least once per step, preferably in your main loop
// If the motor is in the desired position, the cost is very small
// returns true if the motor is still running to the target position.
boolean AccelStepper_Mini::run()
{
    if (runSpeed())
	computeNewSpeed();
    return _speed != 0 || distanceToGo() != 0;
}

AccelStepper_Mini::AccelStepper_Mini(uint8_t pin1, uint8_t pin2, bool enable)
{
    _currentPos = 0;
    _targetPos = 0;
    _speed = 0;
    _maxSpeed = 1;
    _acceleration = 0;
    _sqrt_twoa = 1.0;
    _stepInterval = 0;
    _minPulseWidth = 1;
    _lastStepTime = 0;
    _pin[0] = pin1;
    _pin[1] = pin2;

    // NEW
    _n = 0;
    _c0 = 0.0;
    _cn = 0.0;
    _cmin = 1.0;
    _direction = DIRECTION_CCW;

    int i;
    for (i = 0; i < 2; i++)
	_pinInverted[i] = 0;
    // Some reasonable default
    setAcceleration(1);
}


void AccelStepper_Mini::setMaxSpeed(uint8_t speed)
{
    if (_maxSpeed != speed)
    {
	_maxSpeed = speed;
	_cmin = 1000000.0 / speed;
	// Recompute _n from current speed and adjust speed if accelerating or cruising
	if (_n > 0)
	{
	    _n = (long)((_speed * _speed) / (2 * _acceleration)); // Equation 16
	    computeNewSpeed();
	}
    }
}

float   AccelStepper_Mini::maxSpeed()
{
    return _maxSpeed;
}

void AccelStepper_Mini::setAcceleration(word acceleration)
{
    if (acceleration == 0)
	return;
    if (_acceleration != acceleration)
    {
	// Recompute _n per Equation 17
	_n = _n * (_acceleration / acceleration);
	// New c0 per Equation 7, with correction per Equation 15
	_c0 = 0.676 * sqrt(2.0 / acceleration) * 1000000.0; // Equation 15
	_acceleration = acceleration;
	computeNewSpeed();
    }
}

void AccelStepper_Mini::setSpeed(uint8_t speed)
{
    if (speed == _speed)
        return;
    speed = constrain(speed, -_maxSpeed, _maxSpeed);
    if (speed == 0)
	_stepInterval = 0;
    else
    {
	_stepInterval = fabs(1000000.0 / speed);
	_direction = (speed > 0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    _speed = speed;
}

float AccelStepper_Mini::speed()
{
    return _speed;
}

// Subclasses can override
void AccelStepper_Mini::step(long step)
{
  step1(step);
}

// You might want to override this to implement eg serial output
// bit 0 of the mask corresponds to _pin[0]
// bit 1 of the mask corresponds to _pin[1]
// ....
void AccelStepper_Mini::setOutputPins(uint8_t mask)
{
    uint8_t numpins = 2;
    uint8_t i;
    for (i = 0; i < numpins; i++)
	digitalWrite(_pin[i], (mask & (1 << i)) ? (HIGH ^ _pinInverted[i]) : (LOW ^ _pinInverted[i]));
}

// 1 pin step function (ie for stepper drivers)
// This is passed the current step number (0 to 7)
// Subclasses can override
void AccelStepper_Mini::step1(long step)
{
    // _pin[0] is step, _pin[1] is direction
    setOutputPins(_direction ? 0b10 : 0b00); // Set direction first else get rogue pulses
    setOutputPins(_direction ? 0b11 : 0b01); // step HIGH
    // Caution 200ns setup time
    // Delay the minimum allowed pulse width
    delayMicroseconds(_minPulseWidth);
    setOutputPins(_direction ? 0b10 : 0b00); // step LOW

}

// Prevents power consumption on the outputs
void AccelStepper_Mini::disableOutputs()
{
    setOutputPins(0); // Handles inversion automatically
}

void AccelStepper_Mini::setMinPulseWidth(unsigned int minWidth)
{
    _minPulseWidth = minWidth;
}

void AccelStepper_Mini::setPinsInverted(bool directionInvert, bool stepInvert, bool enableInvert)
{
    _pinInverted[0] = stepInvert;
    _pinInverted[1] = directionInvert;
}

// Blocks until the target position is reached and stopped
void AccelStepper_Mini::runToPosition()
{
    while (run())
	;
}

boolean AccelStepper_Mini::runSpeedToPosition()
{
    if (_targetPos == _currentPos)
	return false;
    if (_targetPos >_currentPos)
	_direction = DIRECTION_CW;
    else
	_direction = DIRECTION_CCW;
    return runSpeed();
}

// Blocks until the new target position is reached
void AccelStepper_Mini::runToNewPosition(long position)
{
    moveTo(position);
    runToPosition();
}

void AccelStepper_Mini::stop()
{
    if (_speed != 0)
    {
	long stepsToStop = (long)((_speed * _speed) / (2 * _acceleration)) + 1; // Equation 16 (+integer rounding)
	if (_speed > 0)
	    move(stepsToStop);
	else
	    move(-stepsToStop);
    }
}

bool AccelStepper_Mini::isRunning()
{
    return !(_speed == 0 && _targetPos == _currentPos);
}
