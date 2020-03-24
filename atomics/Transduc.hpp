/* Transduc.HPP*/

#ifndef _TRANSDUC_HPP__
#define _TRANSDUC_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
using namespace cadmium;
using namespace std;


//Port definition
struct Transduc_defs{
    struct HeatlossOut : public out_port<float> {};
    struct TroomIn : public in_port<float> {};
	struct ToutIn : public in_port<float> {};
};
template<typename TIME> class Transduc{
    public:
	        TIME   preparationTime;
    // ports definition
	
        using input_ports=tuple<typename Transduc_defs::TroomIn, typename Transduc_defs::ToutIn>;
	    using output_ports=tuple<typename Transduc_defs::HeatlossOut>;
	
	 Transduc() noexcept{
          preparationTime  = TIME("00:00:01:000");
          state.next_internal    = std::numeric_limits<TIME>::infinity();
          state.model_active     = false;
        }
    // state definition
    struct state_type{
		bool model_active;
        bool sending;
		int index;
        float Heatloss;
        float Troom=25;
		float Tout;
		TIME next_internal;
    }; 
    state_type state; 	

 // internal transition
    void internal_transition() {
        state.sending = false;  
    }	

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
         for(const auto &x : get_messages<typename Transduc_defs::TroomIn>(mbs)){
         state.Troom = x;
                if(state.model_active == false){
                        state.sending = true;
                        state.model_active = true;
                        state.next_internal = TIME("00:00:01:000");
                    }
                }
            for(const auto &x : get_messages<typename Transduc_defs::ToutIn>(mbs)){
			state.Tout = x;
                if(state.model_active == false) { 
                        state.sending = true;
                        state.model_active = true;
                        state.next_internal = TIME("00:00:01:000");
                    }
                }
                 if(state.Tout > state.Troom)   {
                        state.Tout = state.Troom;
						
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
			new1 = (state.Troom - (state.Troom - state.Tout)*0.1);
        get_messages<typename Transduc_defs::HeatlossOut>(bags).push_back(new1);
        return bags;
    }
    // time_advance function
    TIME time_advance() const {
        
       
        return TIME("00:00:01:000");
    }

    friend ostringstream& operator<<(ostringstream& os, const typename Transduc<TIME>::state_type& i) {
        os << "heatlossout " << i.Troom - (i.Troom - i.Tout)*0.1; 
        return os;
    }
};    
#endif // _TRANSDUC_HPP__
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	