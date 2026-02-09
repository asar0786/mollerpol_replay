// replay_polgem_segments.C
#include <iostream>
#include <cstring>

#include "TSystem.h"
#include "TDatime.h"
#include "TString.h"
#include "TROOT.h"

#include "THaGlobals.h"
#include "THaAnalyzer.h"
#include "THaEvent.h"
#include "THaRun.h"
#include "THaApparatus.h"
#include "THaDetector.h"

// NOTE:
// We intentionally do NOT include project headers here.
// We rely on ROOT dictionaries from libMollerPol.so and create objects via gROOT->ProcessLine().

static TString GetEnvStr(const char* key, const char* fallback = "")
{
  const char* v = gSystem->Getenv(key);
  return (v && *v) ? TString(v) : TString(fallback);
}

static TString ResolveDBDir()
{
  TString dbdir = GetEnvStr("DB_DIR", "");

  // If user didn't set DB_DIR at all, just return empty:
  if (dbdir.Length() == 0) return "";

  // If DB_DIR points to ".../DB" but files are in ".../DB/20230226", auto-fix:
  if (gSystem->AccessPathName(Form("%s/db_run.dat", dbdir.Data())) &&
      !gSystem->AccessPathName(Form("%s/20230226/db_run.dat", dbdir.Data()))) {
    dbdir += "/20230226";
  }

  return dbdir;
}

void replay_polgem_segments(
    int runnum = 1857,
    const char* datadir = "/volatile/halla/moller12gev/asar/TestLab_Data", // KEEP SAME as you asked
    int firstseg = 0,
    int lastseg  = -1,      // -1 = auto until missing file
    long firstevent = 0,
    long nevents    = 5000,  // 0 = all events in each segment
    const char* out = ""     // "" -> auto: $OUT_DIR/polgem_<run>.root (or ./polgem_<run>.root)
){
  // -------- Load analyzer libs (assumes you sourced analyzer-1.7.18 setup.csh) --------
  gSystem->Load("libPodd.so");
  gSystem->Load("libHallA.so");

  // -------- Load your plugin library (libMollerPol.so) --------
  // Best case: your setenv.csh put build dir in LD_LIBRARY_PATH so this works:
  int rc = gSystem->Load("libMollerPol.so");

  // Fallback: if not found, try $MOLLERPOL_BUILD/libMollerPol.so
  if (rc < 0) {
    TString bld = GetEnvStr("MOLLERPOL_BUILD", "");
    if (bld.Length()) {
      rc = gSystem->Load(Form("%s/libMollerPol.so", bld.Data()));
    }
  }

  if (rc < 0) {
    ::Error("replay_polgem_segments",
            "Failed to load libMollerPol.so. Fix by either:\n"
            "  (1) add your build dir to LD_LIBRARY_PATH in setenv.csh, or\n"
            "  (2) setenv MOLLERPOL_BUILD <path-to-build>\n");
    return;
  }

  // -------- Output/log dirs from env --------
  TString outdir = GetEnvStr("OUT_DIR", ".");
  TString logdir = GetEnvStr("LOG_DIR", Form("%s/log", outdir.Data()));

  gSystem->mkdir(outdir.Data(), kTRUE);
  gSystem->mkdir(logdir.Data(), kTRUE);

  TString outfile;
  if (out && std::strlen(out) > 0) {
    // user explicitly provided output file
    outfile = out;
  } else {
    // auto output file inside OUT_DIR
    outfile = Form("%s/polgem_%d.root", outdir.Data(), runnum);
  }

  // -------- DB_DIR handling --------
  TString dbdir = ResolveDBDir();
  if (dbdir.Length()) {
    gSystem->Setenv("DB_DIR", dbdir.Data());
  } else {
    // last resort fallback (only works if you run from repo top)
    gSystem->Setenv("DB_DIR", "DB/20230226");
  }

  std::cout << "=== replay_polgem_segments ===\n"
            << "  runnum   : " << runnum << "\n"
            << "  datadir  : " << datadir << "\n"
            << "  OUT_DIR  : " << outdir << "\n"
            << "  out file : " << outfile << "\n"
            << "  DB_DIR   : " << gSystem->Getenv("DB_DIR") << "\n"
            << "==============================\n";

  // -------- Create apparatus + detector WITHOUT needing headers --------
  auto* mollerPol = (THaApparatus*) gROOT->ProcessLine(
      "new MOLLERSpectrometer(\"mollerPol\",\"Generic apparatus\");"
  );
  auto* polgem = (THaDetector*) gROOT->ProcessLine(
      "new MOLLERGEMSpectrometerTracker(\"polgem\",\"cosmic stand\");"
  );

  if (!mollerPol || !polgem) {
    ::Error("replay_polgem_segments",
            "Failed to create MOLLERSpectrometer or MOLLERGEMSpectrometerTracker.\n"
            "This usually means the ROOT dictionary is missing from libMollerPol.so.\n");
    return;
  }

  mollerPol->AddDetector(polgem);
  gHaApps->Add(mollerPol);

  // -------- Analyzer --------
  auto* analyzer = new THaAnalyzer;
  analyzer->SetVerbosity(2);
  analyzer->SetMarkInterval(100);
  analyzer->SetOutFile(outfile.Data());
  analyzer->SetOdefFile("replay_mollerPol_polgem.odef");
  analyzer->SetSummaryFile(Form("%s/summary_polgem_%d.log", logdir.Data(), runnum));

  auto* event = new THaEvent;
  analyzer->SetEvent(event);

  TDatime now;

  int seg = firstseg;
  int nseg_used = 0;

  while (true) {
    if (lastseg >= 0 && seg > lastseg) break;

    TString f;
    f.Form("%s/moller_ssp_%d.evio.%d", datadir, runnum, seg);

    if (gSystem->AccessPathName(f.Data())) {
      if (lastseg < 0) break;  // auto mode: stop at first missing
      seg++;
      continue;
    }

    std::cout << ">>> Processing segment " << seg << " : " << f << std::endl;

    THaRun run(f.Data());
    run.SetNumber(runnum);
    run.SetDate(now);
    run.SetFirstEvent(firstevent);

    if (nevents > 0)
      run.SetLastEvent(firstevent + nevents - 1);

    run.SetDataRequired(0); // cosmic / SSP
    analyzer->Process(run);

    nseg_used++;
    seg++;
  }

  std::cout << ">>> Done. Segments processed = " << nseg_used << std::endl;
}

