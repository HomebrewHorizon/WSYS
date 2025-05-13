#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

// ANSI Escape Codes for Colors
#define GREEN   "\033[32m"
#define PURPLE  "\033[35m"
#define RED     "\033[31m"
#define RESET   "\033[0m"

// Check execution permission for TOOLPTFIX.exe
bool checkExecPerm() {
    std::string toolPath = "./TOOLPTFIX.exe";
    
    std::cout << "Checking TOOLPTFIX.exe for ExecPerm...\n";
    
    if (!fs::exists(toolPath)) {
        std::cout << "Check finished with a status of: Unknown Edition\n";
        return false;
    }

    std::ifstream file(toolPath);
    if (file.good()) {
        std::cout << GREEN << "Check finished with a status of: Enabled" << RESET << "\n";
        return true;
    } else {
        std::cout << RED << "Check finished with a status of: Disabled" << RESET << "\n";
        return false;
    }
}

// Runs TOOLPTFIX.exe
void runToolPTFIX() {
    std::string toolPath = "./TOOLPTFIX.exe";

    if (fs::exists(toolPath)) {
        std::cout << GREEN << "Starting TOOLPTFIX..." << RESET << "\n";
        int ret = system(toolPath.c_str());
        if (ret != 0) {
            std::cout << RED << "Error: TOOLPTFIX exited with code " << ret << RESET << "\n";
        }
    } else {
        std::cout << RED << "Error: TOOLPTFIX.exe not found." << RESET << "\n";
    }
}

// Process built-in WSYS commands.
bool processBuiltInCommand(const std::string& command, const std::string& username,
                           const std::string& hostname, const std::string& edition) {
    if (command == "help") {
        std::cout << "Available commands:\n"
                  << "  help               - Show this help message\n"
                  << "  clear              - Clear the screen\n"
                  << "  date               - Show current date and time\n"
                  << "  version            - Show WSYS Terminal version\n"
                  << "  echo <text>        - Echo the input text\n"
                  << "  whoami             - Show the username and hostname\n"
                  << "  ls                 - List files in the current directory\n"
                  << "  pwd                - Print current working directory\n"
                  << "  cd <dir>           - Change current directory\n"
                  << "  calc <a> <op> <b>  - Calculate simple arithmetic\n"
                  << "  about              - About WSYS Terminal\n"
                  << "  run TOOLPTFIX      - Run TOOLPTFIX.exe\n"
                  << "  chk TOOLPTFIX.exe for ExecPerm - Check TOOLPTFIX execution status\n"
                  << "  exit               - Exit the terminal\n";
        return true;
    } else if (command == "clear") {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        return true;
    } else if (command == "date") {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::cout << std::ctime(&now_time);
        return true;
    } else if (command == "version") {
        std::cout << "WSYS Terminal version 1.0. Edition: " << edition << "\n";
        return true;
    } else if (command.rfind("echo ", 0) == 0) {
        std::string text = command.substr(5);
        std::cout << text << "\n";
        return true;
    } else if (command == "whoami") {
        std::cout << GREEN << username << "@" << hostname << RESET << "\n";
        return true;
    } else if (command == "ls") {
        try {
            for (const auto &entry : fs::directory_iterator(fs::current_path())) {
                std::cout << entry.path().filename().string() << "\n";
            }
        } catch (const std::exception &e) {
            std::cout << "Error listing directory: " << e.what() << "\n";
        }
        return true;
    } else if (command == "pwd") {
        std::cout << fs::current_path() << "\n";
        return true;
    } else if (command.rfind("cd ", 0) == 0) {
        std::string newPath = command.substr(3);
        try {
            fs::current_path(newPath);
            std::cout << "Changed directory to " << fs::current_path() << "\n";
        } catch (const std::exception &e) {
            std::cout << "cd: " << e.what() << "\n";
        }
        return true;
    } else if (command.rfind("calc ", 0) == 0) {
        std::istringstream iss(command.substr(5));
        double a, b;
        char op;
        if (!(iss >> a >> op >> b)) {
            std::cout << "Usage: calc <number> <operator> <number>\n";
        } else {
            switch (op) {
                case '+': std::cout << (a + b) << "\n"; break;
                case '-': std::cout << (a - b) << "\n"; break;
                case '*': std::cout << (a * b) << "\n"; break;
                case '/': 
                    if (b == 0) {
                        std::cout << "Error: Division by zero\n";
                        return true;
                    }
                    std::cout << (a / b) << "\n"; 
                    break;
                default:
                    std::cout << "Unsupported operator: " << op << "\n";
            }
        }
        return true;
    } else if (command == "about") {
        std::cout << "WSYS Terminal is a custom terminal emulator built in C++.\n"
                  << "It leverages C++17 filesystem features and supports several built-in\n"
                  << "commands along with executing system commands.\n";
        return true;
    } else if (command == "chk TOOLPTFIX.exe for ExecPerm") {
        checkExecPerm();
        return true;
    } else if (command == "run TOOLPTFIX") {
        runToolPTFIX();
        return true;
    }
    
    return false;
}

int main(int argc, char* argv[]) {
    std::string executableName = (argc > 0) ? fs::path(argv[0]).filename().string() : "UNKNOWN";
    size_t dotPos = executableName.find_last_of('.');
    std::string edition = (dotPos == std::string::npos) ? executableName : executableName.substr(0, dotPos);

    std::string username = std::getenv("USERNAME") ? std::getenv("USERNAME") : "user";
    std::string hostname = std::getenv("COMPUTERNAME") ? std::getenv("COMPUTERNAME") : "pc";

    std::cout << GREEN << username << "@" << hostname << RESET 
              << " ~ " << PURPLE << edition << RESET << "\n";

    std::string command;
    while (true) {
        std::cout << "$ = ";
        std::getline(std::cin, command);

        if (command == "exit") {
            std::cout << "Exiting WSYS Terminal...\n";
            break;
        }

        if (processBuiltInCommand(command, username, hostname, edition))
            continue;

        int ret = system(command.c_str());
        if (ret != 0) {
            std::cout << RED << "Command exited with code " << ret << RESET << "\n";
        }
    }
    
    return 0;
}
