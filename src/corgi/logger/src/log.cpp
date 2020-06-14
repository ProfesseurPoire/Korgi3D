
#include <corgi/logger/log.h>

#include <algorithm>
#include <vector>

#include <iostream>
#include <fstream>

#include <filesystem>

#include <sstream>

#include <map>
#include <string>

// Only here so I can have colors on windows
#ifdef _WIN32
#include <windows.h>   
#endif // _WIN32
#include <ctime>


#ifdef _WIN32
// Yoloed that, probably won't work on linux
// Helps with the stack trace
#include <DbgHelp.h>
#pragma comment( lib, "dbghelp.lib" )
#endif


static void set_console_color(int color)
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#endif
}

using namespace corgi;
using namespace std;

struct Channel
{
    std::vector<std::string> messages;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

static std::map<std::string, std::ofstream> files_;
static std::map<std::string, Channel>		channels_;

// Set that to false if you don't want the log operations to write
// inside a file
static bool write_in_file_ = true;

static void write_string(const std::string& str, const std::string& channel, int t)
{
	std::time_t time	= std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto gmtime = std::gmtime(&time);
	

    std::string minutes = std::to_string(gmtime->tm_min);
    if(gmtime->tm_min<10)
    {
        minutes = "0"+minutes;
    }

    std::string seconds =  std::to_string(gmtime->tm_sec);
    if(gmtime->tm_sec<10)
    {
        seconds = "0"+seconds;
    }

	std::string time_str = std::to_string(gmtime->tm_hour) + ":" +minutes+ ":" +seconds ;
    std::string new_str =  "["+ time_str +"] : " + str;
	
    std::cout << new_str << std::flush;

    if (channels_.count(channel)>0)
    {
        channels_.emplace(channel, Channel());
    }
    
    switch (t)
    {
        case 0:
            channels_[channel].messages.push_back(new_str);
            break;
        case 1:
            channels_[channel].warnings.push_back(new_str);
            break;
        case 2:
            channels_[channel].errors.push_back(new_str);
            break;
    }

    if (write_in_file_)
    {
        if (!files_[channel].is_open())
        {
            //Creates the directory to store the logs if it doesn't exist already
            std::filesystem::create_directory("logs");

            // Opening/closing to erase the content of the file, a bit weird 
            // but if I open the file with trunc it won't write on it
            files_[channel].open(("logs/" + channel + ".log"), std::ofstream::out | std::ofstream::trunc);
            files_[channel].close();
            files_[channel].open(("logs/" + channel + ".log"), std::ofstream::out | std::ofstream::app );
        }
        if (files_[channel].is_open())
            files_[channel] << new_str;
    }
}

static std::string filename(const std::string& path)
{
    for (size_t i = path.size() - 1; i > 0; --i)
        if (path[i] == '/' || path[i] == '\\')
            return path.substr(i + 1, std::string::npos);	//npos means until the end of the string
    return std::string();
}

// Here the static only mean that the function won't be defined in other translation unit
static std::string build_string(const std::string& t, int line, const std::string& file, const std::string& func, const std::string&text, const std::string channel)
{
    return  t + " : {" +channel + "} : \"" + text +"\" at (" + filename(file) + "::" + func + " " + std::to_string(line) +  ") \n";
}

void Logger::message(const std::string& text, int line, const std::string& file, const std::string& func, const std::string& channel)
{
    set_console_color(11);  // Blue
    write_string(build_string("Message", line, file, func, text.c_str(), channel), channel, 0);
}

void Logger::warning(const std::string& text, int line, const std::string& file, const std::string& func, const std::string& channel)
{
    set_console_color(14);  // Yellow
    write_string(build_string("Warning", line, file, func, text, channel), channel, 1);
}

void Logger::error(const std::string& text, int l, const std::string& file, const std::string& func, const std::string& channel)
{
    set_console_color(12); // Red
    write_string(build_string("Error  ", l, file, func, text, channel), channel, 2);

#ifdef _WIN32

	void* stack[200];
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	WORD numberOfFrames = CaptureStackBackTrace(0, 200, stack, NULL);
	SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + (200 - 1) * sizeof(TCHAR));
	symbol->MaxNameLen = 200;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	DWORD displacement;
	IMAGEHLP_LINE64* line = (IMAGEHLP_LINE64*)malloc(sizeof(IMAGEHLP_LINE64));
	line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	for (int i = 0; i < numberOfFrames; i++)
	{
		DWORD64 address = (DWORD64)(stack[i]);
		SymFromAddr(process, address, NULL, symbol);
		if (SymGetLineFromAddr64(process, address, &displacement, line))
		{
			printf("\tat %s in %s: line: %lu: address: 0x%0X\n", symbol->Name, line->FileName, line->LineNumber, symbol->Address);
		}
		else
		{
			/*printf("\tSymGetLineFromAddr64 returned error code %lu.\n", GetLastError());
			printf("\tat %s, address 0x%0X.\n", symbol->Name, symbol->Address);*/
		}
	}

#endif
	//return 0;

}