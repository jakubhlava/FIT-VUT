#include "main.hpp"

#define RUNTIME 10000
//#define SMARTSQSTART 40
#define INIT_EXPOSE 0

int main(int argc, char** argv) {
    cout << SEPARATOR << endl;
    cout << "\tCOVID-19 model simulator" << endl;
    cout << SEPARATOR << endl;
    int opt;
    string rulefile = "rules.txt", outfile = "out.csv";
    static struct option long_options[] = {
        {"rules", required_argument, 0, 'r' },
        {"out", required_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'}
    };
    while (true) {
        int index;
        opt = getopt_long(argc, argv, "r:o:h", long_options, &index);
        if (opt == -1) break;

        switch (opt) {
            case 'r':
                rulefile = optarg;
                break;
            case 'o': 
                outfile = optarg;
                break;
            default:
            case 'h':
                cout << "Usage: ./sim [-r/--rules <filename>] [-o/--out <filename>] [-h/--help]" << endl;
                cout << "\t-r --rules\t- Specifies file with simulation rules (default: rules.txt)" << endl;
                cout << "\t-o --out\t- Specifies output file for simulation result" << endl;
                cout << "\t-h --help\t- Shows this message" << endl << endl;
                cout << "Rulefile help:" << endl;
                cout << "\tRulefile consists of rules, one rule per line, rule names are UPPERCASE" << endl;
                cout << "Valid rules and parameters:" << endl;
                cout << "\tINIT <cases> - list of imported cases from abroad per day since day 0, numbers are separated with ' ' space" << endl;
                cout << "\tRUNTIME <days> - number of days for model to simulate" << endl;
                cout << "\tQUARANTINE <begin> <rise time> <max efficiency> - 'smart quarantine' function of model, which simulates tracing of contacts" << endl; 
                cout << "\t\t<begin> - number of days after model start when SQ was introduced" << endl; 
                cout << "\t\t<rise time> - how many days it takes to reach full efficiency of SQ" << endl; 
                cout << "\t\t<max efficiency> - maximal efficiency of smart quarantine" << endl;
                cout << "\tR0 <start> <end> <r0> - Base reproduction number R0 parameter to enable modelling of various strains" << endl;
                cout << "\t\t<start> and <end> specifies an  interval in days since start of simulation when this rule for R0 value is active" << endl;
                cout << "\t\t<r0> - float value of actual R0 in this interval" << endl;
                cout << "\tLOCKDOWN <start> <end> <efficiency> - user for model lockdowns and various restrictions against epidemic with specific effect" << endl;
                cout << "\t\t<start> and <end> specifies an interval in days since start of simulation during which those restrictions were applied" << endl;
                cout << "\t\t<efficiency> - float number which represents relative efficiency of lockdown" << endl;
                cout << SEPARATOR << endl;
                exit(0);
                break;
        }
    }
    cout << "Rules file: " << rulefile << endl;
    cout << "Output file: " << outfile << endl << endl;
    auto rules = load_rules(rulefile);
    cout << "Parameters are:" << endl;
    cout << "\tPopulation size: " << Pop << endl;
    cout << "\tSimulation duration: " << rules.runtime-SHIFT_OF_INIT << endl << endl;
    cout << "\tSmart quarantine starts on day: " << rules.quarantine_start-SHIFT_OF_INIT << endl;
    cout << "\tSmart quarantine rise time: " << rules.quarantine_rise_time << endl;
    cout << "\tSmart quarantine max efficiency: " << rules.quarantine_max_eff << endl << endl;
    cout << "\tR0 progress:" << endl;
    for (auto rrule : rules.r0_changes) {
        cout << "\t\tFrom day " << rrule.t_start-SHIFT_OF_INIT;
        if (rrule.t_end > 0) {
            cout << " to day " << rrule.t_end-SHIFT_OF_INIT; 
        } else {
            cout << " until the end of simulation";
        }
        cout << " R0 is " << rrule.value << endl;
    }
    cout << endl;
    cout << "\tRestrictions/lockdowns: " << endl;
    for (auto lockdown : rules.lockdowns) {
        cout << "\t\tFrom day " << lockdown.t_start-SHIFT_OF_INIT;
        if (lockdown.t_end > 0) {
            cout << " to day " << lockdown.t_end-SHIFT_OF_INIT ;
        } else {
            cout << " until the end of simulation";
        }
        cout << " with efficiency " << lockdown.eff << endl;
        } 
    cout << endl;
    cout << "\tInitialized by importing following cases per day from abroad: ";
    for (auto cases : rules.init_vector) {
        cout << cases << " ";
    }
    cout << endl;
    simulate(outfile, rules);
    return 0;
}

void simulate(string out_file, sim_rules rules) {
    string out_data("day,susceptible,diagnosed,infectious,recovered,dead,total_exposed,total_cases\n");
    vector<model_state> statelist;
    long totalexposed = 0;
    long totaldiagnosed = 0;
    model_state init_state = {
        .time = 0,
        .S = Pop,
        .E = {0, 0, 0, 0, 0},
        .I = 0,
        .IS = {0, 0, 0, 0, 0},
        .INT = {0, 0, 0, 0, 0},
        .INN = {0, 0, 0, 0, 0, 0, 0, 0, 0},
        .recovery = 0,
        .death = 0,
        .HCNT = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .HCR = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .HCHo = {0, 0, 0},
        .HoO = {0, 0},
        .HoSR = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .HoOICU = 0,
        .HoOR = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .HoICUR = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .HoICUD = {0, 0, 0, 0, 0, 0, 0},
        .Q = {0, 0, 0, 0, 0},
        .Diagnosed = 0
    };
    statelist.push_back(init_state);
    for (int t = 0; t < rules.runtime; t++) {
        model_state last_state = statelist.back();
        model_state new_state = statelist.back();
        // Eq. 1
        new_state.S = lround((double)last_state.S * (1 - (get_R0(t, rules) * (1 - get_EfLockdown(t, rules)) * (double)last_state.I) / (c24 * Pop)));
        int newexposed = lround((get_R0(t, rules) * (1 - get_EfLockdown(t, rules)) * (double)last_state.I * (double)last_state.S) / (c24 * Pop)); // Eq. 2
        if (t < rules.init_vector.size()) { // initialization via import
            new_state.S -= rules.init_vector[t]*3; // removing 3x cases because 66% are added to INN per base model description
            //newexposed += rules.init_vector[t];
        }
        totalexposed += newexposed;
        new_state.E.insert(new_state.E.begin(), newexposed); 
        new_state.E.pop_back(); // Eq. 3
        long is = lround((double)last_state.E.back() * (1 - c26) * (1 - get_SQEff(t, rules)));
        if (t < rules.init_vector.size()) is += rules.init_vector[t];
        new_state.IS.insert(new_state.IS.begin(), is); // Eq. 4
        new_state.IS.pop_back(); 
        for (int k = 1; k < 5; k++) { // Eq. 5
            new_state.IS[k] = last_state.IS[k - 1] * (1 - cd[k]);
        }
        new_state.INT.insert(new_state.INT.begin(), lround((double)last_state.E.back() * c26 * par * (1 - get_SQEff(t, rules)))); // Eq. 6
        new_state.INT.pop_back();
        for (int k = 1; k < 5; k++) {
            new_state.INT[k] = lround(last_state.INT[k - 1] * (1 - cd[k])); // Eq. 7
        }
        long inn = lround((double)last_state.E.back() * c26 * (1 - par) * (1 - get_SQEff(t, rules))); // Eq. 8
        if (t < rules.init_vector.size()) inn += rules.init_vector[t]*2;  // adding imports per base model description
        new_state.INN.insert(new_state.INN.begin(), inn);
        new_state.INN.pop_back();
        for (int k = 1; k < 9; k++) {
            new_state.INN[k] = lround((double)last_state.INN[k - 1] * (1 - dd[k])); // Eq. 9
        }
        
        new_state.Q.insert(new_state.Q.begin(), (double)last_state.E.back() * get_SQEff(t, rules)); // Eq. 10
        new_state.Q.pop_back(); // Eq. 11
        new_state.HCR.insert(new_state.HCR.begin(), lround((1 - c40) * (double)last_state.Diagnosed)); // Eq. 12
        new_state.HCR.pop_back(); // Eq. 13
        new_state.HCHo.insert(new_state.HCHo.begin(), lround(c40 * (double)last_state.Diagnosed)); // Eq. 14
        new_state.HCHo.pop_back(); // Eq. 15
        float sum = 0.;
        for (int k = 0; k < 9; k++) {
            sum += last_state.INN[k] * dd[k]; // Eq. 16
        }
        new_state.HCNT.insert(new_state.HCNT.begin(), lround(sum));
        new_state.HCNT.pop_back(); // Eq. 17
        new_state.HoO[0] = lround((double)last_state.HCHo[2] * (c41 / c40)); // Eq. 18
        new_state.HoO[1] = last_state.HoO[0]; // Eq. 19
        new_state.HoSR.insert(new_state.HoSR.begin(), lround((double)last_state.HCHo[2] * (1 - (c41 / c40)))); // Eq. 20
        new_state.HoSR.pop_back(); // Eq. 21
        new_state.HoOICU = lround((double)last_state.HoO[1] * (c42 / c41)); // Eq. 22
        new_state.HoOR.insert(new_state.HoOR.begin(), lround((double)last_state.HoO[1] * (1 - (c42 / c41)))); // Eq. 23
        new_state.HoOR.pop_back(); // Eq. 24
        new_state.HoICUR.insert(new_state.HoICUR.begin(), lround((double)last_state.HoOICU * (1 - (c43 / c42)))); // Eq. 25
        new_state.HoICUR.pop_back(); // Eq. 26
        new_state.HoICUD.insert(new_state.HoICUD.begin(), lround((double)last_state.HoOICU * (c43 / c42))); // Eq. 27
        new_state.HoICUD.pop_back(); // Eq. 28
        new_state.death = last_state.death + last_state.HoICUD.back(); // Eq. 29
        new_state.recovery = last_state.recovery + last_state.HoICUR.back() + last_state.HoOR.back() +
                             last_state.HCNT.back() + last_state.HoSR.back() + last_state.HCR.back(); // Eq. 30
        double IS_sum = 0, INT_sum = 0; 
        for (int k = 0; k < 4; k++) {
            IS_sum += (double)new_state.IS[k] * cd[k];
            INT_sum += (double)new_state.INT[k] * cd[k];
        }
        new_state.Diagnosed = new_state.Q.back() + lround(IS_sum) + new_state.IS.back() + lround(INT_sum) + new_state.INT.back(); // Eq. 31
        totaldiagnosed += new_state.Diagnosed;
        int IS_fsum = 0, INT_fsum = 0, INN_sum = 0;
        for (int k = 0; k < 5; k++) {
            IS_fsum += new_state.IS[k];
            INT_fsum += new_state.INT[k];
        }
        for (int k = 0; k < 9; k++) {
            INN_sum += new_state.INN[k];
        }
        new_state.I = IS_fsum + INT_fsum + INN_sum; // Eq. 32
        statelist.push_back(new_state);
        if (t>SHIFT_OF_INIT) out_data += to_string(t-SHIFT_OF_INIT)+","+to_string(new_state.S)+","+to_string(new_state.Diagnosed)+"," +to_string(new_state.I)+","+to_string(new_state.recovery)+","+to_string(new_state.death)+","+to_string(totalexposed)+","+to_string(totaldiagnosed)+"\n";
   }
   //cout << out_data << endl;
   cout << SEPARATOR << endl;
    cout << "After this run there will be:" << endl;
    cout << totalexposed << " people who were exposed to virus" << endl;
    cout << totaldiagnosed << " people who were diagnosed with COVID-19 by state authorities" << endl;
    cout << statelist.back().recovery << " people who recovered from COVID-19" << endl;
    cout << statelist.back().death << " dead people" << endl;
    cout << SEPARATOR << endl;
    ofstream outf(out_file);
    outf << out_data;
    outf.close();
}

/**
 * Returns R0 for given day t per rules specified in rules
 */
float get_R0(int t, sim_rules rules) {
    for (auto r0 : rules.r0_changes) {
        if (t >= r0.t_start) {
            if (r0.t_end == -1 || t <= r0.t_end) {
                return r0.value;
            }
        }
    } 
    return c27;
}

/**
 * Returns estimated lockdown efficiency for given day t per rules specified in rules
 */
float get_EfLockdown(int t, sim_rules rules) {
    for (auto l : rules.lockdowns) {
        if (t >= l.t_start) {
            if (l.t_end == -1 || t <= l.t_end) {
                return l.eff;
            }
        }
    }
    return 0.0;
}

/**
 * Returns samrt quarantine efficiency base don rules, day t and equation 33 of Model A
 */
float get_SQEff(int t, sim_rules rules) {
    if (t < rules.quarantine_start) return 0;
    if (t > rules.quarantine_start+rules.quarantine_rise_time) return rules.quarantine_max_eff;
    return ((double)(1+t-rules.quarantine_start)/(1+rules.quarantine_rise_time)) * rules.quarantine_max_eff; 
}

/**
 * Loads and parses rules from file specified in filename
 */
sim_rules load_rules(string filename) {
    sim_rules rules;
    ifstream conf_file(filename);
    string cline;
    while (getline(conf_file, cline, '\n')) {
        stringstream linestream(cline);
        string command;
        getline(linestream, command, ' ');
        if (command == "INIT") {
            string param;
            while(getline(linestream, param, ' ')) {
                rules.init_vector.push_back(stol(param));
            }
        } else if (command == "LOCKDOWN") {
            lockdown l;
            string start, stop, eff;
            getline(linestream, start, ' ');      
            getline(linestream, stop, ' '); 
            getline(linestream, eff);
            try {
                l.t_start = start != "X" ? stol(start)+SHIFT_OF_INIT : -1;
                l.t_end = stop != "X" ? stol(stop)+SHIFT_OF_INIT : -1;
                l.eff = stof(eff);
            } catch (...) {
                cerr << "Error in your rules file near LOCKDOWN setting" << endl;
                exit(1);
            }
            rules.lockdowns.push_back(l);
        } else if (command == "R0") {
            r0_rule r;
            string start, stop, r0;
            getline(linestream, start, ' ');      
            getline(linestream, stop, ' '); 
            getline(linestream, r0);
            try {
                 r.t_start = start != "X" ? stol(start)+SHIFT_OF_INIT : -1;
                r.t_end = stop != "X" ? stol(stop)+SHIFT_OF_INIT : -1;
                r.value = stof(r0);
            } catch (...) {
                cerr << "Error in your rules file near R0 setting" << endl;
                exit(1);
            }
            rules.r0_changes.push_back(r);
        } else if (command == "QUARANTINE") {
            string start, time_to_full, maxeff;
            getline(linestream, start, ' ');
            getline(linestream, time_to_full, ' ');
            getline(linestream, maxeff);
            try {
                rules.quarantine_start = stol(start)+SHIFT_OF_INIT;
                rules.quarantine_rise_time = stol(time_to_full);
                rules.quarantine_max_eff = stof(maxeff);
            } catch (...) {
                cerr << "Error in your rules file near QUARANTINE setting" << endl;
                exit(1);
            }
        } else if (command == "RUNTIME") {
            string runtime;
            getline(linestream, runtime);
            try {
                rules.runtime = stol(runtime) + SHIFT_OF_INIT;
            } catch (...) {
                cerr << "Error in your rules file near RUNTIME setting" << endl;
                exit(1);
            }
            
        }
    }
    return rules;
}