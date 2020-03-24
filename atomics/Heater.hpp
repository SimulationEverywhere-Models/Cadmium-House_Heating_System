/* Heater.HPP*/

#ifndef _HEATER_HPP__
#define _HEATER_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
using namespace cadmium;
using namespace std;


//Port definition
struct Heater_defs{
    struct HGOut : public out_port<float> {};
//    struct TroomIn : public in_port<float> {};
	struct ConIn : public in_port<float> {};
};
template<typename TIME> class Heater{
    public:
	        TIME   preparationTime;
    // ports definition
	
        using input_ports=tuple<typename Heater_defs::ConIn>;
	    using output_ports=tuple<typename Heater_defs::HGOut>;
	
	 Heater() noexcept{
          preparationTime  = TIME("00:00:01:000");
          state.next_internal    = std::numeric_limits<TIME>::infinity();
          state.model_active     = false;
        }
    // state definition
    struct state_type{
		bool model_active;
        bool sending;
		int index;
        float HG;
        float Troom;
		float Con;
		TIME next_internal;
    }; 
    state_type state; 	

 // internal transition
    void internal_transition() {
        state.sending = false;  
    }	

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
         for(const auto &x : get_messages<typename Heater_defs::ConIn>(mbs)){
         state.Con = x;
                if(state.model_active == false){
                        state.sending = true;
                        state.model_active = true;
                        state.next_internal = TIME("00:00:01:000");
                    }
                }

                 if(state.Con == 1)   {
                        state.HG = 0.5;
                        
				}
                 if(state.Con != 1)   {
                        state.HG = 0;
                        
				} 				
	}			






	
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
	
    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
	    float new1;
			new1 = (state.HG);
        get_messages<typename Heater_defs::HGOut>(bags).push_back(new1);
        return bags;
    }
    // time_advance function
    TIME time_advance() const {
		
      
        return TIME("00:00:01:000");
    }

    friend ostringstream& operator<<(ostringstream& os, const typename Heater<TIME>::state_type& i) {
        os << "index: " << i.HG; 
        return os;
    }
};    
#endif // _TRANSDUC_HPP__
	
	


	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	