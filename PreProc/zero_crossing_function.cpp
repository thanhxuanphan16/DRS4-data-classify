
//Note
//This function including 3 processing
//firt: Shifting waveform and calculating "Amplitude and Risetime Compensated Triggering (ARC))"
//second: Finding zero crossing position
//third: Correcting arrival time based on zero crossing position

void zero_crossing_function(TH1D* waveform)
{
  double xlow[1024];
  int bin_shift = 10;
  double attenuated_factor = 0.8;
  double value =0.;

  ///////////Fist processing /////////////////////////////////////

  
  //Loop for getting bin edge
  for(int i=0; i<1024; i++){
    if(i != 1023) xlow[i] = waveform->GetBinLowEdge(i+1);
    else xlow[i] = waveform->GetBinLowEdge(i+1) + waveform->GetBinWidth(i+1);
  }

  TH1D* shift_waveform = new TH1D("shift_waveform", "shifted waveform", 1023, xlow);
  TH1D* zero_crossing = new TH1D("zero_crossing","zero crossing", 1023, xlow);
  TH1D* output = new TH1D("output","output", 1024, 0, 512);
  
  //Loop for filling shifted waveform
  for(int i = 0; i< shift_waveform->GetNbinsX(); i++){
    value = 0.;
    value = waveform->GetBinContent(i+1 - bin_shift);
    shift_waveform->SetBinContent(i+1, value);
  }

  //Loop for ARC calculation
  for(int i=0; i<zero_crossing->GetNbinsX(); i++){
    value = 0.;
    value = shift_waveform->GetBinContent(i+1) - attenuated_factor*waveform->GetBinContent(i+1);
    zero_crossing->SetBinContent(i+1, value);
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////

  /////////////Second processing//////////////////////////////////////////////////////////
  double negative =0.;
  double positive =0.;
  double min=0.;
  double max=0.;
  int bin_min=0;
  int bin_max=0;
  int bin_zero_crossing_negative=0;
  int bin_zero_crossing_positive=0;
  //int bin_zero_crossing=0;

  //Loop for finding Minimum and Maximim
  for(int i =20; i< zero_crossing->GetNbinsX()-40; i++){
    negative = zero_crossing->GetBinContent(i+1);
    positive = zero_crossing->GetBinContent(zero_crossing->GetNbinsX() + 1 - i);
    
    if(negative<0 && negative<min){
      min = negative;
      bin_min = i+1;
    }
    if(positive>0 && positive>max){
      max = positive;
      bin_max = zero_crossing->GetNbinsX() + 1 -i;
    }

  }

  //Loop for finding zero crossing position
  for(int i = bin_min ; i<bin_max; i++){
    negative = zero_crossing->GetBinContent(i);

    if(negative > 0){
      bin_zero_crossing_negative = i - 1;
      break;
    }
  }

  for(int  i = 1 ; i<bin_max - bin_min; i++){
    positive = zero_crossing->GetBinContent(bin_max - i);
    if(positive < 0){
      bin_zero_crossing_positive = bin_max - i + 1;
      break;
    }
  }

  //Choosing zero crossing position
  /*
  if(bin_zero_crossing_positive - bin_zero_crossing_negative != 1){
    negative = zero_crossing->GetBinContent(bin_zero_crossing_negative + 1);
    positive = zero_crossing->GetBinContent(bin_zero_crossing_positive - 1);
    if(negative < zero_crossing->GetBinContent(bin_zero_crossing_negative) && positive > zero_crossing->GetBinContent(bin_zero_crossing_positive))
      {
	bin_zero_crossing = (int)((bin_zero_crossing_negative + bin_zero_crossing_positive)/2);
      }
    else if(negative < zero_crossing->GetBinContent(bin_zero_crossing_negative) && positive < zero_crossing->GetBinContent(bin_zero_crossing_positive)){
      bin_zero_crossing = bin_zero_crossing_negative;
    }
    else if(negative > zero_crossing->GetBinContent(bin_zero_crossing_negative) && positive > zero_crossing->GetBinContent(bin_zero_crossing_positive)){
      bin_zero_crossing = bin_zero_crossing_positive;
	}
    else{
      bin_zero_crossing = bin_zero_crossing_positive;
    }
  }
  */  
	  
	
	
  //cout<<"negative pos : "<<bin_zero_crossing_negative<<"\n";
  //cout<<"positive pos : "<<bin_zero_crossing_positive<<"\n";
  //cout<<"zero crossing : "<<bin_zero_crossing<<"\n";
  //TAxis*axis = zero_crossing->GetXaxis();
  //cout<<"Bin fit : "<<axis->FindBin(58.782)<<"\n";
  //zero_crossing->Draw();

  /////////////////////////////////////////////////////////////////////////////////////////////

  ///////Third processing///////////////////////////
  for(int i = 0 ; i<output->GetNbinsX(); i++){
    value =0.;
    value = waveform->GetBinContent(i + bin_zero_crossing_positive-60);
    output->SetBinContent(i+1, value);
  }

  waveform->Reset("ICSEM");
  for(int i = 0; i<waveform->GetNbinsX(); i++){
    value = 0;
    value = output->GetBinContent(i+1);
    waveform->SetBinContent(i+1,value);
  }

  ///////////////////////////////////////////////////////
  output->Delete();
  zero_crossing->Delete();
  shift_waveform->Delete();
  
}
