#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <WinSock2.h>
#include <windows.h>
#include <time.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <myo\myo.hpp>
// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 2390

#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
void wait(unsigned timeout)
{
  timeout += std::clock(); 
  while(std::clock() < timeout) continue;
}


int communicate(char in[], char in2[] = ""){
	char three[4];
	memcpy(three, in, 1);
	memcpy(&three[1], in2, 1);
#define APP_PORT 2390 // We define a port that we are going to use.

	// Here is a structure contains the port we'll use,
	// the protocol type and the IP address we'll communicate with.
	SOCKADDR_IN sockaddr;

	// This is our socket, it is the handle to the IO address to read/write packets
	SOCKET sock;

	WSADATA data;

	// First we see if there is a winsock ver 2.2 installed on the computer,
	//	we initizalize the sockets DLL for out app.
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return(0);

	// Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (!sock)
	{
		// Creation failed!
	}
	sockaddr.sin_family = AF_INET; 
	sockaddr.sin_addr.s_addr = INADDR_ANY; 
	sockaddr.sin_addr.s_addr = inet_addr("192.168.0.102"); // IP to communicate with.

	sockaddr.sin_port = htons(APP_PORT);
	int ret = bind(sock, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR));
	if (ret)
	{
		// Bind failed!
	}

	char buffer[10];
	strcpy(buffer, three);
	int len = sizeof(SOCKADDR);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR));
	// Easy huh?? Let's receive a packet..
	WSACleanup();
	return 0;

}

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DataCollector : public myo::DeviceListener {
public:
	DataCollector()
		: onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
	{
	}

	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		roll_w = 0;
		pitch_w = 0;
		yaw_w = 0;
		onArm = false;
		isUnlocked = false;
	}

	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;

		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

		// Convert the floating point angles in radians to a scale from 0 to 18.
		roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 255);
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 255);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 255);
	}

	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;

		if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
			// Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
			// Myo becoming locked.
			myo->unlock(myo::Myo::unlockHold);

			// Notify the Myo that the pose has resulted in an action, in this case changing
			// the text on the screen. The Myo will vibrate.
			myo->notifyUserAction();
		}
		else {
			// Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
			// are being performed, but lock after inactivity.
			myo->unlock(myo::Myo::unlockTimed);
		}
	}

	//PoseControl() will be called to control the lights using speciffic poses
	int poseControl(myo::Myo* myo, myo::Pose pose, int pos)
	{

		char* colors[7] = { "r", "s", "t", "u", "v", "w", "y" };


		bool lock = false;
		int tme = 0;
		//Colors::Colors c;
		currentPose = pose;
		bool lightOff = false;
		//lightOff = true;

		//lights toggle
		if (isUnlocked == false && pose == myo::Pose::fist && lightOff == true)
		{

			std::cout << "toggle on" << std::endl;
		}
		else if (isUnlocked == true && pose == myo::Pose::fist && lightOff == false)
		{

			communicate("q");
			std::cout << "toggle off" << std::endl;
			isUnlocked = false;
			lightOff = true;
			Sleep(5000);
		}

		//light intensity
		if (lightOff == false && 0 <= pitch_w && pitch_w <= 170   )
		{

			char w = pitch_w / 2;
			char z = (255-roll_w)/4;
			char array2[] = { w };
			char array3[] = { z };

			//roll from 90 to 150
			//roll from 170 to 110
			if (roll_w >=70 && roll_w < 110){
				communicate("A", array3);
			}
			else if (roll_w > 110 && roll_w <= 130){
				communicate("C", array3);
			}
			else if (roll_w > 150){
				communicate("B", array3);
			}
			if (roll_w < 65 && pitch_w < 110 ){
				communicate("x", array2);
			}
			std::cout << "Light intensity" << std::endl;
		}
		
		//light hand switch color
		if (isUnlocked == true && lightOff == false && pose == myo::Pose::waveOut)
		{
			pos++;

			if (pos == 7){ pos = 0; }

			lock = true;
			std::cout << "Color Right" <<colors[pos]<<pos<< std::endl;
			communicate(colors[pos]);
			Sleep(1000);
		

		}
		else if (isUnlocked == true && lightOff == false && pose == myo::Pose::waveIn)
		{
			if (pos == 0){ pos = 6; }
			pos--;

			std::cout << "Color Left" <<colors[pos]<<pos<< std::endl;
			communicate(colors[pos]);
			lock = false;
			Sleep(1000);
		}
		
		//light flash
		if (lightOff == false &&  200 <= pitch_w && pitch_w <= 255)
		{
			//myo->lightFlash();
			pos++;

			//pos %= 7;
			if (pos == 7){ pos = 1; }
			//myo->switchColor("right");
			//communicate("a");
			lock = true;
			//std::cout << "Color Right" << colors[pos] << pos << std::endl;
			communicate(colors[pos]);
			//Sleep(1000);
			//pos = ((pos + 1) % 7)+ 1;
			//communicate("o");
			std::cout << "Flash" << std::endl;
		}
		
		return pos;
	}

	// onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
	// arm. This lets Myo know which arm it's on and which way it's facing.
	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
	{
		onArm = true;
		whichArm = arm;
	}

	// onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
	// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
	// when Myo is moved around on the arm.
	void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
	}

	// onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
	void onUnlock(myo::Myo* myo, uint64_t timestamp)
	{
		isUnlocked = true;
	}

	// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
	void onLock(myo::Myo* myo, uint64_t timestamp)
	{
		isUnlocked = false;
	}

	// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
	// For this example, the functions overridden above are sufficient.

	// We define this function to print the current values that were updated by the on...() functions above.
	void print()
	{
		// Clear the current line
		std::cout << '\r';

		// Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
		//std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
		//<< '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
		//<< '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';
		std::cout << "Roll " << roll_w << " pitch " << pitch_w << " yaw " << yaw_w;

		if (onArm) {
			// Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.

			// Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
			// output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
			// that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
			std::string poseString = currentPose.toString();

			std::cout << '[' << (isUnlocked ? "unlocked" : "locked  ") << ']'
				<< '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
				<< '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
		}
		else {
			// Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
			std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
		}

		std::cout << std::flush;
	}


	

	// These values are set by onArmSync() and onArmUnsync() above.
	bool onArm;
	myo::Arm whichArm;

	// This is set by onUnlocked() and onLocked() above.
	bool isUnlocked;

	// These values are set by onOrientationData() and onPose() above.
	int roll_w, pitch_w, yaw_w;
	myo::Pose currentPose;
};



int main(int argc, char** argv)
{
	unsigned int pos = 3;
	try
	{
		
		std::cout << "Attempting to find a Myo..." << std::endl << std::endl;

		myo::Hub hub("com.example.hello-myo");

		myo::Myo* myo = hub.waitForMyo(10000);

		if (!myo)
		{
			throw std::runtime_error("Unable to find a Myo!");
		}

		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

		DataCollector collector;



		//myo::Pose pose = collector.currentPose;

		hub.addListener(&collector);

		while (1)
		{
			
			hub.run(1000 / 20);
			myo::Pose pose = collector.currentPose;
			collector.print();
			pos=collector.poseControl(myo, pose,pos);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}