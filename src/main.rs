use std::env;
use std::fs::{self, OpenOptions};
use std::io::{self, Read, Write};
use std::path::PathBuf;
use std::time::{Instant, SystemTime};

const RESET: &str = "\x1b[0m";
const RED: &str = "\x1b[0;31m";
const GOLD: &str = "\x1b[0;33m";
const GREEN: &str = "\x1b[0;32m";
const CYAN: &str = "\x1b[0;36m";
const CLEAR: &str = "\x1b[2J\x1b[H";

fn get_timestamp() -> String {
    let now = SystemTime::now();
    let datetime: chrono::DateTime<chrono::Local> = now.into();
    datetime.format("%y%m%d_%H%M%S").to_string()
}

fn ensure_dir() {
    let mut path = PathBuf::from(env::var("HOME").unwrap_or_default());
    path.push("smosummarysave");
    let _ = fs::create_dir_all(path);
}

fn timetoseconds(s: &str) -> f64 {
    if s == "b" || s == "undo" {
        return -2.0;
    }
    let parts: Vec<&str> = s.split(':').collect();
    let mut vals = Vec::new();

    for p in parts {
        match p.parse::<f64>() {
            Ok(v) => vals.push(v),
            Err(_) => return -1.0,
        }
    }

    match vals.len() {
        3 => (vals[0] * 3600.0) + (vals[1] * 60.0) + vals[2],
        2 => (vals[0] * 60.0) + vals[1],
        1 => vals[0],
        _ => -1.0,
    }
}

fn print_tui(kingdoms: &[String], times: &[f64], current_k: &str, current_run_time: f64) {
    print!("{}{}{}=== smosummary ==={}\n\n", CLEAR, GOLD, RESET, RESET);
    for i in 0..times.len() {
        println!("  {:12}: {}{:.2}s{}", kingdoms[i], CYAN, times[i], RESET);
    }
    if !current_k.is_empty() {
        print!(
            "{}> {:12}: {}{:.2}s\n\n[enter] split | [ctrl+c] quit",
            GREEN, current_k, RESET, current_run_time
        );
    } else {
        println!("\n{}run finished{}", GOLD, RESET);
    }
    let _ = io::stdout().flush();
}

fn main() -> io::Result<()> {
    let mut stopwatch = false;
    ensure_dir();

    let args: Vec<String> = env::args().collect();
    for i in 1..args.len() {
        let flag = &args[i];
        if flag == "-v" || flag == "--version" {
            println!("smosummary v3.0.1-3 | owner: aray4iv3");
            return Ok(());
        } else if flag == "-h" || flag == "--help" {
            println!("{}smosummary tui{}\nusage:\n  ./smosummary      - manual calculator mode\n  ./smosummary -s   - stopwatch tui\n  -v, --version     - owner info", GOLD, RESET);
            return Ok(());
        } else if flag == "-s" || flag == "--stopwatch" {
            stopwatch = true;
        } else {
            eprintln!("{}err: unknown flag {}{}", RED, flag, RESET);
            std::process::exit(1);
        }
    }

    print!(
        "{}.. smosummary ..{}\n1) any%\n2) 100%\n3) custom\n> ",
        GOLD, RESET
    );
    let _ = io::stdout().flush();

    let mut choice_str = String::new();
    io::stdin().read_line(&mut choice_str)?;
    let choice: i32 = choice_str.trim().parse().unwrap_or(0);

    let mut active: Vec<String> = Vec::new();
    if choice == 1 {
        active = vec![
            "cap", "cascade", "sand", "lake", "wooded", "cloud", "lost", "metro", "snow",
            "seaside", "luncheon", "ruined", "bowser", "moon",
        ]
        .into_iter()
        .map(String::from)
        .collect();
    } else if choice == 2 {
        active = vec![
            "mushroom", "cap", "cascade", "sand", "lake", "wooded", "cloud", "lost", "metro",
            "snow", "seaside", "luncheon", "ruined", "bowser", "moon", "dark", "darker",
        ]
        .into_iter()
        .map(String::from)
        .collect();
    } else if choice == 3 {
        println!("enter kingdom names (type 'done' to finish):");
        loop {
            let mut k_name = String::new();
            io::stdin().read_line(&mut k_name)?;
            let trimmed = k_name.trim();
            if trimmed == "done" || trimmed.is_empty() {
                break;
            }
            active.push(trimmed.to_string());
        }
    }

    if active.is_empty() {
        return Ok(());
    }

    let mut save_path = PathBuf::from(env::var("HOME").unwrap_or_default());
    save_path.push("smosummarysave");
    save_path.push(format!("run_{}.txt", get_timestamp()));

    let mut times: Vec<f64> = Vec::new();

    if stopwatch {
        print!("hit enter to start...");
        let _ = io::stdout().flush();
        let mut dummy = [0u8; 1];
        let _ = io::stdin().read(&mut dummy);

        let mut last = Instant::now();

        // Setup for polling stdin
        use std::os::unix::io::AsRawFd;
        let mut poll_fds = [libc::pollfd {
            fd: io::stdin().as_raw_fd(),
            events: libc::POLLIN,
            revents: 0,
        }];

        for k in &active {
            loop {
                let ret = unsafe { libc::poll(poll_fds.as_mut_ptr(), 1, 50) };
                if ret > 0 {
                    break;
                }

                let now = Instant::now();
                let duration = now.duration_since(last).as_secs_f64();
                print_tui(&active, &times, k, duration);
            }

            let mut buf = String::new();
            io::stdin().read_line(&mut buf)?;

            let now = Instant::now();
            let split = now.duration_since(last).as_secs_f64();
            times.push(split);

            let mut f = OpenOptions::new()
                .create(true)
                .append(true)
                .open(&save_path)?;
            writeln!(f, "{}: {:.2}s", k, split)?;
            last = now;
        }
        print_tui(&active, &times, "", 0.0);
    } else {
        let mut i = 0;
        while i < active.len() {
            print!("{} (or 'b' to undo) > ", active[i]);
            let _ = io::stdout().flush();
            let mut input = String::new();
            io::stdin().read_line(&mut input)?;
            let t = timetoseconds(input.trim());

            if t == -2.0 {
                if i > 0 {
                    i -= 1; // Back up one (loop adds 1 later)
                    times.pop();
                    println!("{}undone last segment.{}", RED, RESET);
                    continue;
                } else {
                    println!("{}nothing to undo.{}", RED, RESET);
                    continue;
                }
            }
            if t < 0.0 {
                println!("{}invalid format.{}", RED, RESET);
                continue;
            }
            times.push(t);
            i += 1;
        }
        let mut f = fs::File::create(&save_path)?;
        for j in 0..active.len() {
            writeln!(f, "{}: {:.2}s", active[j], times[j])?;
        }
    }

    let total: f64 = times.iter().sum();
    let mut f = OpenOptions::new().append(true).open(&save_path)?;
    writeln!(f, "-------------------\ntotal: {:.2}s", total)?;

    println!("\n{}final total: {:.2}s{}", GOLD, total, RESET);
    println!("saved: {}", save_path.display());

    Ok(())
}
