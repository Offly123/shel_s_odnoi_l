#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <signal.h>
#include <cstring>
#include <iomanip>
#include <cstdlib>
#include <fuse.h>

using namespace std;

bool vfsIsCreated = 0;

vector<string> commands = {
    "exit", "\\q",
    "run",
    "sh",
    "echo",
    "ch",
    "\\e",
    "sus",
    "\\l",
    "\\cron",
    "\\mem",
    "help",
    };

void show_history() {
    ifstream command_history_output("command_history.txt");
    string temp;
    cout << "История команд:" << endl;
    int i = 1;
    while (getline(command_history_output, temp)) {
        cout << setw(3) << i++ << ") " << temp << endl;
    }

    command_history_output.close();
}

void echo(string h) {
    cout << h.substr(5) << endl;
}

bool isValidCommand(string h) {
    bool isValid = false;
    if (h.find(' ') != -1) {
        h.erase(h.begin()+h.find(' '), h.end());
    }

    for (vector<string>::iterator it = commands.begin(); it != commands.end(); it++) {
        if (*(it) == h) {
            isValid = true;
            break;
        }
    }
    return isValid;
}

void executeFile(string &fileName) {
    char buf[255];
    strcpy(buf, fileName.c_str());
    system(buf);
}

void readSignal(int number) {
    system("");
    cout << "Configuration succesfully reloaded\n";
}

int main() {
    signal(SIGHUP, readSignal);
    string h;
    ofstream command_history_input("command_history.txt", ios::app);
    cout << "$ ";
    while (getline(cin, h)) {
        if (!isValidCommand(h)) {
            cout << h << ": command not found" << endl << "$ ";
        } else {
            command_history_input << h << endl;

            if (h == "exit" || h == "\\q") {
                cout << "Exiting...\n";
                break;
            } else if (!h.find("run")) {
                string temp = h.substr(4);
                executeFile(temp);
            } else if (h == "sh") {
                command_history_input.close();
                show_history();
                command_history_input.open("command_history.txt", ios::app);
            } else if (h == "sus") {
                cout << "    @@@@@@@     " << endl << "   @       @    " << endl << "  @         @   " << endl << "@@@       @@@@@ " << endl << "@ @      @     @" << endl << "@ @       @@@@@ " << endl << "@ @         @   " << endl << "@@@    @    @   " << endl << "  @    @    @   " << endl << "  @    @    @   " << endl << "   @@@@ @@@@    " << endl;
            } else if (h.find("echo ") == 0) {
                echo(h);
            } else if (h == "ch") {
                cout << "История очищена\n";
                command_history_input.close();
                command_history_input.open("command_history.txt");
            } else if (h.find("\\e ") == 0 && h[3] == '$') {
                string env_var = h.substr(4);
                const char* value = getenv(env_var.c_str());
                if (value) {
                    cout << env_var << " = " << value << endl;
                } else {
                    cout << "Environment variable " << env_var << " not found." << endl;
                }            
            } else if (h == "\\l") {
                system("df");
            } else if (h == "\\cron" && vfsIsCreated == 0) {
                system("mkdir /tmp/vfs");
                system("sudo mount -B /var/spool/cron/crontabs /tmp/vfs/");
                system("sudo ls /tmp/vfs");
            } else if (!h.find("\\mem")) {
                string temp = h.substr(5);
                cout << temp << endl;
                char buf[255];
                sprintf(buf, "sudo ./memory.out %s > hehe.bin", temp.c_str());
                system(buf);
                system("hexdump hehe.bin");
                cout << endl;
            } else if (h == "help") {
                for (vector<string>::iterator it = commands.begin(); it != commands.end(); it++) {
                    cout << *(it) << endl;
                }
            }

            cout << "$ ";

        }
    }
    if (vfsIsCreated) {
        system("sudo umount /tmp/vfs/");
        system("rm -r /tmp/vfs");
    }
    command_history_input.close();
    return 0;
}