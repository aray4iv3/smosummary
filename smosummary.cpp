#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <poll.h>
#include <unistd.h>
#include <sys/stat.h>

#define RESET   "\033[0m"
#define RED     "\033[0;31m"
#define GOLD    "\033[0;33m"   
#define GREEN   "\033[0;32m"      
#define CYAN    "\033[0;36m"
#define CLEAR   "\033[2J\033[H"

std::string get_timestamp() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%y%m%d_%h%m%s");
    return ss.str();
}

void ensure_dir() {
    std::string path = std::string(getenv("HOME")) + "/smosummarysave";
    mkdir(path.c_str(), 0777);
}

double timetoseconds(std::string s) {
    if (s == "b" || s == "undo") return -2; 
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

void print_tui(const std::vector<std::string>& kingdoms, const std::vector<double>& times, const std::string& current_k, double current_run_time) {
    std::cout << CLEAR << GOLD << "=== smosummary tui ===" << RESET << "\n\n";
    for (size_t i = 0; i < times.size(); ++i) {
        std::cout << "  " << std::left << std::setw(12) << kingdoms[i] << ": " << CYAN << std::fixed << std::setprecision(2) << times[i] << "s" << RESET << "\n";
    }
    if (!current_k.empty()) {
        std::cout << GREEN << "> " << std::left << std::setw(12) << current_k << ": " << RESET 
                  << std::fixed << std::setprecision(2) << current_run_time << "s\n\n"
                  << "[enter] split | [ctrl+c] quit";
    } else {
        std::cout << "\n" << GOLD << "run finished" << RESET << "\n";
    }
    std::cout << std::flush;
}

int main(int argc, char* argv[]) {
    bool stopwatch = false;
    ensure_dir();

    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "-v" || flag == "--version") { 
            std::cout << "smosummary v2.4.0-1 | owner: aray4iv3\n"; 
            return 0; 
        }
        else if (flag == "-h" || flag == "--help") {
            std::cout << GOLD << "smosummary tui" << RESET << "\n"
                      << "usage:\n"
                      << "  ./smosummary      - manual calculator mode\n"
                      << "  ./smosummary -s   - stopwatch tui\n"
                      << "  -v, --version     - owner info\n";
            return 0;
        }
        else if (flag == "-s" || flag == "--stopwatch") stopwatch = true;
        else { std::cerr << RED << "err: unknown flag " << flag << RESET << "\n"; return 1; }
    }

    std::cout << GOLD << ".. smosummary .." << RESET << "\n1) any%\n2) 100%\n3) custom\n> ";
    int choice; 
    if (!(std::cin >> choice)) return 0;

    std::vector<std::string> active;
    if (choice == 1) active = {"cap", "cascade", "sand", "lake", "wooded", "cloud", "lost", "metro", "snow", "seaside", "luncheon", "ruined", "bowser", "moon"};
    else if (choice == 2) active = {"mushroom", "cap", "cascade", "sand", "lake", "wooded", "cloud", "lost", "metro", "snow", "seaside", "luncheon", "ruined", "bowser", "moon", "dark", "darker"};
    else {
        std::cout << "enter kingdom names (type 'done' to finish):\n";
        std::string k_name;
        while (std::cin >> k_name && k_name != "done") active.push_back(k_name);
    }

    if (active.empty()) return 0;

    std::string save_path = std::string(getenv("HOME")) + "/smosummarysave/run_" + get_timestamp() + ".txt";
    std::vector<double> times;

    if (stopwatch) {
        std::cout << "hit enter to start...";
        std::cin.ignore(1000, '\n'); std::cin.get();
        auto last = std::chrono::high_resolution_clock::now();
        struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };

        for (auto& k : active) {
            while (poll(&pfd, 1, 50) <= 0) {
                auto now = std::chrono::high_resolution_clock::now();
                print_tui(active, times, k, std::chrono::duration<double>(now - last).count());
            }
            std::cin.get();
            auto now = std::chrono::high_resolution_clock::now();
            double split = std::chrono::duration<double>(now - last).count();
            times.push_back(split);
            std::ofstream f(save_path, std::ios::app);
            f << k << ": " << split << "s\n";
            last = now;
        }
        print_tui(active, times, "", 0);
    } else {
        for (size_t i = 0; i < active.size(); ++i) {
            std::string in;
            std::cout << active[i] << " (or 'b' to undo) > ";
            std::cin >> in;
            double t = timetoseconds(in);
            if (t == -2) { 
                if (i > 0) { i -= 2; times.pop_back(); std::cout << RED << "undone last segment.\n" << RESET; }
                else { i--; std::cout << RED << "nothing to undo.\n" << RESET; }
                continue;
            }
            if (t < 0) { std::cout << RED << "invalid format.\n" << RESET; i--; continue; }
            times.push_back(t);
        }
        std::ofstream f(save_path);
        for(size_t i=0; i<active.size(); ++i) f << active[i] << ": " << times[i] << "s\n";
    }

    double total = 0; for (double t : times) total += t;
    std::ofstream f(save_path, std::ios::app);
    f << "-------------------\ntotal: " << total << "s\n";
    
    std::cout << "\n" << GOLD << "final total: " << total << "s" << RESET << "\n";
    std::cout << "saved: " << save_path << "\n";

    return 0;
}
