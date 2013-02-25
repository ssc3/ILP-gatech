
##################################################
#
#   ECE6100 Advanced Computer Architectures
#   School of Electrical and Computer Engineering
#   Georgia Institute of Technology
#
#   Instructor: Prof. Hsien-Hsin Sean Lee
#
#   CPU2000 benchmark execute script
##################################################

# Your simplescalar simulation root, you have to modify this to your directory where you place your sim-safe 
SIM_DIR  = /home/bulldog1/leehs/Courses/2008/ECE6100/Project/P1/simplesim-3.0

# CPU2000 benchmark direcotry, you have to modify this to your SPEC directory
SIM_DIR_SPEC2000INT = /home/bulldog1/leehs/research/CPU2000sim/CPU2000int
SIM_DIR_SPEC2000FP  = /home/bulldog1/leehs/research/CPU2000sim/CPU2000fp

# The simulator name
SIM_BIN   =  sim-safe

# configuration file for your simulator, none for this project
SIM_CONFIG =

# additional simulation knobs
SIM_KNOBS  = $(SIM_CONFIG) -max:inst 1000000000

# The master directory of your results. Simulation results will be deposited into this directory, create /results directory first
MASTER_RESULT_DIR     = /home/bulldog1/leehs/Courses/2008/ECE6100/Project/P1/results

# the individual result directory of your simulation, you have to create these directories yourself
RESULT_DIR = $(MASTER_RESULT_DIR)/Naive


# run the entire CPU2000 suite with the test inputs
all: \
       	$(RESULT_DIR)/eon.cook.stdout \
       	$(RESULT_DIR)/gcc.166.stdout \
       	$(RESULT_DIR)/mcf.inp.stdout \
       	$(RESULT_DIR)/parser.ref.stdout \
       	$(RESULT_DIR)/twolf.ref.stdout \
       	$(RESULT_DIR)/perlbmk.scrabbl.stdout \
	$(RESULT_DIR)/ammp.ref.stdout \
	$(RESULT_DIR)/equake.ref.stdout \
	$(RESULT_DIR)/fma3d.ref.stdout \
	$(RESULT_DIR)/mgrid.ref.stdout \
	$(RESULT_DIR)/swim.ref.stdout \

int-all: \
       	$(RESULT_DIR)/eon.cook.stdout \
       	$(RESULT_DIR)/gcc.166.stdout \
       	$(RESULT_DIR)/mcf.inp.stdout \
       	$(RESULT_DIR)/parser.ref.stdout \
       	$(RESULT_DIR)/twolf.ref.stdout \
       	$(RESULT_DIR)/perlbmk.scrabbl.stdout \

fp-all: \
        $(RESULT_DIR)/ammp.ref.stdout \
	$(RESULT_DIR)/equake.ref.stdout \
	$(RESULT_DIR)/fma3d.ref.stdout \
	$(RESULT_DIR)/mgrid.ref.stdout \
	$(RESULT_DIR)/swim.ref.stdout \

# run the CPU2000 benchmarks individually by name
eon-cook: \
       $(RESULT_DIR)/eon.cook.stdout
gcc-166: \
       $(RESULT_DIR)/gcc.166.stdout
mcf: \
       $(RESULT_DIR)/mcf.inp.stdout
parser: \
       $(RESULT_DIR)/parser.ref.stdout
twolf: \
       $(RESULT_DIR)/twolf.ref.stdout
perl: \
       $(RESULT_DIR)/perlbmk.scrabbl.stdout
ammp: \
       $(RESULT_DIR)/ammp.ref.stdout
equake: \
       $(RESULT_DIR)/equake.ref.stdout
fma3d: \
       $(RESULT_DIR)/fma3d.ref.stdout
mgrid: \
       $(RESULT_DIR)/mgrid.ref.stdout
swim: \
       $(RESULT_DIR)/swim.ref.stdout
clean:
	\rm $(RESULT_DIR)/*


$(RESULT_DIR)/eon.cook.stdout: $(SIM_DIR)/$(SIM_BIN)
	-cp $(SIM_DIR_SPEC2000INT)/252.eon/data/ref/input/eon.dat .  
	-cp $(SIM_DIR_SPEC2000INT)/252.eon/data/ref/input/materials .  
	-cp $(SIM_DIR_SPEC2000INT)/252.eon/data/ref/input/spectra.dat .  
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/eon.cook.stdout \
	  $(RESULT_DIR)/eon.cook.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS) $(SIM_DIR_SPEC2000INT)/252.eon/exe/eon_peak.ev6 \
	  $(SIM_DIR_SPEC2000INT)/252.eon/data/ref/input/chair.control.cook \
	  $(SIM_DIR_SPEC2000INT)/252.eon/data/ref/input/chair.camera \
	  $(SIM_DIR_SPEC2000INT)/252.eon/data/ref/input/chair.surfaces \
	  ./chair.cook.ppm ppm \
	  ./pixels_out.cook 
	  rm eon.dat
	  rm materials
	  rm spectra.dat

$(RESULT_DIR)/gcc.166.stdout: $(SIM_DIR)/$(SIM_BIN)
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/gcc.166.stdout \
	  $(RESULT_DIR)/gcc.166.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS) $(SIM_DIR_SPEC2000INT)/176.gcc/exe/cc1_peak.ev6 < \
	  $(SIM_DIR_SPEC2000INT)/176.gcc/data/ref/input/166.i

$(RESULT_DIR)/mcf.inp.stdout: $(SIM_DIR)/$(SIM_BIN)
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/mcf.inp.stdout \
	$(RESULT_DIR)/mcf.inp.stderr $(SIM_DIR)/$(SIM_BIN) \
	$(SIM_KNOBS) $(SIM_DIR_SPEC2000INT)/181.mcf/exe/mcf_peak.ev6 \
	$(SIM_DIR_SPEC2000INT)/181.mcf/data/ref/input/inp.in

$(RESULT_DIR)/parser.ref.stdout: $(SIM_DIR)/$(SIM_BIN)
	mkdir ./words
	-cp $(SIM_DIR_SPEC2000INT)/197.parser/data/all/input/words/* ./words/
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/parser.ref.stdout \
	  $(RESULT_DIR)/parser.ref.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS) $(SIM_DIR_SPEC2000INT)/197.parser/exe/parser_peak.ev6 \
	  $(SIM_DIR_SPEC2000INT)/197.parser/data/all/input/2.1.dict \
	  -batch < $(SIM_DIR_SPEC2000INT)/197.parser/data/ref/input/ref.in
	  rm -f ./words/*
	  rmdir words

$(RESULT_DIR)/twolf.ref.stdout: $(SIM_DIR)/$(SIM_BIN)
	mkdir ./twolf
	-cp $(SIM_DIR_SPEC2000INT)/300.twolf/data/ref/input/ref* ./twolf/
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/twolf.ref.stdout \
	  $(RESULT_DIR)/twolf.ref.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS) $(SIM_DIR_SPEC2000INT)/300.twolf/exe/twolf_peak.ev6 \
	  ./twolf/ref
	rm -f ./twolf/*
	rmdir ./twolf
	
$(RESULT_DIR)/perlbmk.scrabbl.stdout: $(SIM_DIR)/$(SIM_BIN)
	-cp $(SIM_DIR_SPEC2000INT)/253.perlbmk/data/train/input/dictionary .
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/perlbmk.scrabbl.stdout \
	  $(RESULT_DIR)/perlbmk.scrabbl.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS) $(SIM_DIR_SPEC2000INT)/253.perlbmk/exe/perlbmk_peak.ev6 \
	  $(SIM_DIR_SPEC2000INT)/253.perlbmk/data/train/input/scrabbl.pl < \
	  $(SIM_DIR_SPEC2000INT)/253.perlbmk/data/train/input/scrabbl.in
	-rm -f dictionary

$(RESULT_DIR)/ammp.ref.stdout: $(SIM_DIR)/$(SIM_BIN)
	-cp $(SIM_DIR_SPEC2000FP)/188.ammp/data/ref/input/* .
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/ammp.ref.stdout \
	  $(RESULT_DIR)/ammp.ref.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS) $(SIM_DIR_SPEC2000FP)/188.ammp/exe/ammp_peak.ev6 \
	  < ammp.in

$(RESULT_DIR)/equake.ref.stdout: $(SIM_DIR)/$(SIM_BIN)
	-cp $(SIM_DIR_SPEC2000FP)/183.equake/data/ref/input/* .
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/equake.ref.stdout \
	  $(RESULT_DIR)/equake.ref.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS)  $(SIM_DIR_SPEC2000FP)/183.equake/exe/equake_peak.ev6 < \
	  inp.in

$(RESULT_DIR)/fma3d.ref.stdout: $(SIM_DIR)/$(SIM_BIN)
	-cp $(SIM_DIR_SPEC2000FP)/191.fma3d/data/ref/input/* .
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/fma3d.ref.stdout \
	$(RESULT_DIR)/fma3d.ref.stderr $(SIM_DIR)/$(SIM_BIN) \
	$(SIM_KNOBS)  $(SIM_DIR_SPEC2000FP)/191.fma3d/exe/fma3d_peak.ev6

$(RESULT_DIR)/mgrid.ref.stdout: $(SIM_DIR)/$(SIM_BIN)
	-cp $(SIM_DIR_SPEC2000FP)/172.mgrid/data/ref/input/* .
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/mgrid.ref.stdout \
	$(RESULT_DIR)/mgrid.ref.stderr $(SIM_DIR)/$(SIM_BIN) \
	$(SIM_KNOBS)  $(SIM_DIR_SPEC2000FP)/172.mgrid/exe/mgrid_peak.ev6 < \
	mgrid.in

$(RESULT_DIR)/swim.ref.stdout: $(SIM_DIR)/$(SIM_BIN)
	-cp $(SIM_DIR_SPEC2000FP)/171.swim/data/ref/input/* .
	-$(SIM_DIR)/redir.sh  $(RESULT_DIR)/swim.ref.stdout \
	  $(RESULT_DIR)/swim.ref.stderr $(SIM_DIR)/$(SIM_BIN) \
	  $(SIM_KNOBS) $(SIM_DIR_SPEC2000FP)/171.swim/exe/swim_peak.ev6 < \
	  swim.in
