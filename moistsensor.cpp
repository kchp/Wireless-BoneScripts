/** Simple Moist Sensor Reading Application
* With inspiration from Derek Molloy's LDR Reading Application from the book* "Explo
ring BeagleBone: Tools and
* Techniques for Building with Embedded Linux" by John Wiley & Sons, 2014
* ISBN 9781118935125. Please see the file README.md in the repository root
* directory for copyright and GNU GPLv3 license information.
* This application has been amended by Jesper Hjort        	*/

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;

#define MoistSensor_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

int readAnalog(int number){
   stringstream ss;
   ss << MoistSensor_PATH << number << "_raw";
   fstream fs;
   fs.open(ss.str().c_str(), fstream::in);
   fs >> number;
   fs.close();
   return number;
}

int main(int argc, char* argv[]){
   cout << "\nStarting the readMoistSensor program" << endl;
   int value = readAnalog(0);
   cout << "The MoistSensor value was " << value << " out of 4095." << endl;
   if (value < 3000) {
   cout << "Well done! Your plants are happy!\n" << endl;
}  else {
   cout << "It's time to water your plants!\n" << endl;
}
   return 0;
}
root@beaglebone:~/myBBBfiles#

