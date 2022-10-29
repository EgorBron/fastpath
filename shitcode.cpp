#include <iostream> // stdout/stdin
#include <fstream> // filesys io
#include <algorithm> // foreach
#include <string> // stdstring
#include <vector> // stdvector
#include <map> // stdmap
#include <regex> // regexpr
#include <processenv.h> // winapi sysenv
#include "nlohmann/json.hpp" // json

using namespace nlohmann;
typedef std::string str; // ima pythoner

void Inject(string & dllPath, string & processName) {
    DWORD processId = GetProcessIdentificator(processName);
    if (processId == NULL) throw invalid_argument("Process dont existed");
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processId);
    HMODULE hModule = GetModuleHandle("kernel32.dll");
    FARPROC address = GetProcAddress(hModule, "LoadLibraryA");
    int payloadSize = sizeof(char) * dllPath.length() + 1;
    LPVOID allocAddress = VirtualAllocEx(hProcess, NULL, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    SIZE_T written;
    bool writeResult = WriteProcessMemory(hProcess, allocAddress, dllPath.c_str(), payloadSize, & written);
    DWORD treadId;
    CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) address, allocAddress, 0, & treadId);
    CloseHandle(hProcess);
}

std::vector<str> strsplit(str inpstr, char seperator) {  
    int currIndex = 0, i = 0;
    int startIndex = 0, endIndex = 0;
    std::vector<str> strings;
    while (i <= inpstr.length()) {  
        if (inpstr[i] == seperator || i == inpstr.length()) {  
            endIndex = i;  
            str subStr = "";  
            subStr.append(inpstr, startIndex, endIndex - startIndex);  
            strings[currIndex] = subStr;  
            currIndex += 1;  
            startIndex = endIndex + 1;  
        };
        i++;
    }
    return strings;
};

std::map<std::string, std::string> parseArgv(char* argv[]) {
    str args = str(*argv);
    std::vector<str> toparse = strsplit(args, '-');
    str last = "";
    std::map<std::string, std::string> finite;
    std::for_each(toparse.begin(), toparse.end(), [last, finite](str iter) mutable->void{
        if (iter._Starts_with("-")) {
            last = (str)iter.erase(0, 1);
            finite.insert(last, (str)"");
        } else {
            try {
                auto founded = finite.find(iter);
                finite.insert(last, finite[(int)founded]+" "+iter);
            } catch (std::exception) {
                finite.insert(last, iter);
            }
        };
    });
};

str regexReplacesVariables (str inp){
    auto matcher = std::regex("%[\.\w\d-]+%");
    std::smatch matches;
    std::regex_search(inp, matches, matcher);
    std::vector<str> itermatch{std::sregex_token_iterator{cbegin(inp), cend(inp), matcher, 1}, std::sregex_token_iterator{}};
    std::for_each(itermatch.begin(), itermatch.end(), [inp](str &match) mutable -> void {
        std::cout<<match<<std::endl;
        // inp.replace("", "");
    });
    DWORD bufCharCount = 32767;
    TCHAR  infoBuf[sizeof(bufCharCount)];
    ExpandEnvironmentStrings(TEXT("%USERPROFILE%\\1.txt"), infoBuf, bufCharCount);
    return "";
};

class FastpathDir {
    str path;
    json config;
    public:
    FastpathDir(str path) {
        (*this).path = path;
        std::fstream fio;
        fio.open(path+"\\CONFIG.fph");
        if (fio.is_open()) {
            str readed;
            fio>>readed;
            config = json::parse(readed);
            
            fio.close();
        };
    }
};

int main(int argc, char *argv[]) {
    // FastpathDir *fpdir = new FastpathDir("C:\\fastpath");
    std::cout<<regexReplacesVariables("C:\\%username%\\tst")<<"\n";
    return 0;
}