void PSDFunc(TH1D *h, double *Arg){

 double short_gate;
 double long_gate;
 double Q_ratio;
 double E;
 double Q_ratio_new;
 double pulse_height;


 //loop for finding peak///

 pulse_height = h->GetMaximum();
 TAxis *axis = h->GetXaxis();
 double bin_width=0.;

 

 int bin_low_LG = 1;
 int bin_up_LG = 500;
 int bin_low_SG = 133;
 
 //calculate Long gate
 long_gate =0.;
 for(int i = bin_low_LG; i<bin_up_LG+1; i++){ //bin = x*2
   bin_width = axis->GetBinWidth(i);
   //cout<<"i : "<<i<<"bin width : "<<bin_width<<"\n";
   long_gate = long_gate + h->GetBinContent(i+1)*bin_width;
 }
 
 //calculate Short gate
 short_gate =0.;
 for(int i =  bin_low_SG; i<bin_up_LG+1; i++){
   bin_width = axis->GetBinWidth(i);
   short_gate = short_gate + h->GetBinContent(i)*bin_width;
 }
 
 //calculate Q ratio
 Q_ratio = short_gate/long_gate;

 //calculate Short gate new
 short_gate =0.;
 for(int i =  bin_low_LG; i<bin_low_SG+1; i++){
   bin_width = axis->GetBinWidth(i);
   short_gate = short_gate + h->GetBinContent(i)*bin_width;
 }

 Q_ratio_new = (long_gate - short_gate)/long_gate;

 
 //Calculate energy
 E = 0.13826*long_gate+0.02404; //depend on database
 
 //Assign result to Arg
  Arg[0] = short_gate;
  Arg[1] = long_gate;
  Arg[2] = Q_ratio;
  Arg[3] = E;
  Arg[4] = Q_ratio_new;
  Arg[5] = pulse_height;   

}