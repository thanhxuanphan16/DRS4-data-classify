void moving_average_filter(TH1D *input){

  int filterWidth = 11;
  double val = 0.;
  int binUp = input->GetNbinsX() - filterWidth/2;
  int binLow =  filterWidth/2;
  int Nbins = input->GetNbinsX();
  double *data = new double[Nbins];


  for(int i = 0; i<Nbins; i++){

    data[i] = input->GetBinContent(i+1);
    //cout<<"Data "<<i<<" : "<<data[i]<<endl;
  }


  for(int i = binLow; i< binUp +1; i++){
    val = 0.;
    for(int j = -filterWidth/2; j<filterWidth/2 +1; j++){

      val = val + data[i+j];
    }
    val = val/filterWidth;
    input->SetBinContent(i, val);
    //cout<<"i : "<<i<<endl;
  }

  delete [] data;

}
