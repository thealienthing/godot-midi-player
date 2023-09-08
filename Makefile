
FSDEPS := -I/opt/homebrew/Cellar/readline/8.2.1/include -I/opt/homebrew/Cellar/fluid-synth/2.3.3/include -I/opt/homebrew/Cellar/glib/2.76.4/include -I/opt/homebrew/Cellar/glib/2.76.4/include/glib-2.0 -I/opt/homebrew/Cellar/glib/2.76.4/lib/glib-2.0/include -I/opt/homebrew/opt/gettext/include -I/opt/homebrew/Cellar/pcre2/10.42/include -I/opt/homebrew/Cellar/libsndfile/1.2.2/include -I/opt/homebrew/Cellar/flac/1.4.3/include -I/opt/homebrew/Cellar/libvorbis/1.3.7/include -I/opt/homebrew/Cellar/libogg/1.3.5/include -I/opt/homebrew/Cellar/opus/1.4/include/opus -I/opt/homebrew/Cellar/mpg123/1.31.3/include -I/opt/homebrew/Cellar/portaudio/19.7.0/include -L/opt/homebrew/Cellar/fluid-synth/2.3.3/lib -lfluidsynth
MIDI_OUT := "midifile-example"
BIN = ./bin


all: | $(BIN) midifile fluidsynth both

midifile: | $(BIN)
	g++ -o $(BIN)/midifile-example midifile-example.cpp --std=c++11 -I./midifile/include -L./lib -lmidifile

runmidi: midifile
	$(BIN)/midifile-example re/Vampire_Killer_1.mid

fluidsynth: | $(BIN)
	gcc fluidsynth-example.c -o $(BIN)/fluidsynth-example ${FSDEPS}

runfluid:
	$(BIN)/fluidsynth-example

both: | $(BIN)
	g++ -o ${BIN}/midi-fluidsynth midi-fluidsynth-example.cpp --std=c++11 -I./midifile/include -L./lib -lmidifile ${FSDEPS}

runboth:
	./midi-fluidsynth res/Vampire_Killer_1.mid

clean:
	rm -rf bin

$(BIN):
	@echo "Creating bin"
	mkdir -p bin
