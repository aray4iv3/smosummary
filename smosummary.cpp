#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

// ANSI Color Codes
#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define BOLD    "\033[1m"
#define GOLD    "\033[38;2;255;215;0m"   
#define GREEN   "\033[38;2;0;255;0m"     
#define CYAN    "\033[38;2;0;255;255m"   

double timeToSeconds(std::string s) {
    std::vector<double> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ':')) {
        try { parts.push_back(std::stod(item)); } 
        catch (...) { return -1; }
    }
    if (parts.size() == 3) return (parts[0] * 3600) + (parts[1] * 60) + parts[2];
    if (parts.size() == 2) return (parts[0] * 60) + parts[1];
    if (parts.size() == 1) return parts[0];
    return -1;
}

int main() {
    std::vector<std::string> kingdoms = {
        "Mushroom", "Cap", "Cascade", "Sand", "Lake", "Wooded", 
        "Cloud", "Lost", "Metro", "Snow", "Seaside", 
        "Luncheon", "Ruined", "Bowser's", "Moon", "Dark Side",  "Darker Side"
    };

    std::vector<double> times;
    std::string input;

    std::cout << GOLD << "--- SMO Speedrun: Sum of Best Calculator ---" << RESET << std::endl;
    std::cout << "Enter times (M:S). Type 'skip' for 0.\n" << std::endl;
    std::cout << "The result will output into the following: ~/Documents/best_times.txt\n" << std::endl;

    for (const std::string& k : kingdoms) {
        while (true) {
            std::cout << CYAN << k << " Kingdom" << RESET << " > ";
            std::cin >> input;

            if (input == "skip") {
                times.push_back(0);
                break;
            }

            double sec = timeToSeconds(input);
            if (sec >= 0) {
                times.push_back(sec);
                break;
            } else {
                std::cout << RED << "Invalid format!" << RESET << " Use M:S" << std::endl;
            }
        }
    }

    double total = 0;
    for (double t : times) total += t;

    int h = (int)total / 3600;
    int m = ((int)total % 3600) / 60;
    double s = total - (h * 3600) - (m * 60);

    std::cout << "\n" << BOLD << "===============================" << RESET << std::endl;
    std::cout << GREEN << " FINAL SUM OF BEST: " << h << "h " << m << "m " << s << "s " << RESET << std::endl;
    std::cout << BOLD << "===============================" << RESET << std::endl;

    // --- Handling the File Path ---
    const char* homeDir = std::getenv("HOME");
    std::string fullPath;
    if (homeDir != nullptr) {
        fullPath = std::string(homeDir) + "/Documents/best_times.txt";
    } else {
        fullPath = "best_times.txt"; 
    }

    // Using std::ios::app to PREVENT OVERWRITING
    std::ofstream outFile(fullPath, std::ios::app); 

    if (outFile.is_open()) {
        // Create a Timestamp
        std::time_t now = std::time(0);
        char* dt = std::ctime(&now);

        outFile << "\n===========================================\n";
        outFile << "SESSION: " << dt;
        outFile << "===========================================\n";
        
        for (size_t i = 0; i < kingdoms.size(); ++i) {
            outFile << kingdoms[i] << ": " << times[i] << "s\n";
        }
        outFile << "TOTAL SOB: " << h << "h " << m << "m " << s << "s\n";
        outFile.close();
        
        std::cout << "\n[" << GREEN << "SUCCESS" << RESET << "] Session saved to " << fullPath << "!" << std::endl;
    } else {
        std::cout << "\n[" << RED << "ERROR" << RESET << "] Could not write to file!" << std::endl;
    }

    return 0;
}
