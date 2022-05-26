// AUTHOR: Jakub Hlava (xhlava52)

#include "main.hpp"

// Options pro getopt
static struct option long_options[] = {
    {"interface", optional_argument, 0, 'i'},
    {"tcp", no_argument, 0, 't'},
    {"udp", no_argument, 0, 'u'},
    {"arp", no_argument, 0, 0},
    {"icmp", no_argument, 0, 0}
};

int main(int argc, char* argv[]) {
    bool tcpf = false, udpf = false, arpf = false, icmpf = false;   // přepínače filtrů
    int port = 0;                                                   // port pro filtrování (nebo 0 - pak je filtrace podle portu vypnutá)
    int packets = 1;                                                // počet přijatých paketů
    string interface = "";                                          // rozhraní, na kterém bude odchytávána komunikace
    bool print_interfaces = true;                                   // přepínač, zda program pouze vytiskne dsotupná rozhraní nebo bude chytat pakety
    vector<string> devs;                                         // Vektor dostupných rozhraní
    // Pomocné proměnné
    int result;
    pcap_if_t* devs_list;

    // Načtení dostupných rozhraní
    int dev_lookup_result = pcap_findalldevs(&devs_list, errbuf);
    if (dev_lookup_result == PCAP_ERROR) {
        cerr << "No interfaces detected" << endl;
        return 1;
    }
    pcap_if_t* temp = devs_list;
    while (temp != NULL) {
        devs.push_back(temp->name);
        temp = temp->next;
    }
    pcap_freealldevs(devs_list);
    
    int opt;
    int longopt;
    // Speciální ošetření -i a --interface, kvůli limitacím getopt
    for (int i = 0; i < argc; i++) { 
        if ((string)argv[i] == "-i" || (string)argv[i] == "--interface") {
            if (i+1 < argc) {
                string next = argv[i+1];
                if (next[0] != '-') {
                    interface = next;
                    print_interfaces = false;
                } 
            }
            break;
        }
    }

    // Zpracování vstupních parametrů z příkazové řádky
    while (true) {
        opt = getopt_long(argc, argv, "i::p:tun:", long_options, &longopt);
        if (opt == -1) break;

        switch (opt) {
            case 0:
                if (long_options[longopt].name == "tcp") {
                    tcpf = true;
                } else if (long_options[longopt].name == "udp") {
                    udpf = true;
                } else if (long_options[longopt].name == "arp") {
                    arpf = true;
                } else if (long_options[longopt].name == "icmp") {
                    icmpf = true;
                }
            break;
            case 'p':
                port = strtod(optarg, NULL);
            break;
            case 't':
                tcpf = true;
            break;
            case 'u':
                udpf = true;
            break;
            case 'n':
                packets = strtod(optarg, NULL);
            break;
        }
    }

    // Pokud nebylo načteno rozhraní v parametrech, pak se tiskne jejich seznam
    if (print_interfaces) {
        cout << "List of interfaces: " << endl;
        for (auto iface : devs) {
            cout << iface << endl;
        }
    } else { // jinak se provede odchytávání paketů
        
        // Zpracování filter programu pro libpcap
        string filter = "";
        if (port != 0 && (tcpf || udpf)) {
            filter += FILTER_PORT(port);
        }
        if (!tcpf && !udpf && !icmpf && !arpf) {
            tcpf = udpf = icmpf = arpf = true;
        } else {
            bool first = true;
            if (port != 0 && (tcpf || udpf)) {
                filter += " and (";
            }
            if (tcpf) {
                if (!first) filter += " or ";
                else first = false;
                filter += FILTER_TCP;
            } 
            if (udpf) {
                if (!first) filter += " or ";
                else first = false;
                filter += FILTER_UDP;
            }
            if (port != 0 && (tcpf || udpf)) {
                filter += ")";
            }
            if (arpf) {
                if (!first) filter += " or ";
                else first = false;
                filter += FILTER_ARP;
            }
            if (icmpf) {
                if (!first) filter += " or ";
                else first = false;
                filter += FILTER_ICMP;
            }
            
        }
        // Pomocný výpis
        #ifdef DEBUG
        cout << "filter: " << filter << endl;

        cout << "Interface: " << interface << endl;
        cout << "Print interfaces: " << print_interfaces << endl;
        cout << "Port filter: " << port << endl;
        cout << "Packet count: " << packets << endl;
        cout << "TCP Filter: " << tcpf << endl;
        cout << "UDP Filter: " << udpf << endl;
        cout << "ARP Filter: " << arpf << endl;
        cout << "ICMP Filter: " << icmpf << endl;
        #endif

        // Ovaření, zda zadané rozhraní je platné (podle předem načteného seznamu)
        if (!is_valid_iface(interface, devs)) {
            cerr << interface << " is not a valid interface!" << endl;
            return 1;
        }

        // Zahájení odchytávání
        auto pcap = pcap_open_live(interface.c_str(), 65535, 1, 100, errbuf);
        if (pcap == NULL) {
            cerr << "Cannot create pcap handle: " << errbuf << endl;
            return 1;
        }

        // VCytvoření a připojení filter programu k pcap objektu
        bpf_program filter_prog;
        pcap_compile(pcap, &filter_prog, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN);
        pcap_setfilter(pcap, &filter_prog);
        
        pcap_pkthdr* header;
        const u_char* data;
        for (int i = 0; i < packets; i++) { 
            result = pcap_next_ex(pcap, &header, &data);
            if (result == 1) {
                string timestr = make_timestring(header->ts); 
                // předpokládám Ethernet Type II frame, 6 bytu dest, 6 bytu src, 2byty ethtype
                uint16_t ethType = *(data+ETHER_PROTO_OFFSET) << 8 | *(data+ETHER_PROTO_OFFSET+1);

                int proto;              // IP protokol
                int IHLoff = 0;         // IPv4 IHL offset
                string srcaddr = "";    // zdrojová IP adresa paketu
                string destaddr = "";   // cílová IP adresa paketu
                pair<int,int> ports;    // zdrojový - cílový port, pokud se jedná o TCP nebo UDP protokol
                bool port = false;       // přepínač výpisu portu
                switch (ethType) {      // získání dat z hlaviček paketů podle detekovaného protokolu
                    case ETHER_IPV4:
                        srcaddr = extract_ip(data, 4, IPV4_SRC_OFFSET);
                        destaddr = extract_ip(data, 4, IPV4_DEST_OFFSET);
                        proto = *(data+ETHER_HEAD_SIZE+IPV4_PROTO_OFFSET);
                        IHLoff = *(data+ETHER_HEAD_SIZE) >> 4;
                        if (proto == PROTO_TCP || proto == PROTO_UDP) {
                            ports = extract_ports(data, 4, IHLoff);
                            port = true;
                        }
                        #ifdef DEBUG    // ladící výpisy
                        cout << "Recvd IPv4 packet, proto: ";
                        cout << hex << setw(2) << setfill('0') << proto;
                        #endif
                        break;
                    case ETHER_IPV6:
                        srcaddr = extract_ip(data, 6, IPV6_SRC_OFFSET);
                        destaddr = extract_ip(data, 6, IPV6_SRC_OFFSET);
                        proto = *(data+ETHER_HEAD_SIZE+IPV6_PROTO_OFFSET);
                        if (proto == PROTO_TCP || proto == PROTO_UDP){
                            port = true;
                            ports = extract_ports(data, 6);
                        } 
                        #ifdef DEBUG
                        cout << "Recvd IPv6 packet, proto: ";
                        cout << hex << setw(2) << setfill('0') << proto;
                        #endif
                        break;
                    case ETHER_ARP:
                        #ifdef DEBUG
                        cout << "Recvd ARP packet";
                        #endif
                        srcaddr = extract_arp(data, ETHER_SRC_OFFSET);
                        destaddr = extract_arp(data, ETHER_DEST_OFFSET);
                        break;
                    default:
                        #ifdef DEBUG
                        cout << "Recvd UNDETECTABLE TYPE packet (" << hex << ethType << ")" << endl;
                        #endif
                        break;
                }
                
                #ifdef DEBUG
                cout << endl;
                #endif
                // Hlavička výpisu
                cout << timestr << " "
                     << srcaddr;
                     if (port) cout << " : " << dec << ports.first;
                     cout << " > " << destaddr;
                     if (port) cout << " : " << dec << ports.second;
                     cout << ", length " << dec << header->caplen << " bytes" << endl;    
                int lines = header->caplen / LINE_LENGHT; // počet řádků po 80 bytech
                int lastlen = header->caplen - (lines)*LINE_LENGHT; // počet bytů posledního řádku

                cout << endl;

                for (int L = 0; L < lines; L++) { // výpis paketu
                    char left[7];
                    snprintf(left, 7, "%04x", L*LINE_LENGHT);
                    cout << "0x" << left << ":  ";
                    int width;
                    (L == lines-1) ? width = lastlen+1 : width = LINE_LENGHT; // zarovnání posledního řádku, ochrana proti segfault
                    for (int B = 0; B < LINE_LENGHT; B++) {
                        if (L == lines-1 && B > lastlen) { // zarovnání posledního řádku
                            cout << "   ";
                        } else {
                            cout << hex << setw(2) << setfill('0') << (int)(data[L*LINE_LENGHT+B]) << " ";
                        }
                        
                        if (B == SPLIT_POS) {
                            cout << " ";
                        }
                    }
                    cout << "  ";
                    for (int B = 0; B < width; B++) {
                        char to_print = (char)(data[L*LINE_LENGHT+B]);
                        if (to_print < 32 || to_print > 126) { // nahrazení netisknutelných znaků tečkou
                            to_print = '.';
                        }
                        cout << to_print;
                        if (B == SPLIT_POS) {
                            cout << " ";
                        }
                    }
                    cout << endl;
                }
                cout << endl;

            } else {
                cerr << "Error code " << dec << result << endl;
                break;
            }
        }
        pcap_freecode(&filter_prog);
        pcap_close(pcap);
    }
    
    return 0;
}

/**
 * Ověření existence rozhraní
 * @param iface     Ověřované rozhraní
 * @param valids    Seznam platných rozhraní
 */
bool is_valid_iface(string iface, vector<string> valids) {
    bool valid = false;
    for (auto i : valids) {
        if (i == iface) {
            valid = true;
            break;
        }
    }
    return valid;
}

/**
 * Složení řetězce s časovým údajem dle požadavků zadání
 * @param   conver_time čas ke konverzi na řetězec
 */
string make_timestring(timeval conver_time) {
    time_t now = conver_time.tv_sec;
    tzset();
    auto localnow = localtime(&now);
    char out[64];
    strftime(out, 64, "%Y-%m-%dT%H:%M:%S", localnow);
    string timestr = out;
    timestr += '.'+to_string((int)conver_time.tv_usec/1000);
    char gmtoffset[7];
    int hour_off = localnow->tm_gmtoff / 3600;
    int min_off = abs((localnow->tm_gmtoff - hour_off*3600) / 60);
    snprintf(gmtoffset, 7, "%+03d:%02d", hour_off, min_off);
    timestr += gmtoffset;
    return timestr;
}

/**
 * Získá IP adresu z dat ethernetového paketu a převede ji na požadovaný tvar pro zobrazení
 * @param   data    binární data paketu
 * @param   version verze protokolu IP, 4 nebo 6
 * @param   offset  jeden z offsetů z hlavičkového souboru main.hpp, určující pozici IP v paketu (source/destination)
 */
string extract_ip(const u_char* data, int version, int offset) {
    stringstream out;
    if (version == 4) {
        for (int i = 0; i < 4; i++) { // 4 byty IPv4 adresy
            out << to_string((int)(*(data+ETHER_HEAD_SIZE+offset+i)));
            if (i < 3) out << "."; // aby nevznikl oddělovač za posledním číslem
        }
    } else if (version == 6) {
        for (int i = 0; i < 16; i+=2) { // 16 bytu IPv6 adresy
            uint16_t ippart = *(data+ETHER_HEAD_SIZE+offset+i) << 8 | *(data+ETHER_HEAD_SIZE+offset+i+1);
            out << hex << setfill('0') << (int)(ippart);
            if (i < 14) out << ":";
        }
    }
    return out.str();
}

/**
 * Získá porty z hlavičky TCP nebo UDP paketu
 * @param   data        binární dat paketu
 * @param   version     verze protokolu IP, 4 nebo 6
 * @param   ihloffset   v případě IPv4 hodnota pole IHL, jinak 0
 */
pair<int, int> extract_ports(const u_char* data, int version, int ihloffset) {
    pair<int, int> ports;
    int p_offset;
    if (version == 4) p_offset = ETHER_HEAD_SIZE+IPV4BASE_HEAD_SIZE; 
    if (ihloffset > 5) p_offset += (ihloffset*4-IPV4BASE_HEAD_SIZE); // *4 => ziskam z IHL velikost headeru v bytech
    else if (version == 6) p_offset = ETHER_HEAD_SIZE+IPV6_HEAD_SIZE;
    uint16_t srcport = data[p_offset] << 8 | data[p_offset+1];
    uint16_t destport = data[p_offset+2] << 8 | data[p_offset+3];
    ports = {(int)srcport, (int)destport};
    return ports;
}

/**
 * Získá a zformátuje MAC adresy z ethernetového rámce pro účely výpisu ARP paketu
 * @param   data    binární data paketu
 * @param   offset  jeden z offsetů z hlavičkového souboru main.hpp, určující source/destination
 */
string extract_arp(const u_char* data, int offset) {
    stringstream out;
    for (int i = 0; i < 6; i++) {
        out << hex << setw(2) << setfill('0') << (int)data[offset+i];
        if (i < 5) out << ":";
    } 
    return out.str();
}