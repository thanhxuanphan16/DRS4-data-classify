#include <map>
#include "PreProc/zero_crossing_function.cpp"
#include "PreProc/moving_average_filter.cpp"

#include "Analysis/PSDFunc.cpp"


typedef void (*FnPtr1)(TH1D*, double*);
typedef void (*FnPtr)(TH1D*);

//Pre-Processing functions Mapping
map<string, FnPtr> PreProcMap ={
   {"A1",moving_average_filter},
   {"A2",zero_crossing_function},

}; 

//Analysing functions Mapping
map<string, FnPtr1> AnalsMap ={
    {"B1",PSDFunc}
};



