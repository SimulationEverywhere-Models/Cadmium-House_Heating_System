/* TRANSDUC.CPP */	

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
#include "../atomics/Transduc.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct inp_Troom : public cadmium::in_port<float>{};
struct inp_Tout : public cadmium::in_port<float>{};

/***** Define output ports for coupled model *****/
struct outp_Heatloss : public cadmium::out_port<float>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Tout_t : public iestream_input<float,T> {
    public:
        InputReader_Tout_t () = default;
        InputReader_Tout_t (const char* file_path) : iestream_input<float,T>(file_path) {}
};

template<typename T>
class InputReader_Troom_t : public iestream_input<float,T> {
    public:
        InputReader_Troom_t () = default;
        InputReader_Troom_t (const char* file_path) : iestream_input<float,T>(file_path) {}
};

int main(){

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input_data_Tout = "../input_data/Transduc_Tout_In.txt";
    shared_ptr<dynamic::modeling::model> input_reader_Tout = dynamic::translate::make_dynamic_atomic_model<InputReader_Tout_t, TIME, const char* >("input_reader_Tout" , move(i_input_data_Tout));
	
    const char * i_input_data_Troom = "../input_data/Transduc_Troom_In.txt";
    shared_ptr<dynamic::modeling::model> input_reader_Troom = dynamic::translate::make_dynamic_atomic_model<InputReader_Troom_t , TIME, const char* >("input_reader_Troom" , move(i_input_data_Troom));
	
	    /****** transduc atomic model instantiation *******************/
        shared_ptr<dynamic::modeling::model> transduc1 = dynamic::translate::make_dynamic_atomic_model<Transduc, TIME>("transduc1");
	
	    /*******TOP MODEL********/
	dynamic::modeling::Ports iports_TOP = {};
	dynamic::modeling::Ports oports_TOP = {typeid(outp_Heatloss)};
    dynamic::modeling::Models submodels_TOP = {input_reader_Tout, input_reader_Troom, transduc1};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
    dynamic::translate::make_EOC<Transduc_defs::HeatlossOut,outp_Heatloss>("transduc1"),
	};
	
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<float>::out,Transduc_defs::ToutIn>("input_reader_Tout","transduc1"),
        dynamic::translate::make_IC<iestream_input_defs<float>::out,Transduc_defs::TroomIn>("input_reader_Troom","transduc1"),
	};
	
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/Transduc_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/Transduc_test_output_state.txt");
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
	



	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	















































