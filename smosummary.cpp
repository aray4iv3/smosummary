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

#define RESET   "\033[0m"
#define RED     "\033[0;31m"
#define GOLD    "\033[0;33m"   
#define GREEN   "\033[0;32m"      
#define CYAN    "\033[0;36m"   

// parse string to seconds.
double timetoseconds(std::string s) {
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

// basic html summary.
void generatehtml(const std::vector<std::string>& kingdoms, const std::vector<double>& times, double total) {
    std::time_t now = std::time(0);
    std::tm* ltm = std::localtime(&now);
    std::stringstream ss;
    ss << "summary_" << 1900 + ltm->tm_year << "-" << std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon << "-" 
       << std::setfill('0') << std::setw(2) << ltm->tm_mday << "_" << std::setfill('0') << std::setw(2) << ltm->tm_hour << ".html";
    
    std::string filename = ss.str();
    const char* home = std::getenv("HOME");
    std::string path = (home != nullptr) ? std::string(home) + "/Documents/" + filename : filename;

    std::ofstream html(path);
    if (!html.is_open()) return;

    int h = (int)total / 3600, m = ((int)total % 3600) / 60;
    double s = total - (h * 3600) - (m * 60);

    html << "<!DOCTYPE html><html><head><style>body{background:#121212;color:#e0e0e0;font-family:monospace;padding:40px;}";
    html << ".total{font-size:24px;color:#50fa7b;margin:20px 0;} table{width:100%;} td{padding:8px;border-bottom:1px solid #222;}</style></head>";
    html << "<body><h1>mission summary</h1><div class='total'>final: " << h << "h " << m << "m " << std::fixed << std::setprecision(2) << s << "s</div>";
    html << "<table>";
    for(size_t i=0; i<kingdoms.size(); ++i) html << "<tr><td>" << kingdoms[i] << "</td><td style='text-align:right;'>" << times[i] << "s</td></tr>";
    html << "</table></body></html>";
    html.close();
    std::cout << "[" << GREEN << "ok" << RESET << "] html saved to documents" << std::endl;
}

int main(int argc, char* argv[]) {
    bool stopwatch = false;
    if (argc > 1) {
        std::string flag = argv[1];
        if (flag == "--version" || flag == "-v") {
            std::cout << "smosummary v2.3.0-1 | tool by aray4iv3\n" << std::endl;
            return 0; 
        }
        if (flag == "--stopwatch" || flag == "-s") stopwatch = true;
    }

    std::vector<double> times; std::vector<std::string> active; 
    std::string r_name = "custom"; int choice = 0;

    std::cout << GOLD << ".. smosummary " << (stopwatch ? "(timer)" : "(manual)") << " .." << RESET << std::endl;

    while (true) {
        std::cout << "\n1) any% menu\n2) 100%\n3) custom\n4) import\n" << GOLD << "smosummary by aray4iv3" << RESET << "\n> ";
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
        
        if (choice == 1) {
            int branch1;
            active = {"cap", "cascade", "sand"};
            
            std::cout << "\nbranch a:\n1) lake\n2) wooded\n> "; std::cin >> branch1;
            active.push_back((branch1 == 1) ? "lake" : "wooded");
            
            active.insert(active.end(), {"cloud", "lost", "metro"});

            std::cout << "\nbranch b (e.g. 123):\n1) snow\n2) seaside\n3) luncheon\norder > ";
            std::string order; std::cin >> order;
            for(char c : order) {
                if (c == '1') active.push_back("snow");
                if (c == '2') active.push_back("seaside");
                if (c == '3') active.push_back("luncheon");
            }

            active.insert(active.end(), {"ruined", "bowser's", "moon"});
            r_name = (branch1 == 1) ? "any% lake" : "any% wooded";
            break;
        }
        if (choice == 2) { 
            active = {"mushroom", "cap", "cascade", "sand", "lake", "wooded", "cloud", "lost", "metro", "snow", "seaside", "luncheon", "ruined", "bowser's", "moon", "dark side", "darker side"};
            r_name = "100%"; 
            break; 
        }
        if (choice == 3) {
            int c; std::cout << "segments: "; std::cin >> c;
            for(int i=0; i<c; ++i) { std::string n; std::cout << i+1 << ": "; std::cin >> n; active.push_back(n); }
            break;
        }
        if (choice == 4) {
            std::string f; std::cout << "path: "; std::cin >> f; std::ifstream ifile(f);
            if (ifile.is_open()) { std::string l; while(std::getline(ifile, l)) if(!l.empty()) active.push_back(l); ifile.close(); r_name = "import"; break; }
        }
    }

    if (stopwatch) {
        std::cout << GREEN << "\nready." << RESET << " hit enter." << std::endl;
        std::cin.ignore(1000, '\n'); std::cin.get();
        auto start = std::chrono::high_resolution_clock::now(); auto last = start;
        struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
        for (const std::string& k : active) {
            while (true) {
                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> d = now - last;
                std::chrono::duration<double> run = now - start;
                std::cout << "\r\033[K" << CYAN << std::left << std::setw(10) << k << RESET 
                          << " | seg: " << std::fixed << std::setprecision(1) << d.count() << "s"
                          << " | run: " << (int)run.count() << "s" << std::flush;
                if (poll(&pfd, 1, 50) > 0) { std::cin.get(); times.push_back(d.count()); std::cout << " -> " << GREEN << "saved" << RESET; last = now; break; }
            }
        }
    } else {
        for (const std::string& k : active) {
            while (true) { 
                std::string in; std::cout << k << " > "; std::cin >> in; 
                double s = timetoseconds(in); if (s >= 0) { times.push_back(s); break; } 
            }
        }
    }

    double total = 0; for (double t : times) total += t;
    int h = (int)total / 3600, m = ((int)total % 3600) / 60;
    double s = total - (h * 3600) - (m * 60);

    std::cout << "\n\n" << GOLD << "done." << RESET << std::endl;
    std::cout << "time: " << h << "h " << m << "m " << std::fixed << std::setprecision(2) << s << "s" << std::endl;

    // log with full breakdown
    const char* home = std::getenv("HOME");
    std::string log_path = (home != nullptr) ? std::string(home) + "/smosummary.txt" : "smosummary.txt";
    
    std::time_t now_log = std::time(0);
    char* dt = std::ctime(&now_log);
    std::string date_str(dt);
    if (!date_str.empty()) date_str.pop_back();

    std::ofstream log(log_path, std::ios::app);
    if (log.is_open()) {
        log << "\n[" << date_str << "] " << r_name << "\n";
        for (size_t i = 0; i < active.size(); ++i) {
            log << std::left << std::setw(12) << active[i] << ": " << times[i] << "s\n";
        }
        log << "FINAL: " << h << "h " << m << "m " << std::fixed << std::setprecision(2) << s << "s\n";
        log << "----------------------------------\n";
        log.close();
        std::cout << "journal updated in ~/smosummary.txt" << std::endl;
    }

    std::cout << "\nsave html? (y/n) "; char ex; std::cin >> ex;
    if (ex == 'y') generatehtml(active, times, total);

    return 0;
}
