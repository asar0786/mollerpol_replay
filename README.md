# mollerpol_replay

# Set up the directory
1. Set up the correct enviroment variables, such as:
```
export LD_LIBRARY_PATH=/home/hamoller/hanjie/moller_analyzer/build:$LD_LIBRARY_PATH
```
The above is adding the moller customized library "libMollerPol.so" to the path.
```
export DB_DIR=/home/hamoller/hanjie/moller_analyzer/replay/DB
```
This points to the replay data base.

2. Set up the data file directory. The default data searching path is data/
3. Set up the Rootfiles directory. The root files shouldn't be saved under the /home directory. A better place is /adaq1/data1/hamoller/<username>

# Run the replay
```
analyzer "replay.C(run_number[,number of events])"
```
# To analyze GEM data
You must set up the environment variables to point to the correct libraries and database.
Bash User
export MOLLERPOL_HOME=/path/to/your/mollerpol_analyzer
export MOLLERPOL_BUILD=$MOLLERPOL_HOME/build
export LD_LIBRARY_PATH=$MOLLERPOL_BUILD:$LD_LIBRARY_PATH
export DB_DIR=$MOLLER_REPLAY/DB

#Standard Replay
```
analyzer 'replay_moller_gem.C(run_number, first seg, last seg, first event, total events(-1 for all))'
```
Or use specialized script which handles environment sourcing and automated data path searching
```
./run_gem_replya.sh runnumber first_segment last_segment first_event total_events
```
# Alignment Script

The script GEM_align.C (located in scripts/gems/) is used to align the GEM modules. It recalculates module positions from tracks by iterating until the χ2 is minimized.

To run the alignment:
```
root -l -q 'scripts/gems/GEM_align.C("input_alignment.txt")'
```
Outputs:

The script generates two output files containing optimized geometry parameters.
Action: Update your database (DB_DIR) module positions using the values from these output files to improve tracking resolution.
