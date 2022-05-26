#ifndef __IMS_MAIN_H__
#define __IMS_MAIN_H__
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <cmath>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// costant definitions
#define Pop 10647720 
#define c26     0.5     // share of asymptomatic
#define par     0.3     // asymptomatic treated as symptomatic
#define c27     2.5     // R0
#define c24     5       // infection period
#define c40     0.15    // Share: hospital given diagnosis 
#define c41     0.045   // Share: oxygen given diagnosis
#define c42     0.03    // Share: ventilation given diagnosis
#define c43     0.02   // Death rate (sourced from MZCR, death rate on 21.12.2020)
#define ced1    0.6     // max effectiveness of smart Q
#define sqspeed 21
#define EfLockdown  0.85

#define SHIFT_OF_INIT 8 // shift of imported cases in days (value per base model description)

const float cd[5] = {0.05, 0.1, 0.15, 0.3, 0.4};
const float dd[9] = {0.05, 0.05, 0.05, 0.05, 0.1, 0.1, 0.1, 0.2, 0.3};


typedef struct model_state {

    long time;                   // T    - model time
    long S;            // S    - S individuals
    vector<long> E;             // E    - E individuals
    long I;                 // I(nfectious)    - I
    vector<long> IS;            // IS   - symptomatic individuals
    vector<long> INT;         // INT  - asymptomatic individuals tested
    vector<long> INN;        // INN  - asymptomatic individuals non-tested
    long recovery;               // R    - Recovered
    long death;                  // D    - dead
    vector<long> HCNT;         // HCNT - homecare aympotmatic tested
    vector<long> HCR;    // HCR  - homecare will recover
    vector<long> HCHo;    // HCHo - homecare will need hospital
    vector<long> HoO;       // HoO  - hospital will need oxygen
    vector<long> HoSR;  // HoSR - hopsital will recover
    long HoOICU;         // HoOICU - hospital oxygen will need ICU
    vector<long> HoOR;       // HoOR - hospital oxygen will recover
    vector<long> HoICUR;       // HoICUR - hospital ICU will recover
    vector<long> HoICUD;      // HoICUD - hospital ICU will die
    vector<long> Q;             // Q - qurantined
    long Diagnosed;

} model_state;

typedef struct lockdown {
    int t_start;
    int t_end;
    float eff;
} lockdown_rule;

typedef struct R0 {
    int t_start;
    int t_end;
    float value;
} r0_rule;

typedef struct simulation {
    long runtime;
    vector<lockdown_rule> lockdowns;
    vector<r0_rule> r0_changes;
    long quarantine_start;
    long quarantine_rise_time;
    float quarantine_max_eff;
    vector<long> init_vector;
} sim_rules;

float get_EfLockdown(int t, sim_rules rules);
float get_SQEff(int t, sim_rules rules);
void simulate(string filename, sim_rules rules);
float get_R0(int t, sim_rules rules);

sim_rules load_rules(string filename="rules.txt");

// TUI helpers
#define SEPARATOR "==============================================================================="

#endif

