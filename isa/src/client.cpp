#include "client.hpp"

static struct option long_options[] = {
    {"address", required_argument, 0, 'a'},
    {"port", required_argument, 0, 'p'},
    {"help", no_argument, 0, 'h'}
};

int sock;
struct sockaddr_in sock_ip;

int main(int argc, char** argv) {
    string address = "localhost";
    uint16_t port = 32323;

    int opt, longopt;


    while (true) {
        opt = getopt_long(argc, argv, "a:p:h", long_options, &longopt);
        if (opt == -1){
            print_help();
            break;
        } 

        switch (opt) {
            case 'a':
                address = optarg;
                break;
            case 'p':
                port = stoi(string(optarg));
                if (port > 65535 || port < 0) {
                    cerr << "Bad port number!" << endl;
                    return STATUS_ERR;
                }
                break;
            case 'h':
            default:
                print_help();
                break;
            
        }
    }
    
    vector<string> command;
    for (int i = optind; i < argc; i++) {
        command.push_back(argv[i]);
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    string connip = process_address(address);
    sock_ip.sin_family = AF_INET;
    in_addr addr;
    inet_aton(connip.c_str(), &addr);
    memcpy(&sock_ip.sin_addr.s_addr, &addr.s_addr, connip.size());
    sock_ip.sin_port = htons(port);
    int rc = connect(sock, (const struct sockaddr*)&sock_ip, sizeof(sock_ip));
    if (rc == -1) cerr << "error " << errno << " while connecting" << endl;
    process_command(command);
    close(sock);
    return 0;
}

/**
 * Funkce pro kontrolu, zda vstupním parametrem je IP adresa nebo doménové jméno
 * Pokud se jedná o IP adresu, je vrácena v původním stavu, pokud ne, přeloží se na IP adresu a vrátí se IP serveru
 */
string process_address(string input) {
    char ipregex[] = "([0-9]{1,3}\\.){3}[0-9]{1,3}"; // zjednodušený regex pro základní detekci tvaru IÚ
    bool isip = true;
    if (regex_match(input, regex(ipregex))) {
        stringstream ip;
        cout << input << endl;
        ip.str(input+"\n");
        vector<string> ipparts;
        for (int i = 0; i < 4; i++) { // rozdělení domnělé IP na součásti a ověření rozsahů
            string tmp;
            getline(ip, tmp, '.');
            ipparts.push_back(tmp);
        } 
        for (string p: ipparts) {
            int i = stoi(p);
            cout << i << endl;
            if (i < 0 || i > 255) {
                isip = false;
            }
        }
    } else {
        isip = false;
    }
    if (isip) {
        return input;
    } else { // v případě, že podle rozsahu nebo tvaru nejde o IP, pokusíme se o převod pomocí DNS
        hostent* host = gethostbyname(input.c_str());
        string out;
        if (host != NULL) {
            char* addr = inet_ntoa(*((in_addr*)host->h_addr));
            out = string(addr);
        } else {
            out = "";
        }
        return out;
    }
}

/**
 * Detekuje příkaz, ovaří počet jeho parametrů a případně zavolá obslužnou funkci
 * @param command   vektor příkazů a parametrů získaných z getopt
 */
int process_command(vector<string> command) {
    int status = STATUS_ERR;
    if (command.size() < 1) {
        cerr << "No valid command found!" << endl;
        return STATUS_ERR;
    }
    if (command[0] == "register") {
        if (command.size() != 3) {
            cerr << "Usage: register <username> <password>" << endl;
            return STATUS_PARAMS;
        }
        status = cmd_register(command[1], base64encode(command[2]));
    } else if (command[0] == "login")
    {
        if (command.size() != 3) {
            cerr << "Usage: login <username> <password>" << endl;
            return STATUS_PARAMS;
        }
        status = cmd_login(command[1], base64encode(command[2]));
    } else if (command[0] == "list")
    {
        if (command.size() != 1) {
            cerr << "Usage: list" << endl;
            return STATUS_PARAMS;
        }
        status = cmd_list();
    } else if (command[0] == "send") {
        if (command.size() != 4) {
            cerr << "Usage: send <recipient> <subject> <body>" << endl;
            return STATUS_PARAMS;
        }
        status = cmd_send(command[1], command[2], command[3]);
    } else if (command[0] == "fetch") {
        if (command.size() != 2) {
            cerr << "Usage: fetch <id>" << endl;
            return STATUS_PARAMS;
        }
        status = cmd_fetch(strtod(command[1].c_str(), NULL));
    } else if (command[0] == "logout") {
        if (command.size() != 1) {
            cerr << "Usage: logout" << endl;
            return STATUS_PARAMS;
        }
        status = cmd_logout();
    } else {
        cerr << "No valid command found!" << endl;
        return STATUS_ERR;
    }
    return status;
    
}

/**
 * Tiskne nápovědu
 */
void print_help() {
    cout << "usage: client [ <option> ... ] <command> [<args>] ..." << endl << endl;
    cout << "<option> is one of" << endl << endl;
    cout << "\t-a <addr>, --address <addr>" << endl << "\t\tServer hostname or address to connect to" << endl;
    cout << "\t-p <port>, --port <port>" << endl << "\t\tServer port to connect to" << endl;
    cout << "\t--help, -h" << endl << "\t\tShow this help" << endl;
    cout << "Supported commands:" << endl;
    cout << "\tregister <username> <password>" << endl;
    cout << "\tlogin <username> <password>" << endl;
    cout << "\tlist" << endl;
    cout << "\tsend <recipient> <subject> <body>" << endl;
    cout << "\tfetch <id>" << endl;
    cout << "\tlogout" << endl;
    exit(0);
}

/**
 * Obsluhuje příkaz register
 * @param username  uživatelské jméno
 * @param password  base64 kódované heslo
 */
int cmd_register(string username, string password) {
    string reply = cmdsend("(register \"" + username + "\" \"" + password + "\")");
    if (reply != "") {
        // 5 znaků kvůli odstínění (ok , -7 5 znaků ze začátku + ")
        cout << reply.substr(5, reply.size()-7) << endl;
    }
    return STATUS_OK;
}

/**
 * Obsluha příkazu login
 * @param username  uživatelské jméno
 * @param password  base64 kódované heslo
 */
int cmd_login(string username, string password)
{
    string reply = cmdsend("(login \"" + username + "\" \"" + password + "\")");
    stringstream s;
    s.str(reply);
    s.seekg(1);
    string buffer; 
    getline(s, buffer, ' '); // oddělení úvodu odpovědi
    if (buffer == "ok") {
        cout << "user logged in" << endl;
        s.seekg(21); // za "user logged in"
        getline(s, buffer, ')'); // získání tokenu ze zbytku dat
        write_token(buffer);
    }
    return STATUS_OK;
}

/**
 * Obsluha příkazu list pro získání seznamu přijatých zpráv
 */
int cmd_list() {
    // regex pro značení jedné zprávy protokolu s submatchi pro jednotlivé části zprávy
    regex msgregex ("\\(([0-9]+) (\".*?\") (\".*?\")\\)");
    string tok = read_token();
    regex lfunesc("\\\\n");
    if (tok != "") {
        string msglist = cmdsend("(list " + tok + ")");
        auto message_begin = sregex_iterator(msglist.begin(), msglist.end(), msgregex);
        auto message_end = sregex_iterator();
        for (sregex_iterator i = message_begin; i != message_end; ++i) {
            string message = (*i).str();
            smatch match;
            regex_search(message, match, msgregex);
            cout << match[1] << ":" << endl;
            cout << "  From: " << regex_replace(trimquotes(match[2].str()), lfunesc, "\n") << endl;
            cout << "  Subject: " << regex_replace(trimquotes(match[3].str()), lfunesc, "\n") << endl;
        }
    } else {
        cout << "Not logged in" << endl;
    }
    return STATUS_OK;
}

/**
 * Obsluha příkazu send pro odeslání zpráv
 * @param recipient     jméno příjemce zprávy
 * @param subject       předmět zprávy
 * @param body          tělo zprávy
 */
int cmd_send(string recipient, string subject, string body) {
        string tok = read_token();
    if (tok != "") {
        string reply = cmdsend("(send " + tok + " \"" + recipient + "\" \"" + subject + "\" \"" + body + "\")");
        if (reply != "") {
            // 5 znaků kvůli odstínění (ok , -7 5 znaků ze začátku + ")
            cout << reply.substr(5, reply.size()-7) << endl;
        }
    } else {
        cout << "Not logged in" << endl;
    }
    return STATUS_OK;
}

/**
 * Obsluha příkazu fetch pro načtení zprávy ze serveru
 * @param message_id ID zprávy
 */
int cmd_fetch(int message_id) {
    regex msgregex ("\\((\".*?\") (\".*?\") (\".*?\")\\)");
    regex lfunesc("\\\\n");
    string tok = read_token();
    if (tok != "") {
        string reply = cmdsend("(fetch " + tok + " " + to_string(message_id) + ")");
        if (reply != "") {
            cout << endl << endl;
        }
        auto message_begin = sregex_iterator(reply.begin(), reply.end(), msgregex);
        auto message_end = sregex_iterator();
        for (sregex_iterator i = message_begin; i != message_end; ++i) {
            string message = (*i).str();
            smatch match;
            regex_search(message, match, msgregex);
            cout << "From: " << trimquotes(match[1].str()) << endl;
            cout << "Subject: " << regex_replace(trimquotes(match[2].str()), lfunesc, "\n") << endl << endl;
            cout << regex_replace(trimquotes(match[3].str()), lfunesc, "\n") << endl;
        }
    } else {
        cout << "Not logged in" << endl;
    }
    return STATUS_OK;
}

/**
 * Obsluha příkazu pro odhlášení
 */
int cmd_logout() {
    string tok = read_token();
    if (tok != "") {
        cmdsend("(logout " + tok + ")");
        destroy_token();
        cout << "logged out" << endl;
    } else {
        cout << "Not logged in" << endl;
    }
    return STATUS_OK;
}

/**
 * zapíše získaný login token do pomocného souboru
 */
int write_token(string login_token) {
    if (fs::exists(tokenfile)) {
        fs::remove(tokenfile);
    }
    ofstream ofile(tokenpath, ofstream::out);
    ofile << login_token;
    ofile.close();
    return STATUS_OK;
}

/**
 * pomocná funkce pro odeslání zprávy/příkazu na server a přijetí opdovědi,
 * v případě chybové odpovědi, vypíše její popis na stdout
 * @param command   příkaz
 * @return  nezpracovanou odpověď
 */
string cmdsend(string command) {
    regex lfesc("\n");
    command = regex_replace(command, lfesc, "\\n");
    int reply_buf_size;
    char buffer[RECV_BUF_SIZE];
    send(sock, command.c_str(), command.size(), 0);
    reply_buf_size = recv(sock, buffer, RECV_BUF_SIZE, 0);
    string buf = string(buffer);
    //cout << buffer << endl;
    if (buf.substr(1,2) == "ok") {
        cout << "SUCCESS: ";
    } else {
        cout << "ERROR: ";
        stringstream s;
        s.str(buf);
        s.seekg(6);
        getline(s, buf, '\"');
        cout << buf << endl;
        return "";
    }
    return buf;
}

/**
 * Načte a vrátí uložený token ze souboru
 * @return  token
 */
string read_token() {
    if (!fs::exists(tokenfile)) {
        return "";
    }
    ifstream ifile(tokenpath, ifstream::in);
    string tok;
    ifile >> tok;
    ifile.close();
    return tok;
}

/**
 * Ořeže z řetězce uvozovky (1. a poslední znak)
 */
string trimquotes(string input) {
    return input.substr(1,input.size()-2);
}

/**
 * Smaže soubor s přihlašovacím tokenem
 */
void destroy_token() {
    if (fs::exists(tokenfile)) {
        fs::remove(tokenfile);
    }
}