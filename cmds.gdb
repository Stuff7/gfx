set history save
set verbose off
set print pretty on
set print array off
set print array-indexes on
set python print-stack full

set width 0
set height 0
set debuginfod enabled off
set logging file logs/gdb_output
set logging enabled on
show args

break TableDirParse
break src/main.c:52

run

define print_struct
  printf "\n%s\n", $arg0
  print $arg1
end

finish
print_struct "Table" table
continue

print_struct "GDEF" gdef
print_struct "GPOS" gpos
print_struct "GSUB" gsub
print_struct "OS/2" os2
print_struct "cmap" cmap
print *cmap.encodingRecords@cmap.numTables
print_struct "cvt" cvt
print *cvt.instructions@cvt.size
print_struct "fpgm" fpgm
print *fpgm.instructions@fpgm.size
print_struct "gasp" gasp
print *gasp.gaspRanges@gasp.numRanges
print_struct "loca" loca
print_struct "maxp" maxp
print_struct "name" name
print *name.nameRecord@name.count
print_struct "Strings" *strings@name.count
