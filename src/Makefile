CFLAGS = -Wall -Wextra -Werror
INSTALL_FILES = brick_game/tetris/logic/tetris.c gui/cli/console_front.c brick_game/tetris/controller/cli/controller.c common/common.c -lncurses
TEST_FILES = test/test.c brick_game/tetris/logic/tetris.c common/common.c gui/cli/console_front.c -lcheck -lncurses -fsanitize=address
TEST_FILES_SNEK = test/snek_test.cpp brick_game/snake/logic/snake.cpp common/common.c -lgtest -fsanitize=address
GCOV_FILES = test/test.c brick_game/tetris/logic/tetris.c common/common.c -lcheck
GCOVFLAGS = -fprofile-arcs -ftest-coverage --coverage
DIST_DIR = s21_BrickGame

INSTALL_SNEK_CLI = brick_game/snake/logic/snake.cpp gui/cli/console_front.c brick_game/snake/controller/cli/controller.cpp common/common.c -lncurses
RACE_CONTROLLERS_DIR = brick_game/race/controller

.PHONY: test

UNAME_S = $(shell uname)

ifeq ($(UNAME_S),Darwin)
OPEN_CMD = open
endif

ifeq ($(UNAME_S),Linux)
OPEN_CMD = xdg-open
ADD_LDFLAGS = -lm -lsubunit
endif

all: install

install: install_snek install_snek_gui install_tetris_gui install_tetris install_race install_race_gui install_race_web install_race_gui_web install_server install_web_gui

install_tetris:
	mkdir -p build
	gcc $(CFLAGS) $(INSTALL_FILES) $(ADD_LDFLAGS) -o build/Tetris
	echo 0 > build/max_score_tetris.txt

install_snek:
	mkdir -p build
	g++ $(CFLAGS) $(INSTALL_SNEK_CLI) $(ADD_LDFLAGS) -fsanitize=address -g -o build/Snek
	echo 0 > build/max_score_snek.txt

install_race:
	mkdir -p build
	go build -o build/Race $(RACE_CONTROLLERS_DIR)/cli/controller.go
	echo 0 > build/max_score_race.txt

install_snek_gui:
	mkdir -p build
	cd brick_game/snake/controller/gui && qmake controller.pro && make && make clean && mv snake_gui ../../../../build
	echo 0 > build/max_score_snek.txt

install_tetris_gui:
	mkdir -p build
	cd brick_game/tetris/controller/gui && qmake controller.pro && make && make clean && mv tetris_gui ../../../../build
	echo 0 > build/max_score_tetris.txt

install_race_gui:
	mkdir -p build
	cd $(RACE_CONTROLLERS_DIR)/gui && go build -o libgame.a -buildmode=c-archive adapter.go && qmake controller.pro && make && make clean && mv race_gui ../../../../build
	echo 0 > build/max_score_race.txt

install_race_web:
	mkdir -p build
	go build -o build/Race-web $(RACE_CONTROLLERS_DIR)/cli-web/controller.go
	echo 0 > build/max_score_race.txt

install_race_gui_web:
	mkdir -p build
	cd $(RACE_CONTROLLERS_DIR)/gui-web && go build -o libgame.a -buildmode=c-archive adapter.go && qmake controller.pro && make && make clean && mv race_gui ../../../../build
	echo 0 > build/max_score_race.txt

install_server:
	mkdir -p build
	echo 0 > build/max_score_tetris.txt
	echo 0 > build/max_score_snek.txt
	echo 0 > build/max_score_race.txt
	cd server/controllers && g++ -c -std=c++14 -I../../brick_game/snake/logic -I../../common snake_adapter.cpp ../../brick_game/snake/logic/snake.cpp ../../common/common.c && ar rcs libsnake_adapter.a snake_adapter.o snake.o common.o && cd ../start && go build -o server main.go && mv server ../../build/

install_web_gui:
	mkdir -p build
	cd gui/web && go build -o web_gui server.go && mv web_gui ../../build/web_gui

uninstall:
	rm -rf build

tests: test_snake test_tetris

test_tetris:
	echo 0 > test/max_score_tetris.txt
	gcc -g $(CFLAGS) $(TEST_FILES) $(ADD_LDFLAGS) -o test/test
	cd test && ./test

test_snake:
	echo 0 > test/max_score_snek.txt
	g++ -g $(CFLAGS) $(TEST_FILES_SNEK) $(ADD_LDFLAGS) -o test/test
	cd test && ./test

test_race:
	cd brick_game/race/logic && go test

gcov_report_tetris:
	mkdir -p gcov
	echo 0 > gcov/max_score_tetris.txt
	gcc $(CFLAGS) $(GCOVFLAGS) $(GCOV_FILES) $(ADD_LDFLAGS) -o ./gcov/gcov_test
	cd gcov && ./gcov_test
	mv *.gcno *.gcda ./gcov
	lcov --capture --directory ./gcov --output-file ./gcov/coverage.info
	genhtml -o ./gcov/report ./gcov/coverage.info
	$(OPEN_CMD) ./gcov/report/index.html

gcov_report_snake:
	mkdir -p gcov
	echo 0 > gcov/max_score_snek.txt
	g++ $(CFLAGS) $(GCOVFLAGS) $(TEST_FILES_SNEK) $(ADD_LDFLAGS) -o ./gcov/gcov_test
	cd gcov && ./gcov_test
	lcov --rc geninfo_unexecuted_blocks=1 --capture --directory ./gcov --output-file ./gcov/coverage.info --ignore-errors mismatch
	genhtml -o ./gcov/report ./gcov/coverage.info
	$(OPEN_CMD) ./gcov/report/index.html

dvi:
	doxygen Doxyfile

dist:
	mkdir -p $(DIST_DIR)
	cp -a brick_game $(DIST_DIR)
	cp -a gui $(DIST_DIR)
	cp -a test $(DIST_DIR)
	cp -a *.c $(DIST_DIR)
	cp -a *.h $(DIST_DIR)
	cp -a common $(DIST_DIR)
	cp -a Documentation.html $(DIST_DIR)
	tar -czf s21_Tetris.tar.gz $(DIST_DIR)
	rm -rf $(DIST_DIR)
	
clang_format:
	cp ../../../materials/linters/.clang-format ../../brick_game/tetris/.clang-format
	clang-format -i *.c *.h
	clang-format -i ../../gui/cli/*.c ../../gui/cli/*.h
	clang-format -i ../../test/*.c ../../test/*.h
	rm -f .clang-format

clang_check:
	cp ../../../materials/linters/.clang-format ../../brick_game/tetris/.clang-format
	clang-format -n *.c *.h
	clang-format -n ../../gui/cli/*.c ../../gui/cli/*.h
	clang-format -n ../../test/*.c ../../test/*.h
	rm -f .clang-format

leaks_game: install
	cd build && leaks -atExit -- ./Tetris

leaks_test: test
	cd ../../test && leaks -atExit -- ./test

clean:
	-cd test && rm -f test && rm -f *.txt
	-rm -rf gcov
	-rm -f s21_Tetris.tar.gz
	-rm -r html
	-rm -r latex
	-cd brick_game/tetris/controller/gui && make clean
	-cd brick_game/snake/controller/gui && make clean
	-cd brick_game/race/controller/gui && make clean
	-cd brick_game/race/controller/gui-web && make clean
	-cd server/controllers && rm *.o && rm *.a
