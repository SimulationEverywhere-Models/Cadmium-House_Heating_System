//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>


//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/Heater.hpp"
#include "../atomics/Thermo.hpp"
#include "../atomics/Transduc.hpp"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

// Define input port for coupled models 
struct in_T1 : public in_port<float>{};
struct in_T2 : public in_port<float>{};

// Define output ports for coupled model 
struct outp_out1 : public out_port<float>{}; 
struct outp_out2 : public out_port<float>{}; 


/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<float,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<float,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));

    shared_ptr<dynamic::modeling::model> transduc1 = dynamic::translate::make_dynamic_atomic_model<Transduc, TIME>("transduc1");
    shared_ptr<dynamic::modeling::model> heater1 = dynamic::translate::make_dynamic_atomic_model<Heater, TIME>("heater1");
    shared_ptr<dynamic::modeling::model> thermo1 = dynamic::translate::make_dynamic_atomic_model<Thermo, TIME>("thermo1");
	////////////////////////////
	///////// House model 
	
    dynamic::modeling::Ports iports_House = {typeid(in_T1)};
    dynamic::modeling::Ports oports_House = {typeid(outp_out1),typeid(outp_out2)};
    dynamic::modeling::Models submodels_House = {thermo1, heater1};
    dynamic::modeling::EICs eics_House = {
		cadmium::dynamic::translate::make_EIC<in_T1, Thermo_defs::HLIn>("thermo1")
	};
    dynamic::modeling::EOCs eocs_House = {
        dynamic::translate::make_EOC<Thermo_defs::TroomOut,outp_out1>("thermo1"),
		dynamic::translate::make_EOC<Thermo_defs::TroomOut_final,outp_out2>("thermo1")
    };
    dynamic::modeling::ICs ics_House = {
		dynamic::translate::make_IC<Thermo_defs::ConOut, Heater_defs::ConIn>("thermo1","heater1"),
			dynamic::translate::make_IC<Heater_defs::HGOut, Thermo_defs::HGIn>("heater1","thermo1")
	};
    shared_ptr<dynamic::modeling::coupled<TIME>> HOUSE;
    HOUSE = make_shared<dynamic::modeling::coupled<TIME>>(
        "HOUSE", submodels_House, iports_House, oports_House, eics_House, eocs_House, ics_House
    );

///////////////////////////////////////////
  //HOUSETOP SIMULATOR COUPLED MODEL
    dynamic::modeling::Ports iports_HOUSETOP = {typeid(in_T2)};
    dynamic::modeling::Ports oports_HOUSETOP = {typeid(outp_out2)};
    dynamic::modeling::Models submodels_HOUSETOP = {transduc1, HOUSE};
    dynamic::modeling::EICs eics_HOUSETOP = {
        cadmium::dynamic::translate::make_EIC<in_T2, Transduc_defs::ToutIn>("transduc1")
    };
    dynamic::modeling::EOCs eocs_HOUSETOP = {
        dynamic::translate::make_EOC<outp_out2,outp_out2>("HOUSE")
    };
    dynamic::modeling::ICs ics_HOUSETOP = {
        dynamic::translate::make_IC<Transduc_defs::HeatlossOut, in_T1>("transduc1","HOUSE"),
        dynamic::translate::make_IC<outp_out1, Transduc_defs::TroomIn>("HOUSE","transduc1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> HOUSETOP;
    HOUSETOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "HOUSETOP", submodels_HOUSETOP, iports_HOUSETOP, oports_HOUSETOP, eics_HOUSETOP, eocs_HOUSETOP, ics_HOUSETOP 
    );
///////////////////////////////////////////////

	 /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outp_out2)};
    dynamic::modeling::Models submodels_TOP = {input_reader, HOUSETOP};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<outp_out2,outp_out2>("HOUSETOP")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<float>::out, in_T2>("input_reader","HOUSETOP")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/HOUSETOP_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/HOUSETOP_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("00:10:00:000"));
    return 0;
}
