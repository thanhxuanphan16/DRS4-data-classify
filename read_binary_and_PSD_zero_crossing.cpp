/*
   Name:           read_binary.cpp
   Created by:     Stefan Ritt <stefan.ritt@psi.ch>
   Date:           July 30th, 2014

   Purpose:        Example file to read binary data saved by DRSOsc.
 
   Compile and run it with:
\ 
      gcc -o read_binary read_binary.cpp
 
      ./read_binary <filename>

   This program assumes that a pulse from a signal generator is split
   and fed into channels #1 and #2. It then calculates the time difference
   between these two pulses to show the performance of the DRS board
   for time measurements.

   $Id: read_binary.cpp 22290 2016-04-27 14:51:37Z ritt $
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <vector>



////xuan
#include <TH1D.h>
#include <TObject.h>
#include <TF1.h>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <time.h>


using namespace std;

#include "Mapping.cpp"





///Pre-Processing mapping

//#include "Mapping.cpp"


//////////////////////


clock_t start_time = clock();

const double PI = 3.14159265;



typedef struct {
   char           tag[3];
   char           version;
} FHEADER;

typedef struct {
   char           time_header[4];
} THEADER;

typedef struct {
   char           bn[2];
   unsigned short board_serial_number;
} BHEADER;

typedef struct {
   char           event_header[4];
   unsigned int   event_serial_number;
   unsigned short year;
   unsigned short month;
   unsigned short day;
   unsigned short hour;
   unsigned short minute;
   unsigned short second;
   unsigned short millisecond;
   unsigned short range;
} EHEADER;

typedef struct {
   char           tc[2];
   unsigned short trigger_cell;
} TCHEADER;

typedef struct {
   char           c[1];
   char           cn[3];
} CHEADER;

/*-----------------------------------------------------------------------------*/

int main(int argc,const char * argv[])
{
   FHEADER  fh;
   THEADER  th;
   BHEADER  bh;
   EHEADER  eh;
   TCHEADER tch;
   CHEADER  ch;
   
   unsigned int scaler;
   unsigned short voltage[1024];
   double waveform[16][4][1024], time[16][4][1024];
   float bin_width[16][4][1024];
   int i, j, b, chn, n, chn_index, n_boards;
   double t1,t2,dt;
  
   char filename[256];////////////////xuan
   
   int ndt;

   ///////////////Xuan//////////////////////////////////////
   //Reading Pars file=========================
   int index=0;
   string line;
   int decisionVar;
   string file_data_name_str;// = "run_BM_012.dat";
   string file_output_name_str;// = "test.root";
   string Nevent_str;
   string WaveformSaveDecision;
   vector <string> PreProcStr;
   vector <string> AnalsStr;
   ifstream readPars("SettingPars.txt");
   
   while(getline(readPars,line))
   {  
    //Remove all space in string
    line.erase(std::remove_if(line.begin(),line.end(),::isspace),line.end());
    //============================
    if(line=="Begin"){
      decisionVar = 0;
      continue;
    }
    else if(line=="#DataFileName"){
      decisionVar = 1;
      continue;
    }
    else if(line=="#OutputFileName"){
      decisionVar = 2;
      continue;
    }
    else if (line=="#Nevent"){
      decisionVar = 3;
      continue;
    }
    else if (line=="#PreProc"){
      decisionVar = 4;
      continue;
    }
    else if (line == "#Analyse"){
      decisionVar = 5;
      continue;
    }
    else if (line=="#WaveformSave(YES/NO)"){
      decisionVar = 6;
      continue;
    }
    else if (line=="#End"){
      decisionVar = 0;
      continue;
    }
    //============================
    
    if(decisionVar == 1){ 
      file_data_name_str = line;
      //cout<<line<<"\n";
      decisionVar = 0;
    }
    else if (decisionVar == 2){
      file_output_name_str = line;
      //cout<<line<<"\n";
      decisionVar = 0; 
    }
    else if (decisionVar == 3){
      Nevent_str = line;
      decisionVar = 0;
    }
    else if (decisionVar == 4){
      PreProcStr.push_back(line);
    }
    else if (decisionVar == 5){
      AnalsStr.push_back(line);
    }
    else if (decisionVar == 6){
      WaveformSaveDecision = line;
    }
    
   }
   

   //Getting Directory==========================
   char buff[256]; //array hold current directory 
   getcwd(buff, 256);
   string currentPath(buff);
   currentPath = currentPath + "/";

   string data_directory = currentPath+"Data/";
   string output_directory = currentPath+"Output/";
   //string file_data_name_str = "run_BM_012.dat";
   //string file_output_name_str = "test.root";

   string file_data_directory = data_directory + file_data_name_str;
   string file_output_directory = output_directory + file_output_name_str;

   TFile *file = TFile::Open(file_output_directory.c_str(),"recreate");
   
   //TTree Define=============================
   #include "TTreeDeclaring.cpp"
   
   ///Number of Event and Histogram declaring===================
   int Nevent;
   std::istringstream(Nevent_str)>>Nevent; //String to int conversion (checking carefully input when using this method)
   TH1D** h=new TH1D*[Nevent];
   //TH1D** output = new TH1D*[Nevent];


   /////////////////////////////////////////////////////////////////
   
   if (argc >= 1)
     //strcpy(filename,argv[1]);
     strcpy(filename, file_data_directory.c_str());
   else {
      printf("Usage: read_binary <filename>\n");
      return 0;
   }
  

   // open the binary waveform file
   FILE *f = fopen(filename, "r"); 
  
  
   if (f == NULL) {
      printf("Cannot find file \'%s\'\n", filename);
      return 0;
          
   }
   
   // read file header
   fread(&fh, sizeof(fh), 1, f);
   ////////////xuan
   cout<<"fh.tag : "<<fh.tag[0]<<fh.tag[1]<<fh.tag[2]<<"\n";
   if (fh.tag[0] != 'D' || fh.tag[1] != 'R' || fh.tag[2] != 'S') {
      printf("Found invalid file header in file \'%s\', aborting.\n", filename);
      
      return 0;
   }

    //cout<<"1 \n";
    //getchar();
   cout<<"fh.version : "<<fh.version<<"\n";///////xuan////////////////////
   if (fh.version != '2') {
      printf("Found invalid file version \'%c\' in file \'%s\', should be \'2\', aborting.\n", fh.version, filename);
      
      return 0;
   }
    //cout<<"2 \n";
    //getchar();
   // read time header
   fread(&th, sizeof(th), 1, f);
   if (memcmp(th.time_header, "TIME", 4) != 0) {
      printf("Invalid time header in file \'%s\', aborting.\n", filename);
      return 0;
   }
   //cout<<"3 \n";
   //getchar();
   for (b = 0 ;; b++) {
      // read board header
      fread(&bh, sizeof(bh), 1, f);
      if (memcmp(bh.bn, "B#", 2) != 0) {
         // probably event header found
         fseek(f, -4, SEEK_CUR);
         break;
      }
      
      printf("Found data for board #%d\n", bh.board_serial_number);
      
      // read time bin widths
      memset(bin_width[b], sizeof(bin_width[0]), 0);
      for (chn=0 ; chn<5 ; chn++) {
         fread(&ch, sizeof(ch), 1, f);
         if (ch.c[0] != 'C') {
            // event header found
            fseek(f, -4, SEEK_CUR);
            break;
         }
         i = ch.cn[2] - '0' - 1;
         printf("Found timing calibration for channel #%d\n", i+1);
         fread(&bin_width[b][i][0], sizeof(float), 1024, f);
         // fix for 2048 bin mode: double channel
         if (bin_width[b][i][1023] > 10 || bin_width[b][i][1023] < 0.01) {
            for (j=0 ; j<512 ; j++)
               bin_width[b][i][j+512] = bin_width[b][i][j];
         }
      }
   }
   n_boards = b;
   
   //xuan////////////////////////////////////////////////////////////////////

   // loop over all events in the data file
   for (n=0 ; n<Nevent; n++) {
      // read event header
      i = (int)fread(&eh, sizeof(eh), 1, f);
      if (i < 1)
         break;
      
      printf("Found event #%d\n", eh.event_serial_number);
      
      // loop over all boards in data file
      for (b=0 ; b<n_boards ; b++) {
         
         // read board header
         fread(&bh, sizeof(bh), 1, f);
         if (memcmp(bh.bn, "B#", 2) != 0) {
            printf("Invalid board header in file \'%s\', aborting.\n", filename);
            return 0;
         }
         
         // read trigger cell
         fread(&tch, sizeof(tch), 1, f);
         if (memcmp(tch.tc, "T#", 2) != 0) {
            printf("Invalid trigger cell header in file \'%s\', aborting.\n", filename);
            return 0;
         }

         if (n_boards > 1)
            printf("Found data for board #%d\n", bh.board_serial_number);
         
           for (chn=0 ; chn<4 ; chn++) {
            
            // read channel header
            fread(&ch, sizeof(ch), 1, f);
            if (ch.c[0] != 'C') {
               // event header found
               fseek(f, -4, SEEK_CUR);
               break;
            }
            chn_index = ch.cn[2] - '0' - 1;
            fread(&scaler, sizeof(int), 1, f);
            fread(voltage, sizeof(short), 1024, f);
            
            for (i=0 ; i<1024 ; i++) {
               // convert data to volts
               waveform[b][chn_index][i] = (voltage[i] / 65535. + eh.range/1000.0 - 0.5);	// sang dien the nguong -0.05V den 0.95V
	     
		            									
                // calculate time for this cell
                for (j=0,time[b][chn_index][i]=0 ; j<i ; j++)
                  time[b][chn_index][i] += bin_width[b][chn_index][(j+tch.trigger_cell) % 1024]; 
    		}
 		        	
         }
         
         // align cell #0 of all channels
         t1 = time[b][0][(1024-tch.trigger_cell) % 1024];
         for (chn=1 ; chn<4 ; chn++) {
            t2 = time[b][chn][(1024-tch.trigger_cell) % 1024];
            dt = t1 - t2;
            for (i=0 ; i<1024 ; i++)
               time[b][chn][i] += dt;
         }
////////////////// END OF BINARY READING ///////////////////////////////////////


/// Pedestal calculation /// Pedestal means offset ////////////////////////

 double pedestal=0.;
 for (i=10;i<90;i++) pedestal=pedestal+waveform[b][chn_index][i];
 pedestal=pedestal/80.;
 //pedestal=0.;
 cout<<"pedestal : "<<pedestal<<"\n";
 /////// Pedestal substraction and pulse conversion ///////////////////
 for (i=0;i<1024;i++) waveform[b][chn_index][i]= -(waveform[b][chn_index][i]-pedestal);
 
 h[n] = new TH1D(TString::Format("h%d",n),TString::Format("Event %d",n),1024,0,512);
 //output[n] = new TH1D(TString::Format("output_%d",n),TString::Format("Event %d",n),1024,0,512); 
 //TH1D* h = new TH1D("h","Event",1024, 0, 512);

 

 //Filling histogram with data
 for(int i =0; i<h[n]->GetNbinsX(); i++){
   
   h[n]->SetBinContent(i+1,waveform[b][chn_index][i]);
 }

 //**************Pre-Processing Waveform******************
 for(int i=0; i<PreProcStr.size(); i++){
   PreProcMap[PreProcStr[i]](h[n]);
 }
 
 //**********************Analysing************************
 for(int i=0; i<AnalsStr.size(); i++){
   AnalsMap[AnalsStr[i]](h[n],Arg);
 }
 
 
 
 T->Fill(); //save data
 
 if(WaveformSaveDecision=="YES"){
   h[n]->Write();
 }
 //output[n]->Write();
 //output[n]->Delete();
 h[n]->Delete();
///////////////////////////////////////////////////////////////////////////////////////
      }
   }
   T->Write();
   file->Close();
   readPars.close();
   //save.close();

   //calculate executing time
   clock_t end_time = clock();

   cout<<"================================\n";
   cout<<"= Built by: Phan Thanh Xuan    =\n";
   cout<<"= Based on: Stefan Ritt's code =\n";
   cout<<"================================\n";
   cout<<"Executing time of program : "<<(double)(end_time - start_time)/CLOCKS_PER_SEC<<" s \n";
   

   return 1;
}

