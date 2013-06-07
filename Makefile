cf = -Wall -g
ld = bin/libLeap.dylib -framework SDL -framework OpenGL -framework Cocoa -framework Foundation

INCLUDES = -I/usr/include/GL
linuxflag = -g -O2 -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -DHAVE_OPENGL -lGL -lm -lGLU
libs = -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -lpthread
leapmotion = ./bin/libLeap.so
leapx86 = ./bin/libLeapx86.so

out = bin/flythrough

flythrough:
	cd obj; \
	gcc $(cf) -c ../src/scene/*.c ../src/util/*.c; \
	g++ $(cf) -c ../src/*.cpp
ifeq ($(shell uname), Darwin)
	cd obj; \
	gcc $(cf) -c ../src/macosx.m
	g++ $(cf) -o $(out) obj/*.o $(ld)
	install_name_tool -change @loader_path/libLeap.dylib @executable_path/libLeap.dylib $(out)
	install_name_tool -change @rpath/SDL.framework/Versions/A/SDL @executable_path/SDL $(out)
	install_name_tool -change @rpath/SDL_ttf.framework/Versions/A/SDL_ttf @executable_path/SDL_ttf $(out)
else
	g++ $(cf) -o $(out) $(INCLUDES) obj/*.o $(leapmotion) $(linuxflag) $(libs)
endif

x86:
	cd obj; \
	gcc $(cf) -c ../src/scene/*.c ../src/util/*.c; \
	g++ $(cf) -c ../src/*.cpp
	g++ $(cf) -o $(out) $(INCLUDES) obj/*.o $(leapx86) $(linuxflag) $(libs)

init:
	mkdir obj

clean:
	rm -R $(out) obj/*.o
