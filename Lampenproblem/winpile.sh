chmod +x compile.sh
#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncurses -O2 -march=native
#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -O2 -march=native
#x86_64-w64-mingw32-g++ -o Wampenproblem.exe Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -static -O2 -march=native -I/usr/include/ -I/usr/include/x86_64-linux-gnu/
#x86_64-w64-mingw32-g++ -o Wampenproblem.exe Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -static -O2 -march=native -I/usr/include/x86_64-linux-gnu/
#x86_64-w64-mingw32-g++ -o Wampenproblem.exe Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -static -O2 -march=native -I/usr/include/features.h -I/usr/include/x86_64-linux-gnu/
#x86_64-w64-mingw32-g++ -o Wampenproblem.exe Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -static -O2 -march=native -I/usr/incmgw/
#x86_64-w64-mingw32-g++ -o Wampenproblem.exe Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -static -O2 -march=native -I/usr/incmgw/ -I/usr/share/mingw-w64/include/
#x86_64-w64-mingw32-g++ -o Wampenproblem.exe Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -static -O2 -march=native -I/usr/xinclude -I/usr/share/mingw-w64/include/ -I/usr/mgw/
x86_64-w64-mingw32-g++ -o Wampenproblem.exe Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -static -O2 -march=native -I/usr/share/mingw-w64/include/ -I/usr/mgw/ -I/usr/xinclude
#strip Wampenproblem.exe
#cp -v Wampenproblem.exe ./testumgebung/
