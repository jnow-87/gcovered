#
# Copyright (C) 2020 Jan Nowotsch
# Author Jan Nowotsch	<jan.nowotsch@gmail.com>
#
# Released under the terms of the GNU GPL v2.0
#



obj-y := parser.tab.o lexer.lex.o process.o


$(loc_build_tree)/process.o: $(loc_build_tree)/parser.tab.c
$(loc_build_tree)/lexer.lex.o: $(loc_build_tree)/parser.tab.c
$(loc_build_tree)/parser.tab.o: $(loc_build_tree)/lexer.lex.c
