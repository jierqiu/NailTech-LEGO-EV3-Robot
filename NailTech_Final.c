/*
Group 5: Jier Qiu, Siddhi Patel, Emilia Keddie, Caitlyn Staresinic
This code is to be run on a LEGO EV3 Robot using RobotC for LEGO Mindstorms. 
Please refer to diagram of robot layout for a better understanding of motor directions

Note about motor directions:
For motorA: 
+ve = down
-ve = up

For motorB: 
-ve = towards nail
+ve = back towards origin

*/

const float maxYDist = 9.5; //distance to go all the way down to the brush
const float radGear = 1.05; //radius of the gear
const float distYNail = 3; //distance to go down to the nail
const float lengthNail = 2; //length of painting stroke
const float DIST2NAIL = -15; //horizontal distance to the nail
const int totalFinger = 5; //assumption: total of 5 fingers

//Prompts user to press button to choose color, returns color integer
int selectColor()
{
	int selectedColor = 0;

	clearTimer(T1);

	displayString(5, "Select a color:");
	displayString(8, "Press up for red");
	displayString(9, "Press left for blue");
	displayString(10, "Press right for white");

	while(!getButtonPress(buttonAny) && time1[T1] < 10000)
	{}
	if(getButtonPress(buttonUp) == 1 && time1[T1] < 10000)
		selectedColor = (int)colorRed;
	else if(getButtonPress(buttonLeft)== 1 && time1[T1] < 10000)
		selectedColor = (int)colorBlue;
	else if(getButtonPress(buttonRight) == 1 && time1[T1] < 10000)
		selectedColor = (int)colorWhite;
	else
		selectedColor = 0;
	eraseDisplay();
	return selectedColor;
}

//Runs a timer such that if button not pressed within 5 seconds, program will close
void timeCheck(int & color, bool & timeOut, int & finger)
{
	do
	{
		color = selectColor();
		if(color == 0)
		{
			displayString(5, "Are you still there?");
			displayString(7, "Press down to proceed");
			clearTimer(T2);
			while(!getButtonPress(buttonDown) && time1(T2) < 5000)
			{}
			while(getButtonPress(buttonDown) && time1(T2) < 5000)
			{}
			if(time1(T2) >= 5000)
				timeOut = true;
			eraseDisplay();
		}

	}while(timeOut == false && color == 0);
}

//Move a distance in y direction
void moveY(float dist_cm)
{
	nMotorEncoder[motorA] = 0;
	if (dist_cm <0)
		motor[motorA] = -10; //moves up
	else
		motor[motorA] = 10; //moves down

	int enc_limit = dist_cm*180/(PI*radGear);
	while (abs(nMotorEncoder[motorA]) < abs(enc_limit) && (SensorValue[S1] == 1))
	{}
	motor[motorA] = 0;
}

// Uses color sensor to choose correct brush
void pickBrush(int & selectedColor, int & xPosition)
{
	motor[motorB] = -15; // moves towards nail

	while(SensorValue[S3] != selectedColor && SensorValue[S1] == 1)
	{}
	motor[motorB] = 0;
	xPosition = nMotorEncoder[motorB];

	moveY(maxYDist); // moves down
	wait1Msec(1000);
	moveY(-maxYDist); // moves up
	wait1Msec(1000);
}

// Moves brush towards nail
void reachNail()
{
	motor[motorB] = -10;
	while (nMotorEncoder[motorB] > DIST2NAIL * (180)/(PI*radGear)&& SensorValue[S1] == 1)
	{}
	motor[motorB] = 0;
}

// Moves brush down and back to paint nail, plays sound when complete
void paintNail()
{
		moveY(distYNail);

		int xInitial = nMotorEncoder[motorB];
		int enc_diff = lengthNail*180/(PI*radGear);
		int xFinal = xInitial + enc_diff;

		wait1Msec(1000);

		motor[motorB] = 10;
		while(nMotorEncoder[motorB] < xFinal && SensorValue[S1] == 1)
		{}

		motor[motorB] = 0;

		if(SensorValue[S1] == 1)
		{
			wait1Msec(500);
			playSoundFile("Magic wand.rsf");
			wait1Msec(500);
		}
}

// Moves brush to starting y position and x position to return the brush to its slot in the brush holder
void returnBrush(int & xPosition) //uses x position from pickBrush function
{
	moveY(-distYNail);
	motor[motorB] = 10; //go back towards origin
	while(nMotorEncoder[motorB] < xPosition && SensorValue[S1] == 1 )
	{}
	motor[motorB] = 0;
}

// Brings brush down, disconnects brush, moves back to starting y position
void moveGripper()
{
	if(SensorValue[S1] == 1)
	{
		int y_limit = maxYDist *180/(PI*radGear);

		nMotorEncoder[motorA] = 0;
		motor[motorA] = 10;
		while(nMotorEncoder[motorA] < y_limit)
		{}
		motor[motorA] = 0;

		nMotorEncoder[motorD] = 0;
		motor[motorD] = 20;

		wait1Msec(2000);

		motor[motorA] = -10;
		while(nMotorEncoder[motorA] > 0)
		{}
		motor[motorA] = 0;
		motor[motorD] = 0;

		motor[motorD] = -20;
		while(nMotorEncoder[motorD] > 0)
		{}
		motor[motorD] = 0;
	}
}


task main()
{
	//Configure Sensors and respective modes
	SensorType[S1] = sensorEV3_Touch;
	wait1Msec(100);
	SensorType[S3] = sensorEV3_Color;
	wait1Msec(100);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(100);

	nMotorEncoder[motorB] = 0;
	bool timeOut = false;
	bool touch = true;
	int selectedColor = 0;
	int xPosition = 0;

	displayBigTextLine(5, "Welcome to");
	displayBigTextLine(8, "Nailtech!");
	wait1Msec(1500);
	eraseDisplay();
	displayBigTextLine(5, "Press Enter");
	displayBigTextLine(8, "to Begin");

	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}
	eraseDisplay();

	nMotorEncoder[motorA] = 0;

	for(int finger = 1; finger <= totalFinger && !timeOut; finger++)
	{
		timeCheck(selectedColor, timeOut, finger);

		if(!timeOut)
		{
			displayBigTextLine(5, "Place finger");
			displayBigTextLine(8, "to start");

			while(SensorValue[S1] == 0)
			{}
			eraseDisplay();

			displayBigTextLine(5, "Keep finger");
			displayBigTextLine(8, "in place");

			pickBrush(selectedColor, xPosition);
			reachNail();
			paintNail();
			returnBrush(xPosition);
			moveGripper();

			wait1Msec(1000);

			motor[motorB] = 10;
			while(nMotorEncoder[motorB] < 0)
			{}
			motor[motorB] = 0;

			eraseDisplay();

			if(SensorValue[S1] == 0)
			{
				touch = false;
				finger+=6;
			}

			if(finger < totalFinger && touch == 1)
			{
				displayBigTextLine(5, "Finger Complete");
				displayBigTextLine(8, "Switch finger");
				wait1Msec(2000);
				eraseDisplay();
			}
		}
	}

	if(!timeOut && touch == 1)
	{
		displayBigTextLine(5,"Manicure");
		displayBigTextLine(8, "Complete!");
		wait1Msec(2000);
		eraseDisplay();
		wait1Msec(500);
	}
	else
	{
		displayBigTextLine(5, "Error");
		displayBigTextLine(8, "Try again");
		wait1Msec(2000);
		eraseDisplay();
		wait1Msec(500);
	}

	wait1Msec(1000);
}
