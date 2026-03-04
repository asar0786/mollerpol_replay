/* Replay script for GEM analysis data */
#include "TSystem.h"
#include "TList.h"
#include "THaRun.h"
#include "THaEvent.h"
#include "THaAnalyzer.h"
#include "THaApparatus.h"
#include "TString.h"

void replay_moller_gem( UInt_t runnum=1859, UInt_t firstsegment=0, UInt_t maxsegments=1, UInt_t firstevent=0, UInt_t nevents=1000){
    MOLLERSpectrometer *moller = new MOLLERSpectrometer("mollerPol", "Generic apparatus");
    MOLLERGEMSpectrometerTracker *polgem = new MOLLERGEMSpectrometerTracker("polgem", "4-layer cosmic test stand");
    
    moller->AddDetector(polgem);

    THaAnalyzer* analyzer = new THaAnalyzer;
    
    gHaApps->Add(moller);
  
    THaEvent* event = new THaEvent;
    
    TString prefix = gSystem->Getenv("DATA_DIR");
    
    bool segmentexists = true;
    int segment=firstsegment; 
  
    int lastsegment=firstsegment;
    
    TClonesArray *filelist = new TClonesArray("THaRun",10);
  
    TDatime now = TDatime();
    
    int segcounter=0;
    while( segcounter < maxsegments && segment - firstsegment < maxsegments ){

    TString codafilename;
//    codafilename.Form( "%s/ssp_gem_apv_test_%d.evio.%d", prefix.Data(), runnum, segment );
    codafilename.Form( "%s/moller_ssp_%d.evio.%d", prefix.Data(), runnum, segment );

    segmentexists = true;
    
    if( gSystem->AccessPathName( codafilename.Data() ) ){
      segmentexists = false;
    } else if( segcounter == 0 ){
      new( (*filelist)[segcounter] ) THaRun( codafilename.Data() );
      cout << "Added segment " << segcounter << ", CODA file name = " << codafilename << endl;

      
      ( (THaRun*) (*filelist)[segcounter] )->SetDate(now);
      ( (THaRun*) (*filelist)[segcounter] )->SetNumber( runnum );
      //( (THaRun*) (*filelist)[segcounter] )->Init();
      
    } else {
      THaRun *rtemp = ( (THaRun*) (*filelist)[segcounter-1] ); //make otherwise identical copy of previous run in all respects except coda file name:
      new( (*filelist)[segcounter] ) THaRun( *rtemp );
      ( (THaRun*) (*filelist)[segcounter] )->SetFilename( codafilename.Data() );
      ( (THaRun*) (*filelist)[segcounter] )->SetNumber( runnum );
      ( (THaRun*) (*filelist)[segcounter] )->SetDate(now);
      cout << "Added segment " << segcounter << ", CODA file name = " << codafilename << endl;
    }
    if( segmentexists ){
      segcounter++;
      lastsegment = segment;
    }
    segment++;
  }

  cout << "n segments to analyze = " << segcounter << endl;
  
  prefix = gSystem->Getenv("OUT_DIR");
  firstsegment = 0;
  lastsegment = 1;
  TString outfilename;
  outfilename.Form( "%s/moller_polgem_replayed_%d_seg%d_%d.root", prefix.Data(), runnum,firstsegment,lastsegment);

  analyzer->SetVerbosity(2);
  analyzer->SetMarkInterval(100);

  analyzer->EnableBenchmarks();
  
  // Define the analysis parameters
  analyzer->SetEvent( event );
  analyzer->SetOutFile( outfilename.Data() );
  // File to record cuts accounting information
  analyzer->SetSummaryFile("summary_example.log"); // optional

  prefix = gSystem->Getenv("MOLLER_REPLAY");
  prefix += "/";

  TString odef_filename = "replay_mollerPol_polgem.odef";
  
  odef_filename.Prepend( prefix );

  
  analyzer->SetOdefFile( odef_filename );
  
  //analyzer->SetCompressionLevel(0); // turn off compression

  filelist->Compress();

  for( int iseg=0; iseg<filelist->GetEntries(); iseg++ ){
    THaRun *run = ( (THaRun*) (*filelist)[iseg] );
    if( nevents > 0 ) run->SetLastEvent(nevents); //not sure if this will work as we want it to for multiple file segments chained together

    run->SetFirstEvent( firstevent );
    
    run->SetDataRequired(0);
    
    analyzer->Process(run);     // start the actual analysis
  }
}

