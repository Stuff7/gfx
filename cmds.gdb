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
set logging overwrite on
set logging enabled on
show args

break TableDirParse
break src/main.c:66

run

define print_struct
  printf "\n$arg0 => "
  print $arg0
end

define print_ptr
  if $arg1 > 0
    printf "\n$arg0 => "
    print *$arg0@$arg1
  end
end

finish
print_struct table
continue

print_struct gdef
print_struct gpos
print_struct gsub
print_struct os2
print_struct cmap
print_ptr cmap.encodingRecords cmap.numTables
print_struct cvt
print_ptr cvt.instructions cvt.size
print_struct fpgm
print_ptr fpgm.instructions fpgm.size
print_struct gasp
print_ptr gasp.gaspRanges gasp.numRanges
print_struct glyph
print_ptr glyph.glyf glyph.maxp.numGlyphs
# print_ptr glyph.hmtx.hMetrics glyph.hhea.numberOfHMetrics
# print_ptr glyph.hmtx.leftSideBearings glyph.maxp.numGlyphs
print_ptr glyph.loca.offsets glyph.loca.size
print_struct name
print_ptr name.nameRecord name.count
# print_ptr strings name.count
