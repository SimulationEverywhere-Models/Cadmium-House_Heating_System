/* Transduc.HPP*/

#ifndef _THERMO_HPP__
#define _THERMO_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
using namespace cadmium;
using namespace std;


//Port definition 
struct Thermo_defs{
    struct TroomOut : public out_port<float> {}; 
	struct TroomOut_final : public out_port<float> {};
	struct ConOut : public out_port<float> {};

	struct HGIn : public in_port<float> {};
	struct HLIn : public in_port<float> {};

};
template<typename TIME> class Thermo{
    public:
	        TIME   preparationTime;
    // ports definition
	
               using input_ports=tuple<typename Thermo_defs::HGIn, typename Thermo_defs::HLIn>;
               using output_ports=tuple<typename Thermo_defs::TroomOut, typename Thermo_defs::ConOut,typename Thermo_defs::TroomOut_final>;


	
	 Thermo() noexcept{
          preparationTime  = TIME("00:00:01:000");
          state.next_internal    = std::numeric_limits<TIME>::infinity();
          state.model_active     = false;
        }
    // state definition
    struct state_type{
		bool model_active;
        bool sending;
		int index;
		
        float TroomOut;
        float ConOut=0;
		float HGIn;
		float HLIn=24.9;

		TIME next_internal;
    }; 
    state_type state; 	

 // internal transition
    void internal_transition() {
        state.sending = false;  
		
    }	

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
         
            for(const auto &x : get_messages<typename Thermo_defs::HGIn>(mbs)){
			state.HGIn = x;
                if(state.model_active == false) { 
                        state.sending = true;
						state.model_active = true;
                        state.next_internal = preparationTime;
                        
                    }
			}	
			for(const auto &x : get_messages<typename Thermo_defs::HLIn>(mbs)){
			state.HLIn = x;
                if(state.model_active == false) { 
                        state.sending = true;
						state.model_active = true;
                        state.next_internal = preparationTime;
                        
                    }		
						
                }
           	if(state.HLIn + state.HGIn < 24.9)   {
                        state.ConOut = 1;
						
			}else{
                        state.ConOut = 0;
						
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
			new1 = (state.HGIn + state.HLIn);
        get_messages<typename Thermo_defs::TroomOut>(bags).push_back(new1);
		get_messages<typename Thermo_defs::TroomOut_final>(bags).push_back(new1);
		
		float new2;
            new2 = (state.ConOut);
        get_messages<typename Thermo_defs::ConOut>(bags).push_back(new2);
        return bags;
		
		
    }
    // time_advance function
    TIME time_advance() const {
        
       
        return TIME("00:00:01:000");
    }

    friend ostringstream& operator<<(ostringstream& os, const typename Thermo<TIME>::state_type& i) {
        os << "Troom: " << (i.HGIn + i.HLIn) << " & ControlSignal " << i.ConOut; 
        return os;
    }
};    
#endif // _TRANSDUC_HPP__
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	