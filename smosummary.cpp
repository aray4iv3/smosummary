#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

// colors!!
#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define BOLD    "\033[1m"
#define GOLD    "\033[38;2;255;215;0m"   
#define GREEN   "\033[38;2;0;255;0m"     
#define CYAN    "\033[38;2;0;255;255m"   

// string = raw seconds
double timeToSeconds(std::string s) {
    std::vector<double> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ':')) {
        try { parts.push_back(std::stod(item)); } 
        catch (...) { return -1; } // catch and return it! lol
    }
    if (parts.size() == 3) return (parts[0] * 3600) + (parts[1] * 60) + parts[2];
    if (parts.size() == 2) return (parts[0] * 60) + parts[1];
    if (parts.size() == 1) return parts[0];
    return -1;
}

// epik int main
int main(int argc, char* argv[]) {

    // what's the version???
    if (argc > 1) {
        std::string flag = argv[1];
        if (flag == "--version" || flag == "-v") {
            std::cout << GOLD << BOLD << "smosummary v1.3.0-1" << std::endl;
            std::cout << "creator and publisher: aray4iv3" << RESET << std::endl;
            return 0; // throw the program out the window
        }
    }

    // the kingdom manifests
    std::vector<std::string> anyLake = {
        "Cap", "Cascade", "Sand", "Lake", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon"
    };
    std::vector<std::string> anyWooded = {
        "Cap", "Cascade", "Sand", "Wooded", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon"
    };
    std::vector<std::string> allKingdoms = {
        "Mushroom", "Cap", "Cascade", "Sand", "Lake", "Wooded", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon", "Dark Side", "Darker Side"
    };

    std::vector<double> times;
    std::string input;
    int mode = 1;
    int routeChoice = 1;

    std::cout << GOLD << "--- smosummary: Sum of Best Calculator ---" << RESET << std::endl;
    
    // choose the path!!
    std::cout << BOLD << "Select Route:" << RESET << std::endl;
    std::cout << "1) Any% (Lake)" << std::endl;
    std::cout << "2) Any% (Wooded)" << std::endl;
    std::cout << "3) All Kingdoms" << std::endl;
    std::cout << "Selection > ";
    std::cin >> routeChoice;

    std::vector<std::string> activeKingdoms;
    if (routeChoice == 1) activeKingdoms = anyLake;
    else if (routeChoice == 2) activeKingdoms = anyWooded;
    else activeKingdoms = allKingdoms;

    // total or split? choose already!!
    std::cout << BOLD << "\nChoose Input Mode:" << RESET << std::endl;
    std::cout << "1) Split Times (Individual segments)" << std::endl;
    std::cout << "2) Cumulative Times (Run timer at each kingdom)" << std::endl;
    std::cout << "Selection > ";
    std::cin >> mode;

    std::cout << "\nEnter times (M:S). Type 'skip' for 0.\n" << std::endl;
    std::cout << CYAN << "Output: ~/Documents/best_times.txt\n" << RESET << std::endl;

    double previousCumulative = 0;

    for (const std::string& k : activeKingdoms) {
        while (true) {
            std::cout << CYAN << k << " Kingdom" << RESET << " > ";
            std::cin >> input;

            if (input == "skip") {
                times.push_back(0);
                break;
            }

            double sec = timeToSeconds(input);
            if (sec >= 0) {
                if (mode == 2) {
                    // lazy math, do it yourself!
                    double segmentTime = sec - previousCumulative;
                    times.push_back(segmentTime >= 0 ? segmentTime : 0);
                    previousCumulative = sec;
                } else {
                    times.push_back(sec);
                }
                break;
            } else {
                std::cout << RED << "Invalid format!" << RESET << " Use M:S" << std::endl;
            }
        }
    }

    // summon the answer
    double total = 0;
    for (double t : times) total += t;

    int h = (int)total / 3600;
    int m = ((int)total % 3600) / 60;
    double s = total - (h * 3600) - (m * 60);

    std::cout << "\n" << BOLD << "===============================" << RESET << std::endl;
    std::cout << GREEN << " FINAL SUM OF BEST: " << h << "h " << m << "m " << std::fixed << std::setprecision(2) << s << "s " << RESET << std::endl;
    std::cout << BOLD << "===============================" << RESET << std::endl;

    // grab the home and place it here
    const char* homeDir = std::getenv("HOME");
    std::string fullPath = (homeDir != nullptr) ? std::string(homeDir) + "/Documents/best_times.txt" : "best_times.txt";

    // stack the .txt on top of a .txt or just makes a new one if it didn't exist
    std::ofstream outFile(fullPath, std::ios::app); 
    if (outFile.is_open()) {
        std::time_t now = std::time(0);
        char* dt = std::ctime(&now);

        outFile << "\n===========================================\n";
        outFile << "SESSION: " << dt;
        outFile << "ROUTE: " << (routeChoice == 1 ? "Any% (Lake)" : (routeChoice == 2 ? "Any% (Wooded)" : "All Kingdoms")) << "\n";
        outFile << "MODE: " << (mode == 2 ? "Cumulative" : "Splits") << "\n";
        outFile << "===========================================\n";
        
        for (size_t i = 0; i < activeKingdoms.size(); ++i) {
            // grab "formatting" by the collar and throw him out the window
            outFile << std::left << std::setw(15) << activeKingdoms[i] << ": " << times[i] << "s\n";
        }
        outFile << "TOTAL SOB: " << h << "h " << m << "m " << s << "s\n";
        outFile.close();
        std::cout << "\n[" << GREEN << "SUCCESS" << RESET << "] Session saved to " << fullPath << "!" << std::endl;
    } else {
        std::cout << "\n[" << RED << "ERROR" << RESET << "] The file system rejected the write." << std::endl;
    }

    return 0;
};
