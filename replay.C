#include "TSystem.h"
#include "TList.h"
#include "THaRun.h"
#include "THaEvent.h"
#include "THaAnalyzer.h"
#include "THaApparatus.h"
#include "MollerPolHelicityDecoderBoard.h"
#include "MollerPolApparatus.h"
#include "MollerPolCalorimeter.h"
#include "MollerPolScalerEvtHandler.h"


void replay(int run_number=0, int seg = 0, int nevents=-1,
	    UInt_t first_event = 0, UInt_t last_event = -1){

  gSystem->Load("libMollerPol");

  MollerPolApparatus *mol = new MollerPolApparatus("M", "Moller polarimeter apparatus");
  gHaApps->Add(mol);
  mol->AddDetector( new MollerPolHelicityDecoderBoard("heldecoder", "Helicity Decoder Module") );

  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Apparatus's and PhysicsModules,
  // and executes the output routines.
  THaAnalyzer* analyzer = new THaAnalyzer;
  
  // Add event handler for scaler events
  MollerPolScalerEvtHandler* scaler = new MollerPolScalerEvtHandler("M", "scaler event type 1");
  scaler->AddEvtType(1);
  gHaEvtHandlers->Add(scaler);
  //scaler->SetDebugFile("DebugScaler.txt");

  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  THaEvent* event = new THaEvent;
  
  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
//  THaRun* run = new THaRun( "prod12_4100V_TrigRate25_4.dat" );
  //THaRun* run = new THaRun(Form("/adaqfs/home/hamoller/data/mollerpol_test_%d.evio.%d",run_number,seg) );
  THaRun* run = new THaRun(Form("/hamoller/data/raw/mollerpol_test_%d.evio.%d",run_number,seg) );

  run->SetFirstEvent(first_event);
  run->SetLastEvent((UInt_t)(last_event > first_event ? last_event : nevents));

  run->SetDataRequired(1);
  run->SetDate(TDatime());

  analyzer->SetVerbosity(4);
  analyzer->SetOdefFile("replay.odef");
  
  // Define the analysis parameters
  analyzer->SetEvent( event );
  //analyzer->SetOutFile( Form("%s/fadcV2_moller_analyzer_%d.%d.root", gSystem->Getenv("HAMOLLER_ROOTFILE_DIR"),run_number,seg) );
  analyzer->SetOutFile( Form("%s/fadcV2_moller_analyzer_%d.%d.root", "/hamoller/data/Rootfiles",run_number,seg) );
  // File to record cuts accounting information
  analyzer->SetSummaryFile( Form("summary/summary_%d.log", run_number) ); // optional
  
  //analyzer->SetCompressionLevel(0); // turn off compression
  analyzer->Process(run);     // start the actual analysis
}
