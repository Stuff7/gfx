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

break TableDir_parse
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

print_struct glyph
print_struct glyph.glyphs['A'].numPoints
print_ptr glyph.glyphs['A'].points glyph.glyphs['A'].numPoints
print_ptr glyph.glyphs['A'].endPtsOfContours glyph.glyf['A'].numberOfContours

# print_struct gdef
# print_struct gpos
# print_struct gsub
# print_struct os2

# print_struct cvt
# print_ptr cvt.instructions cvt.size

# print_struct fpgm
# print_ptr fpgm.instructions fpgm.size

# print_struct gasp
# print_ptr gasp.gaspRanges gasp.numRanges

# print_ptr glyph.cmap.encodingRecords glyph.cmap.numTables
# print_ptr glyph.cmap.subtable.startCode 10
# print_ptr glyph.cmap.subtable.endCode 10
# print_ptr glyph.cmap.subtable.idDelta 10
# print_ptr glyph.cmap.subtable.idRangeOffsets 10
# print_ptr glyph.loca.offsets glyph.loca.size
# I'd use glyph.maxp.numGlyphs for the size here but printing large arrays seems to break GDB
# print_ptr glyph.glyf 100
# print_ptr glyph.hmtx.hMetrics glyph.hhea.numberOfHMetrics
# print_ptr glyph.hmtx.leftSideBearings glyph.maxp.numGlyphs

# print_struct name
# print_ptr name.nameRecord name.count
print_ptr strings name.count
