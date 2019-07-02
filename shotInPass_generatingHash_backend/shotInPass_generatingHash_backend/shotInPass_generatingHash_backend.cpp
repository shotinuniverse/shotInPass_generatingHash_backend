#include <iostream>
#include <thread>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <fstream>
#ifdef WIN32
#include <windows.h>
#else
//#include <unistd.h>
#endif

////////////////////////////////////////

#include "sha1.h"
#include "base64.h"

////////////////////////////////////////

#define BEGIN_CODE_IN_ASCII 48

////////////////////////////////////////

using namespace std;

int numberOfSymbolsInASCII{ 62 };
int maxNumberOfSymbolsInGeneratePassword;
int beginNumberOfSymbolsInGeneratePassword;
int X[100];
int counterStrings;
int iteratorMoveInASCII{0};
int counterSizeOfGeneratePasswords{ 0 };
int counterStringsInFileForSizeTransaction{ 0 };
int maxNumberOfElementsInOneTransaction{ 10000 };
int timeWaitThread{ 1000 };
bool generation_is_done{ false };
int finalNumberOfPasswords{ 0 };

struct PasswordAndHash
{
	string password;
	string hash;
};

vector<PasswordAndHash> listOfPasswordAndHash;

class GenerationPassword {
public:
	void operator()(int params) {
		for (beginNumberOfSymbolsInGeneratePassword;
			beginNumberOfSymbolsInGeneratePassword < maxNumberOfSymbolsInGeneratePassword;
			beginNumberOfSymbolsInGeneratePassword++) {
			generate(params);
		}
		generation_is_done = true;
		finalNumberOfPasswords = counterSizeOfGeneratePasswords;
	}

	void generate(int k) {
		if (counterSizeOfGeneratePasswords < maxNumberOfElementsInOneTransaction) {
			if (k == beginNumberOfSymbolsInGeneratePassword) {
				
				cout << to_string(counterStrings) + " : ";
				string tempString = "";
				for (int i = 0; i < beginNumberOfSymbolsInGeneratePassword; i++) {
					tempString += char(X[i]);
				}
				
				string passwordInSHA1 = sha1(tempString);
				//string passwordInBase64 = base64_encode(reinterpret_cast<const unsigned char*>(passwordInSHA1.c_str()), passwordInSHA1.length());
				
				string readyPassword = passwordInSHA1;

				PasswordAndHash passwordAndHash;
				passwordAndHash.password = tempString;
				passwordAndHash.hash = passwordInSHA1;

				listOfPasswordAndHash.push_back(passwordAndHash);
				
				std::cout << tempString <<" : "<< passwordInSHA1 << "\n";
				counterStrings++;
				counterSizeOfGeneratePasswords++;
			}
			else {
				iteratorMoveInASCII = 0;
				for (int j = 0; j < numberOfSymbolsInASCII; j++)
				{
					X[k] = j + BEGIN_CODE_IN_ASCII + iteratorMoveInASCII;


					if (j == 9) 
						iteratorMoveInASCII = 7;
					
					else if (j == 35) 
						iteratorMoveInASCII = 13;
					
					else if (j == 61)
						iteratorMoveInASCII = 0;

					generate(k + 1);
				}
			}
		}
		else {
#ifdef WIN32
			Sleep(timeWaitThread);
#else
			usleep(timeWaitThread);
#endif	
		}
	}
};

class OutputFilePassword {
public:
	void operator()(int params) {
		ofstream myfile;
		myfile.open("generationPasswords.txt", ios::out | ios::app);
		int counterStringsInFile = params;
		while (true) {
			if (generation_is_done && finalNumberOfPasswords <= counterStringsInFile)
				break;

			if (counterStringsInFile > listOfPasswordAndHash.size() - 1) {
#ifdef WIN32
				Sleep(timeWaitThread);
#else
				usleep(timeWaitThread);
#endif	
			}
			if (counterStringsInFile < listOfPasswordAndHash.size()) {
				myfile << listOfPasswordAndHash[counterStringsInFile].password << ":    " 
					<< listOfPasswordAndHash[counterStringsInFile].hash << "\n";

				counterStringsInFile++;
				if (counterStringsInFile == maxNumberOfElementsInOneTransaction) {
					listOfPasswordAndHash.clear();
					counterStringsInFile = 0;
					counterSizeOfGeneratePasswords = 0;
#ifdef WIN32
					Sleep(timeWaitThread);
#else
					usleep(timeWaitThread);
#endif
				}
			}
		}
		myfile.close();
	}
};

int main() {
	cout << "Please, input begin number of symbols in generate password" << "\n"
		<< " (for example, if may begin from 'aa', then input 2): ";
	cin >> beginNumberOfSymbolsInGeneratePassword;

	cout << "Please, input begin maximum number of symbols in generate password" << "\n"
		<< " (for example, if may begin from 'zzzzz', then input 6 (0 to 5)): ";
	cin >> maxNumberOfSymbolsInGeneratePassword;

	int params = 0;

	thread generationThread(GenerationPassword(), params);
	thread outputThread(OutputFilePassword(), params);
	generationThread.join();
	outputThread.join();

	return 0;
}