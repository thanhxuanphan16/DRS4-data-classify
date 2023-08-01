   TTree *T = new TTree("T","Event");
   //Int_t event_n =0;
   double Arg[5];

   //T->Branch("Event_ID", &event_n,"event_n/I");
   T->Branch("short_gate",&Arg[0] ,"short_gate/D");
   T->Branch("long_gate",&Arg[1],"long_gate/D");
   T->Branch("Q_ratio",&Arg[2],"Q_ratio/D");
   T->Branch("E",&Arg[3],"E/D");
   T->Branch("Q_ratio_new",&Arg[4],"Q_ratio_new/D");
   T->Branch("pulse_height", &Arg[5], "pulse_height/D");