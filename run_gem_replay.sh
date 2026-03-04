#!/bin/tcsh

###########################################################################################################
# Written by Asar Ahmed for C-Shell (tcsh)
# Last updated: March 4, 2026.
# 
# Usage: source run_gem_replay.csh <runnum> <first_seg> <max_seg> <first_event> <nevents>
#
###########################################################################################################

# Check if enough arguments are provided
if ( $#argv < 5 ) then
    echo "Usage: source run_gem_replay.csh <runnum> <first_seg> <max_seg> <first_event> <nevents>"
    exit 1
endif

set runnum = $1
set firstsegment = $2
set maxsegments = $3
set firstevent = $4
set nevents = $5

set fname_prefix = "moller_ssp"

# 1. Source the C-Shell version of the analyzer setup
source /w/halla-scshelf2102/moller12gev/asar/Analyzer/local/analyzer-1.7.18/bin/setup.csh

# 2. Set environment variables using setenv (No equals signs in C-Shell)
setenv MOLLERPOL_HOME /w/halla-scshelf2102/moller12gev/asar/Moller_Polarimeter/mollerpol_analyzer_clean
setenv MOLLERPOL_BUILD ${MOLLERPOL_HOME}/build

# 3. Update LD_LIBRARY_PATH
if ( $?LD_LIBRARY_PATH ) then
    setenv LD_LIBRARY_PATH ${MOLLERPOL_BUILD}:${LD_LIBRARY_PATH}
else
    setenv LD_LIBRARY_PATH ${MOLLERPOL_BUILD}
endif

# 4. Set necessary environment variables for replay
setenv MOLLER_REPLAY /w/halla-scshelf2102/moller12gev/asar/Moller_Polarimeter/Replay/mollerpol_replay
setenv DB_DIR ${MOLLER_REPLAY}/DB
setenv OUT_DIR /volatile/halla/moller12gev/asar/GEMRootfiles
setenv LOG_DIR /volatile/halla/moller12gev/asar/GEMRootfiles/logs

# 5. Find the data directory
set DATA_DIR_CANDIDATES = "/adaqeb1/data1 /adaqeb2/data1 /adaqeb3/data1 /cache/halla/sbs/GEp/raw /volatile/halla/moller12gev/asar/TestLab_Data"
set found_dir = ""

foreach d ( $DATA_DIR_CANDIDATES )
    if ( -f "${d}/moller_ssp_${runnum}.evio.${firstsegment}" ) then
        set found_dir = "$d"
        break
    endif
end

if ( "$found_dir" == "" ) then
    echo "ERROR: Could not find moller_ssp_${runnum}.evio.${firstsegment} in any candidate directory"
    exit 1
endif

setenv DATA_DIR "$found_dir"
echo "Using DATA_DIR=$DATA_DIR"

# 6. Run the analyzer
analyzer -b -q "${MOLLER_REPLAY}/replay_moller_gem.C(${runnum},${firstsegment},${maxsegments},${firstevent},${nevents})"

echo "Exited the Analyzer process"
