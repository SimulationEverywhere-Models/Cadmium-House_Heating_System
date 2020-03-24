/* THERMO.CPP */	

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
#include "../atomics/Thermo.hpp"

//C++ libraries 
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/ 
struct inp_HGIn : public cadmium::in_port<float>{};
struct inp_HLIn : public cadmium::in_port<float>{};


/***** Define output ports for coupled model *****/
struct outp_TroomOut : public cadmium::out_port<float>{};
struct outp_ConOut : public cadmium::out_port<float>{};
struct outp_Troom_final : public cadmium::out_port<float>{};


/****** Input Reader atomic model declaration *******************/

 template<typename T>
class InputReader_TroomIn_t : public iestream_input<float,T> {
    public:
        InputReader_TroomIn_t () = default;
        InputReader_TroomIn_t (const char* file_path) : iestream_input<float,T>(file_path) {}
};

 template<typename T>
class InputReader_HGIn_t : public iestream_input<float,T> {
    public:
        InputReader_HGIn_t () = default;
        InputReader_HGIn_t (const char* file_path) : iestream_input<float,T>(file_path) {}
};

 template<typename T>
class InputReader_HLIn_t : public iestream_input<float,T> {
    public:
        InputReader_HLIn_t () = default;
        InputReader_HLIn_t (const char* file_path) : iestream_input<float,T>(file_path) {}
};


int main(){

    /****** Input Reader atomic model instantiation *******************/
	
    const char * i_input_data_HGIn = "../input_data/Thermo_HG_In.txt";
    shared_ptr<dynamic::modeling::model> input_reader_HGIn = dynamic::translate::make_dynamic_atomic_model<InputReader_HGIn_t, TIME, const char* >("input_reader_HGIn" , move(i_input_data_HGIn));
	
	const char * i_input_data_HLIn = "../input_data/Thermo_HL_In.txt";
    shared_ptr<dynamic::modeling::model> input_reader_HLIn = dynamic::translate::make_dynamic_atomic_model<InputReader_HLIn_t, TIME, const char* >("input_reader_HLIn" , move(i_input_data_HLIn));

	
	    /****** transduc atomic model instantiation *******************/
        shared_ptr<dynamic::modeling::model> thermo1 = dynamic::translate::make_dynamic_atomic_model<Thermo, TIME>("thermo1");
	
	    /*******TOP MODEL********/
	dynamic::modeling::Ports iports_TOP = {};

    dynamic::modeling::Ports oports_TOP = {typeid(outp_TroomOut), typeid(outp_ConOut),typeid(outp_Troom_final)};

    dynamic::modeling::Models submodels_TOP = {input_reader_HGIn, input_reader_HLIn, thermo1};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<Thermo_defs::TroomOut,outp_TroomOut>("thermo1"),
        dynamic::translate::make_EOC<Thermo_defs::ConOut,outp_ConOut>("thermo1"),
		dynamic::translate::make_EOC<Thermo_defs::TroomOut_final,outp_Troom_final>("thermo1")
	};
	
    dynamic::modeling::ICs ics_TOP = {
		dynamic::translate::make_IC<iestream_input_defs<float>::out,Thermo_defs::HGIn>("input_reader_HGIn","thermo1"),
		dynamic::translate::make_IC<iestream_input_defs<float>::out,Thermo_defs::HLIn>("input_reader_HLIn","thermo1"),
       
	};

	
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/Thermo_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/Thermo_test_output_state.txt");
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
    r.run_until(NDTime("00:00:10:000"));
    return 0;
}
	



	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	















































