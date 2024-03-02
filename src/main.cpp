#include <iostream>
#include <thread>
#include <getopt.h>
#include <unistd.h>
#include "header/stopwatch.h"
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>


using namespace std;

const string progname = "sw";

std::string formatted_args;

void enableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void disableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO; // Re-enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

bool kbhit() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting > 0;
}

void print_help();

void read_sentence(int argc, char *argv[])
{
    std::vector<std::string> all_args;

    if (argc == 3){
        printf("JOE"); fflush(stdout);
        formatted_args = argv[3];
    }
    else if (argc > 2) {
        // Start from argv[3] and collect all arguments
        for (int i = 3; i < argc; ++i) {
            all_args.push_back(argv[i]);
        }

        // Create a formatted string with argv[3], argv[4], argv[5], etc.
        for (const auto& arg : all_args) {
            formatted_args += arg + " ";
        }

        // Remove trailing space
        // formatted_args.pop_back();

        // Now 'formatted_args' contains the concatenated string
        
        // std::cout << "Concatenated arguments: " << formatted_args << std::endl;
    } 
    
    else {
        std::cout << "No arguments provided." << std::endl;
    }

    return;
}




int main(int argc, char *argv[])
{
    read_sentence(argc, argv);

    int c, idxopt;
    int help = 0, milliseconds = 1;
    string countdown, format = "%T";
    // To change stopwatch color (works on UNIX)
    string fg, bg, color, reset;

	struct option t_opt[] =
	{
		{"help",		no_argument,			&help,		    1},
        {"ms",	    	no_argument,			&milliseconds,  0},
        {"fg",          required_argument,		NULL,		    't'},
        {"bg",		    required_argument,		NULL,		    'b'},
        {"countdown",	required_argument,		NULL,		    'c'},
        {"format",  	required_argument,		NULL,		    'f'},
        {0,         	0,              		NULL,		    0}
	};
	
	while ((c = getopt_long(argc, argv, "hmc:p:f:t:b:", t_opt, &idxopt)) != -1){
		switch (c){
			case 'h': help = 1;
				break;
            case 'm': milliseconds = 0;
				break;
            case 'c': countdown = optarg;
                break;
            case 'f': format = optarg;
				break;
#ifdef __unix__
            case 't': fg = optarg;
				break;
            case 'b': bg = optarg;
				break;
#endif
			default:
				break;
		}
  	}

#ifdef __unix__
    if (!fg.empty()){
        color = fg;
    }
    if (!bg.empty()){
        color += (color.empty() ? bg : ";"+bg);
    }
    if (!color.empty()){
        color.insert(0, "\033[");
        color.push_back('m');
        reset = "\033[0m";
    }
#endif

    // Instantiate the stopwatch
    Stopwatch sw;

    if (help){
        print_help();
    } else if (!isatty(STDIN_FILENO)){
        // We want to measure the execution time of a program (piped)
        sw.start();
        
        char buffer[1];
        while (read(STDIN_FILENO, buffer, sizeof(buffer)) > 0) cout << buffer;
        
        // To compensate for a slight advance in the tube process
        sw.addMsecs(Stopwatch::ms(10));

        cout << color << '\r' << sw.elapsed(format.c_str(), milliseconds) << reset << flush;
    } else if (!countdown.empty()){
        enableRawMode(); // Enable raw mode at the start of the countdown

         // hh:mm:ss
        int h, m, s;
        h = m = s = 0;

        // Existing countdown initialization code...
        h = stoi(countdown.substr(0, 2));
        m = stoi(countdown.substr(3, 5));
        s = stoi(countdown.substr(6, 8));

        // Init values
        Stopwatch::hrs dh(h);
        Stopwatch::mins dm(m);
        Stopwatch::secs ds(s);
        Stopwatch::ms countdown(dh + dm + ds);
        Stopwatch::ms timeout(0);

        sw.start(countdown);
        bool paused = false;

        while (sw.remaining() > timeout) {
            if (kbhit()) {
                char c = getchar();
                if (c == 'p' || c == 'P') {
                    paused = !paused;
                    if (paused) {
                        std::cout << "\nPaused. Press 'p' to resume.\n";
                    } else {
                        std::cout << "Resumed.\n";
                        // Logic to resume might be adjusting the 'begin' time in Stopwatch
                    }
                }
            }

            if (!paused) {
                // Update and display the stopwatch or countdown timer
                std::cout << color << '\r' << sw.remaining(format.c_str(), milliseconds) << " | " << formatted_args << reset << std::flush;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        disableRawMode(); // Disable raw mode after the countdown

    } else {
        // Start the chrono
        sw.start();

        while (1){
            cout << color << '\r' << sw.elapsed(format.c_str(), milliseconds) << reset << flush;
            std::this_thread::sleep_for(Stopwatch::ms(100));
        }
    }


    cout << endl;

    return 0;
}

void print_help(){
    cout << "Usage: " << progname << "\n"
        "\t--help, \t-h\t Show help\n"
        "\t--ms, \t\t-m\t Hide milliseconds\n"
        "\t--countdown, \t-c\t [hh:mm:ss]\n"
        "\t--format, \t-f\t Display format (default: %T with milliseconds)\n"
        "\t\t\t\t https://en.cppreference.com/w/cpp/io/manip/put_time\n"
#ifdef __unix__
        "\t--fg, \t\t-t \t [30-37;90-97] Change the foreground color\n"
        "\t--bg, \t\t-b \t [40-47;100-107] Change the background color\n"
#endif
        "Example of use:\n"
        << progname << endl
#ifdef __unix__
        << progname << " --fg 97" << endl
        << progname << " --fg 97 --bg 46" << endl
#endif
        << progname << " --countdown 00:00:10" << endl
        << progname << " --format %H:%M:%S";
}
