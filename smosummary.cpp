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
        catch (...) { return -1; } 
    }
    if (parts.size() == 3) return (parts[0] * 3600) + (parts[1] * 60) + parts[2];
    if (parts.size() == 2) return (parts[0] * 60) + parts[1];
    if (parts.size() == 1) return parts[0];
    return -1;
}

int main(int argc, char* argv[]) {

    if (argc > 1) {
        std::string flag = argv[1];
        if (flag == "--version" || flag == "-v") {
            std::cout << GOLD << BOLD << "smosummary v1.4.0-1" << std::endl;
            std::cout << "creator and publisher: aray4iv3" << RESET << std::endl;
            return 0; 
        }
    }

    // important stuff
    std::vector<std::string> anyLake = {
        "Cap", "Cascade", "Sand", "Lake", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon"
    };
    std::vector<std::string> anyWooded = {
        "Cap", "Cascade", "Sand", "Wooded", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon"
    };
    std::vector<std::string> hundredPercent = {
        "Mushroom", "Cap", "Cascade", "Sand", "Lake", "Wooded", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon", "Dark Side", "Darker Side"
    };

    std::vector<double> times;
    std::vector<std::string> activeKingdoms;
    std::string input;
    int mode = 1;
    int routeChoice = 1;

    std::cout << GOLD << "--- smosummary: Sum of Best Calculator ---" << RESET << std::endl;
    // choices
    std::cout << BOLD << "Select Route:" << RESET << std::endl;
    std::cout << "1) Any% (Lake)" << std::endl;
    std::cout << "2) Any% (Wooded)" << std::endl;
    std::cout << "3) 100% (All Kingdoms)" << std::endl;
    std::cout << "4) Custom Layout" << std::endl;
    std::cout << "Selection > ";
    std::cin >> routeChoice;
	//ifelse statement
    if (routeChoice == 1) activeKingdoms = anyLake;
    else if (routeChoice == 2) activeKingdoms = anyWooded;
    else if (routeChoice == 3) activeKingdoms = hundredPercent;
    else if (routeChoice == 4) {
        int count;
        std::cout << "How many segments? ";
        std::cin >> count;
        for(int i = 0; i < count; ++i) {
            std::string name;
            std::cout << "Name for segment " << (i+1) << ": ";
            std::cin >> name;
            activeKingdoms.push_back(name);
        }
    } else {
        activeKingdoms = anyLake;
    }
	// timer type
    std::cout << BOLD << "\nChoose Input Mode:" << RESET << std::endl;
    std::cout << "1) Split Times (Individual segments)" << std::endl;
    std::cout << "2) Cumulative Times (Run timer at each kingdom)" << std::endl;
    std::cout << "Selection > ";
    std::cin >> mode;

    std::cout << "\nEnter times (M:S). Type 'skip' for 0.\n" << std::endl;

    double previousCumulative = 0;

    for (const std::string& k : activeKingdoms) {
        while (true) {
            std::cout << CYAN << k << RESET << " > ";
            std::cin >> input;

            if (input == "skip") {
                times.push_back(0);
                break;
            }

            double sec = timeToSeconds(input);
            if (sec >= 0) {
                if (mode == 2) {
                    double segmentTime = sec - previousCumulative;
                    times.push_back(segmentTime >= 0 ? segmentTime : 0);
                    previousCumulative = sec;
                } else {
                    times.push_back(sec);
                }
                break;
            } else {
                std::cout << RED << "Invalid format!" << RESET << " Use M:S or H:M:S" << std::endl;
            }
        }
    }

    double total = 0;
    for (double t : times) total += t;

    int h = (int)total / 3600;
    int m = ((int)total % 3600) / 60;
    double s = total - (h * 3600) - (m * 60);
	// the outfile gets saved in ~/Documents/best_times.txt
    std::cout << "\n" << BOLD << "===============================" << RESET << std::endl;
    std::cout << GREEN << " FINAL SUM OF BEST: " << h << "h " << m << "m " << std::fixed << std::setprecision(2) << s << "s " << RESET << std::endl;
    std::cout << BOLD << "===============================" << RESET << std::endl;

    const char* homeDir = std::getenv("HOME");
    std::string fullPath = (homeDir != nullptr) ? std::string(homeDir) + "/Documents/best_times.txt" : "best_times.txt";

    std::ofstream outFile(fullPath, std::ios::app); 
    if (outFile.is_open()) {
        std::time_t now = std::time(0);
        char* dt = std::ctime(&now);

        outFile << "\n===========================================\n";
        outFile << "SESSION: " << dt;
        
        std::string rName;
        if (routeChoice == 1) rName = "Any% (Lake)";
        else if (routeChoice == 2) rName = "Any% (Wooded)";
        else if (routeChoice == 3) rName = "100%";
        else rName = "Custom";

        outFile << "ROUTE: " << rName << "\n";
        outFile << "MODE: " << (mode == 2 ? "Cumulative" : "Splits") << "\n";
        outFile << "===========================================\n";
        
        for (size_t i = 0; i < activeKingdoms.size(); ++i) {
            outFile << std::left << std::setw(15) << activeKingdoms[i] << ": " << times[i] << "s\n";
        }
        outFile << "TOTAL SOB: " << h << "h " << m << "m " << s << "s\n";
        outFile.close();
        std::cout << "\n[" << GREEN << "SUCCESS" << RESET << "] Session saved to " << fullPath << "!" << std::endl;
    }

    return 0;
}
