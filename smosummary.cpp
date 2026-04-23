#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <poll.h>
#include <unistd.h>

// colors!!
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

int main(int argc, char* argv[]) {
    bool stopwatchMode = false;
    
    // flags
    if (argc > 1) {
        std::string flag = argv[1];
        if (flag == "--version" || flag == "-v") {
            std::cout << GOLD << BOLD << "smosummary v2.1.0-1" << std::endl;
            std::cout << "creator and publisher: aray4iv3" << RESET << std::endl;
            return 0; 
        }
        if (flag == "--stopwatch" || flag == "-s") {
            stopwatchMode = true;
        }
    }

    // preset modes
    std::vector<std::string> anyLake = {"Cap", "Cascade", "Sand", "Lake", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon"};
    std::vector<std::string> anyWooded = {"Cap", "Cascade", "Sand", "Wooded", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon"};
    std::vector<std::string> hundredPercent = {"Mushroom", "Cap", "Cascade", "Sand", "Lake", "Wooded", "Cloud", "Lost", "Metro", "Snow", "Seaside", "Luncheon", "Ruined", "Bowser's", "Moon", "Dark Side", "Darker Side"};

    std::vector<double> times;
    std::vector<std::string> activeKingdoms;
    std::string input;
    int routeChoice = 0;
    int mode = 1;

    std::cout << GOLD << "--- smosummary: " << (stopwatchMode ? "Live Stopwatch" : "Sum of Best") << " ---" << RESET << std::endl;
    
    // loooooooooooooooooooooooooooooooop
    while (true) {
        std::cout << BOLD << "\nSelect Route:" << RESET << std::endl;
        std::cout << "1) Any% (Lake)\n2) Any% (Wooded)\n3) 100%\n4) Custom (Manual Entry)\n5) Import .txt Route\nSelection > ";
        
        if (!(std::cin >> routeChoice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        if (routeChoice == 1) { activeKingdoms = anyLake; break; }
        else if (routeChoice == 2) { activeKingdoms = anyWooded; break; }
        else if (routeChoice == 3) { activeKingdoms = hundredPercent; break; }
        else if (routeChoice == 4) {
            int count;
            std::cout << "How many segments? "; std::cin >> count;
            for(int i = 0; i < count; ++i) {
                std::string name;
                std::cout << "Name for segment " << (i+1) << ": ";
                std::cin >> name;
                activeKingdoms.push_back(name);
            }
            break;
        }
        else if (routeChoice == 5) {
            std::string filename;
            std::cout << "Enter path to route file (e.g., route.txt): ";
            std::cin >> filename;
            std::ifstream inFile(filename);
            if (inFile.is_open()) {
                std::string line;
                activeKingdoms.clear(); 
                while (std::getline(inFile, line)) {
                    if (!line.empty()) activeKingdoms.push_back(line);
                }
                inFile.close();
                std::cout << GREEN << "Successfully imported " << activeKingdoms.size() << " segments!" << RESET << std::endl;
                break; 
            } else {
                std::cout << RED << "Error: Could not open file. Please check the path and try again." << RESET << std::endl;
            }
        } else {
            std::cout << RED << "Invalid selection! Enter 1-5." << RESET << std::endl;
        }
    }

    // mode selection
    while (true) {
        std::cout << BOLD << "\nChoose Mode:" << RESET << "\n1) Split Times\n2) Cumulative Times\nSelection > ";
        if (std::cin >> mode && (mode == 1 || mode == 2)) break;
        std::cout << RED << "Invalid selection! Enter 1 or 2." << RESET << "\n";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    // stopwatch or manual
    if (stopwatchMode) {
        std::cout << GREEN << "\nSTOPWATCH READY." << RESET << " Press ENTER to start the run!" << std::endl;
        std::cin.ignore(1000, '\n'); 
        std::cin.get(); 
        
        auto runStart = std::chrono::high_resolution_clock::now();
        auto lastSplit = runStart;

        struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};

        for (const std::string& k : activeKingdoms) {
            while (true) {
                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = now - lastSplit;
                std::chrono::duration<double> totalDiff = now - runStart;
                
                std::cout << "\r\033[K" << CYAN << std::left << std::setw(12) << k << RESET << " | ";
                
                if (mode == 1) {
                    std::cout << "SEGMENT: " << std::fixed << std::setprecision(2) << diff.count() << "s"
                              << " | TOTAL: " << totalDiff.count() << "s" << std::flush;
                } else {
                    std::cout << "CUMULATIVE: " << std::fixed << std::setprecision(2) << totalDiff.count() << "s" << std::flush;
                }

                if (poll(&pfd, 1, 33) > 0) { // ~30 FPS refresh rate
                    std::cin.get(); 
                    times.push_back(diff.count());
                    std::cout << "\n" << GREEN << " >> " << k << " SAVED: " << diff.count() << "s" << RESET << std::endl;
                    lastSplit = now;
                    break;
                }
            }
        }
    } else {
        double previousCumulative = 0;
        std::cout << "\nEnter times (M:S). Type 'skip' for 0.\n" << std::endl;
        for (const std::string& k : activeKingdoms) {
            while (true) {
                std::cout << CYAN << k << RESET << " > ";
                std::cin >> input;
                if (input == "skip") { times.push_back(0); break; }
                double sec = timeToSeconds(input);
                if (sec >= 0) {
                    if (mode == 2) {
                        double segmentTime = sec - previousCumulative;
                        times.push_back(segmentTime >= 0 ? segmentTime : 0);
                        previousCumulative = sec;
                    } else times.push_back(sec);
                    break;
                }
                std::cout << RED << "Invalid format!" << RESET << std::endl;
            }
        }
    }

    // calculate
    double total = 0;
    for (double t : times) total += t;
    int h = (int)total / 3600;
    int m = ((int)total % 3600) / 60;
    double s = total - (h * 3600) - (m * 60);

    std::cout << "\n" << BOLD << "===============================" << RESET << std::endl;
    std::cout << GREEN << " FINAL TOTAL TIME: " << h << "h " << m << "m " << std::fixed << std::setprecision(2) << s << "s " << RESET << std::endl;
    std::cout << BOLD << "===============================" << RESET << std::endl;

    // export
    const char* homeDir = std::getenv("HOME");
    std::string fullPath = (homeDir != nullptr) ? std::string(homeDir) + "/Documents/best_times.txt" : "best_times.txt";
    std::ofstream outFile(fullPath, std::ios::app);
    if (outFile.is_open()) {
        std::time_t now = std::time(0);
        outFile << "\nSESSION: " << std::ctime(&now) << "TYPE: " << (stopwatchMode ? "LIVE STOPWATCH" : "MANUAL ENTRY") << "\nTOTAL: " << h << "h " << m << "m " << s << "s\n---------------------------\n";
        for (size_t i = 0; i < times.size(); ++i) {
            outFile << activeKingdoms[i] << ": " << times[i] << "s\n";
        }
        outFile.close();
        std::cout << "[" << GREEN << "SUCCESS" << RESET << "] Session saved to " << fullPath << std::endl;
    }

    return 0;
}
