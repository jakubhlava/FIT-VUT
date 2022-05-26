// AUTHOR: Jakub Hlava (xhlava52)

#ifndef __MAIN_H__
#define __MAIN_H__

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <pcap/pcap.h>
#include <iomanip>

using namespace std;

// Konfigurace výpisu paketu
#define LINE_LENGHT 16
#define SPLIT_POS 7

// Identifikátory protokolu v Ethernet rámci
#define ETHER_IPV4  0x0800
#define ETHER_IPV6  0x86DD
#define ETHER_ARP   0X0806

// Velikosti a offsety jednotlivých "zajímavých" informací v paketech
#define ETHER_HEAD_SIZE     14
#define ETHER_PROTO_OFFSET  12
#define IPV4_SRC_OFFSET     12
#define IPV4_DEST_OFFSET    16
#define IPV4_PROTO_OFFSET   9
#define IPV6_SRC_OFFSET     8
#define IPV6_DEST_OFFSET    24
#define IPV6_PROTO_OFFSET   6
#define ETHER_SRC_OFFSET    6
#define ETHER_DEST_OFFSET   0

// Velikosti paketů
#define IPV4BASE_HEAD_SIZE  20  // základní velikost při nepoužití IHL
#define IPV6_HEAD_SIZE      40

// Kódy protokolů v hlavičce IP paketu
#define PROTO_TCP       0X06
#define PROTO_UDP       0x11
#define PROTO_ICMP      0X02
#define PROTO_ICMPv6    0x3A

// Části filter programů pro libpcap
#define FILTER_TCP "ip proto \\tcp or ip6 proto \\tcp"      // filtrace TCP
#define FILTER_UDP "ip proto \\udp or ip6 proto \\udp"      // filtrace UDP
#define FILTER_ICMP "icmp or icmp6"                         // filtrace ICMP
#define FILTER_ARP "ether proto \\arp"                      // filtrace ARP rámců
#define FILTER_PORT(x) "port "+to_string(x)                 // filtrace určitého portu

//#define DEBUG

char errbuf[PCAP_ERRBUF_SIZE];                                                      // pomocný buffer pro chybová hlášení z libpcap

int main(int argc, char* argv[]);
bool is_valid_iface(string iface, vector<string> valids);                        // kontrola, zda zadané rozhraní existuje
string make_timestring(timeval conver_time);                                        // převod času ve formátu z paketu na požadovaný formát
string extract_ip(const u_char* data, int version, int offset);                     // získá IP adresu podle jejího offsetu uvnitř ethernetového rámce a verze IP                 
pair<int, int> extract_ports(const u_char* data, int version, int ihloffset=0);     // získá porty z TCP a UDP paketů
string extract_arp(const u_char* data, int offset);                                 // získá MAC z ethernet rámce

#endif