// (c) 2013-2020, David Gerstl, all rights reserved
// For the use of my C++ students to use as a base to implement
// various programs needed for their data structures class.

// Class farmingdale::linkedList: General string linked List implementation
// This is a very simple test driver that prints a message and runs a set of test functions 
// contained in another file. 
// test methods return 0 on success. Otherwise return the line number 
// on which the error occurred (For ease in finding it. Note that the 
// preprocessor directive __LINE__ will give you this line.

// very minimal set of tests. 
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>    // For MD5 methods
#include <Wincrypt.h>   // For MD5 methods
#include <cstdlib>      // For MD5 methods
#include <sstream>    // for stringStream
#include <iomanip>   // for setw etc.
#include "farmingdaleDynamicQueueTest.h"


// typedef testFuncPtr as a pointer to a no-param function returning int
typedef int (*testFuncPtr)();
// We could use a map, but this allows us to set the order
typedef struct  {
	std::string testName;
	testFuncPtr testFunction;
} testContainer;


std::string returnMD5String(std::string);
int main() { 
    // make a vector with our tests and their names. 
    std::vector<testContainer> testFunctions =
    {
        // Students are free to write their own test functions and call them from "studentTest()", but 
        // cannot change this file
        { "studentTest", studentTest }
        , {"normalTest1", normalTest1 }
        , { "normalTest2" , normalTest2}
        , { "copyCtorTest" , copyCtorTest}
    };
	bool failed = false;
	// Depending on your #defines, this method will print different messages
	printTestMessages(std::cerr);
    std::cout << "\n\n";
    std::cout << "The following file must be unchanged from the file I supplied:\n\t" << 
        returnMD5String("farmingdaleTestDriver.cpp");
    std::cout << "The following file must be unchanged from the file I supplied:\n\t" << 
        returnMD5String("farmingdaleDynamicQueueTest.cpp");
    std::cout << "The following file must be unchanged from the file I supplied:\n\t" << 
        returnMD5String("farmingdaleDynamicQueueTest.h");
    std::cout << "The following file must be unchanged from the file I supplied:\n\t" << 
        returnMD5String("farmingdaleStatus.h");
    std::cout << "The following file may be changed:\n\t" << 
        returnMD5String("farmingdaleDynamicContiguousMemoryQueue.cpp");
    std::cout << "The following file may be changed:\n\t" << 
        returnMD5String("farmingdaleDynamicContiguousMemoryQueue.h");
    std::cout << "\n";
	// loop through the list of tests 
	// You will understand this after the STL Module 
	for (std::vector<testContainer>::iterator mIter = testFunctions.begin();
		mIter != testFunctions.end();
		++mIter) {
		std::cout << "\nStarting test " << mIter->testName << std::endl;
		// call the appropriate test function
		int thisFail = (mIter->testFunction)();
		if (0 != thisFail) {
			failed = true;
			std::cerr << "\tFailed " << mIter->testName << " on line " << thisFail << std::endl;
		} else {
			std::cerr << "\tPassed " << mIter->testName << std::endl;
		}
	} // for mIter = 
	if (failed) {
		std::cerr << "You have failed at least one test. You have more work to do." << std::endl;
	} else {
		std::cerr << "Good job. You've passed the tests." << std::endl;
	}
	system("pause");
	return 0;
}


// Hashing code is based on https://docs.microsoft.com/en-us/windows/win32/seccrypto/example-c-program--creating-an-md-5-hash-from-file-content

// Convert std::string to wide string
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
// This hashes the test file to make sure it hasn't been changed 
// (I have a list of the correct hashes). 
std::string returnMD5String(std::string testFileName) {
    const int BUFSIZE = 1024;
    const int MD5LEN = 16;
    std::stringstream sStream;
    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";
    std::wstring stringTemp = s2ws(testFileName);
    LPCWSTR filename = stringTemp.c_str();
    // Logic to check usage goes here.

    hFile = CreateFile(filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        dwStatus = GetLastError();
        sStream << "Error opening file " << testFileName
            << "\nError: " << dwStatus << std::endl;
        return sStream.str();
    }

    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT))
    {
        dwStatus = GetLastError();
        sStream << "CryptAcquireContext failed: " << 
            dwStatus << std::endl;
        CloseHandle(hFile);
        return sStream.str();
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
       sStream << "CryptAcquireContext failed: " << 
            dwStatus << std::endl;
        CryptReleaseContext(hProv, 0);
        return sStream.str();
    }

    while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, 
        &cbRead, NULL))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            dwStatus = GetLastError();        
            sStream << "CryptHashData failed: " << 
                dwStatus << std::endl;
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return sStream.str();
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
        sStream << "ReadFile failed: " << 
            dwStatus << std::endl;
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
        return sStream.str();
    }

    cbHash = MD5LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        sStream << "MD5 hash of file " << testFileName <<
            " is ";
        for (DWORD i = 0; i < cbHash; i++)
        {
            sStream << rgbDigits[rgbHash[i] >> 4];
            sStream << rgbDigits[rgbHash[i] & 0xf];
          //  std::cout << std::string(buf);        
        }
        sStream << std::endl;
    }
    else
    {
        dwStatus = GetLastError();
        sStream << "CryptGetHashParam failed: " << 
            dwStatus << std::endl;
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return sStream.str();
 }