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

break src/main.c:24
break GDEFTableParse
break GPOSTableParse
break OS2TableParse
break CmapTableParse
break CvtTableParse
break FpgmTableParse
break GaspTableParse
break HeadTableParse
break LocaTableParse
break MaxpTableParse
break NameTableParse

run

finish
printf "\nGDEF\n"
print gdef
continue

finish
printf "\nGPOS\n"
print gpos
continue

finish
printf "\nGSUB\n"
print gsub
continue

finish
printf "\nOS/2\n"
print os2
continue

finish
printf "\ncmap\n"
print cmap
print *cmap.encodingRecords@cmap.numTables
continue

finish
printf "\ncvt\n"
print cvt
print *cvt.instructions@cvt.size
continue

finish
printf "\nfpgm\n"
print fpgm
print *fpgm.instructions@fpgm.size
continue

finish
printf "\ngasp\n"
print gasp
print *gasp.gaspRanges@gasp.numRanges
continue

finish
printf "\nhead\n"
print head
continue

finish
printf "\nloca\n"
print loca
continue

finish
printf "\nmaxp\n"
print maxp
continue

finish
printf "\nname\n"
print name
print *name.nameRecord@name.count
# print *name.strings@name.count
continue

printf "\nTable\n"
print table
print *table.tableRecords@table.numTables
continue
