source /w/halla-scshelf2102/moller12gev/asar/Analyzer/local/analyzer-1.7.18/bin/setup.csh
setenv MOLLERPOL_HOME /w/halla-scshelf2102/moller12gev/asar/Moller_Polarimeter/mollerpol_analyzer/mollerpol_analyzer
setenv MOLLERPOL_BUILD $MOLLERPOL_HOME/build

# make ROOT find the plugin library at runtime
setenv LD_LIBRARY_PATH ${MOLLERPOL_BUILD}:$LD_LIBRARY_PATH


#setenv DATA_DIR /adaqeb1/data1:/cache/halla/sbs/GEp/raw:/cache/halla/sbs/GEnRP/raw:/adaqeb2/data1
setenv DATA_DIR /volatile/halla/moller12gev/asar/TestLab_Data
#setenv SBSOFFLINE /work/halla/sbs/asar/sbsoffline/install
setenv OUT_DIR /volatile/halla/moller12gev/asar/GEMRootfiles
setenv MOLLER_REPLAY /w/halla-scshelf2102/moller12gev/asar/Moller_Polarimeter/Replay/mollerpol_replay
setenv DB_DIR $MOLLER_REPLAY/DB
setenv LOG_DIR /volatile/halla/moller12gev/asar/GEMRootfiles/logs
#setenv ANALYZER_CONFIGPATH $SBS_REPLAY/replay

