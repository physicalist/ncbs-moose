//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sat Dec 24 14:33:36 2005
 
include kkit {argv 1}
 
FASTDT = 5e-05
SIMDT = 0.1
CONTROLDT = 10
PLOTDT = 5
MAXTIME = 100
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 0
DEFAULT_VOL = 1.6667e-21
VERSION = 11.0
setfield /file/modpath value /home2/bhalla/scripts/modules
kparms
 
//genesis

initdump -version 3 -ignoreorphans 1
simobjdump doqcsinfo filename accessname accesstype transcriber developer \
  citation species tissue cellcompartment methodology sources \
  model_implementation model_validation x y z
simobjdump table input output alloced step_mode stepsize x y z
simobjdump xtree path script namemode sizescale
simobjdump xcoredraw xmin xmax ymin ymax
simobjdump xtext editable
simobjdump xgraph xmin xmax ymin ymax overlay
simobjdump xplot pixflags script fg ysquish do_slope wy
simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \
  link savename file version md5sum mod_save_flag x y z
simobjdump geometry size dim shape outside xtree_fg_req xtree_textfg_req x y \
  z
simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable \
  geomname xtree_fg_req xtree_textfg_req x y z
simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z
simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \
  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z
simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \
  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z
simobjdump xtab input output alloced step_mode stepsize notes editfunc \
  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z
simobjdump kchan perm gmax Vm is_active use_nernst notes xtree_fg_req \
  xtree_textfg_req x y z
simobjdump transport input output alloced step_mode stepsize dt delay clock \
  kf xtree_fg_req xtree_textfg_req x y z
simobjdump proto x y z
simundump geometry /kinetics/geometry 0 1.6667e-21 3 sphere "" white black 0 \
  0 0
simundump group /kinetics/MAPK 0 yellow black x 0 0 "" MAPK \
  /home2/bhalla/scripts/modules/MAPK_0.g 0 0 0 1 10 0
simundump kpool /kinetics/MAPK/MKKK 0 0 0.1 0.1 0.1 0.1 0 0 1 0 \
  /kinetics/geometry 16 yellow -8 5 0
simundump kpool /kinetics/MAPK/MKKK-P 0 0 0 0 0 0 0 0 1 0 /kinetics/geometry \
  51 yellow 0 5 0
simundump kpool /kinetics/MAPK/Ras-MKKKK 0 0 0.001 0.001 0.001 0.001 0 0 1 0 \
  /kinetics/geometry 47 yellow 6 8 0
simundump kenz /kinetics/MAPK/Ras-MKKKK/1 0 0 0 0 0 1 1250 10 2.5 0 1 "" red \
  47 "" -4 8 0
simundump xgraph /graphs/conc1 0 0 100 0 0.3 0
simundump xgraph /graphs/conc2 0 0 100 4.5157e-05 0.3 0
simundump xplot /graphs/conc2/Ras-MKKKK.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc2/MKKK.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 16 0 0 1
simundump xgraph /moregraphs/conc3 0 0 100 0 1 0
simundump xgraph /moregraphs/conc4 0 0 100 0 1 0
simundump xcoredraw /edit/draw 0 -10 10 -3 12
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"22 Jan 2002" \
" " \
" This model is based on Kholodenko, B.N." \
"      Eur. J. Biochem. 267, 1583-1588(2000)" \
""
addmsg /kinetics/MAPK/Ras-MKKKK/1 /kinetics/MAPK/MKKK REAC sA B 
addmsg /kinetics/MAPK/Ras-MKKKK/1 /kinetics/MAPK/MKKK-P MM_PRD pA 
addmsg /kinetics/MAPK/Ras-MKKKK /kinetics/MAPK/Ras-MKKKK/1 ENZYME n 
addmsg /kinetics/MAPK/MKKK /kinetics/MAPK/Ras-MKKKK/1 SUBSTRATE n 
addmsg /kinetics/MAPK/Ras-MKKKK /graphs/conc2/Ras-MKKKK.Co PLOT Co *Ras-MKKKK.Co *47 
addmsg /kinetics/MAPK/MKKK /graphs/conc2/MKKK.Co PLOT Co *MKKK.Co *16 
enddump
// End of dump

call /kinetics/MAPK/notes LOAD \
"This is the oscillatory MAPK model from Kholodenko 2000" \
"Eur J. Biochem 267:1583-1588" \
"The original model is formulated in terms of idealized" \
"Michaelis-Menten enzymes and the enzyme-substrate complex" \
"concentrations are therefore assumed negligible. The" \
"current implementation of the model uses explicit enzyme" \
"reactions involving substrates and is therefore an" \
"approximation to the Kholodenko model. The approximation is" \
"greatly improved if the enzyme is flagged as Available" \
"which is an option in Kinetikit. This flag means that the" \
"enzyme protein concentration is not reduced even when it" \
"is involved in a complex. However, the substrate protein" \
"continues to participate in enzyme-substrate complexes" \
"and its concentration is therefore affected. Overall," \
"this model works almost the same as the Kholodenko model" \
"but the peak MAPK-PP amplitudes are a little reduced and" \
"the period of oscillations is about 10% longer." \
"If the enzymes are  not flagged as Available then the" \
"oscillations commence only when the Km for enzyme 1" \
"is set to 0.1 uM."
call /kinetics/MAPK/MKKK/notes LOAD \
"The total concn. of MKKK is 100nM " \
"from" \
"Kholodenko, 2000"
call /kinetics/MAPK/MKKK-P/notes LOAD \
"This is the phosphorylated form of MKKK which converts MKK" \
"to MKK-P and then to MKK-PP" \
"from" \
"Kholodenko, 2000."
call /kinetics/MAPK/Ras-MKKKK/notes LOAD \
"The concn. of Ras-MKKKK* is set to 1 nM implicitly" \
"from" \
"Kholodenko, 2000"
call /kinetics/MAPK/Ras-MKKKK/1/notes LOAD \
"The Km is 10nM and Vmax is 2.5nM sec^-1.  We  assume that" \
"there is 1 nM of the Ras-MKKKK." \
"From Kholodenko, 2000." \
"" \
"If the enzymes are not flagged as Available, then this" \
"Km should be set to 0.1 to obtain oscillations."
complete_loading
