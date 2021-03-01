#include <Midifile/MidiFile.h>

#include <SDL2/SDL.h>

#include <VSSynth/VSSynth.h>

#include "MIDIPatches.h"
#include "MIDISequencer.h"

#include <algorithm>
#include <vector>

void addNotesToSequencer(std::vector<MIDISequencer> &seqs, smf::MidiFile &file)
{
    int tracks = file.getTrackCount();

    for (int track = 0; track < tracks; track++)
    {
        for (int event = 0; event < file[track].size(); event++)
        {
            double startTime = file[track][event].seconds;
            int channel = file[track][event].getChannelNibble();

            smf::MidiEvent midiEvent = file[track][event];

            if (midiEvent.isNoteOn())
            {
                double duration = file[track][event].getDurationInSeconds();
                int note = file[track][event].getP1();
                int velocity = file[track][event].getP2();

                seqs[channel].addNotePlayEvent(note, velocity, startTime, duration);
            }
            else if(midiEvent.isAftertouch())
            {
                int note = file[track][event].getP1();
                int pressure = file[track][event].getP2();

                seqs[channel].addNotePressureEvent(note, pressure, startTime);
            }
            else if(midiEvent.isController())
            {
                int controllerNum = file[track][event].getP1();
                int value = file[track][event].getP2();

                seqs[channel].addControllerEvent(controllerNum, value, startTime);
            }
            else if (midiEvent.isPatchChange())
            {
                int patch = file[track][event].getP1();

                seqs[channel].addProgramChangeEvent(patch, startTime);
            }
            else if(midiEvent.isAftertouch())
            {
                int velocity = file[track][event].getP1();

                seqs[channel].addChannelPressureEvent(velocity, startTime);
            }
            else if(midiEvent.isPitchbend())
            {
                int lsb = file[track][event].getP1();
                int msb = file[track][event].getP2();
                
                seqs[channel].addPitchBendEvent(lsb, msb, startTime);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    smf::MidiFile midifile;
    if (argc <= 1)
    {
        midifile.read("../../midis/tmnt400.mid");
    }
    else
    {
        midifile.read(argv[1]);
    }

    midifile.doTimeAnalysis();
    midifile.linkNotePairs();

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

    SDL_Window *window;
    window = SDL_CreateWindow("MIDI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 900, SDL_WINDOW_SHOWN);

    std::vector<MIDISequencer> sequencers;
    for (int i = 0; i < 16; i++)
    {
        MIDISequencer seq(new MIDIChannel(
            VSSynth::Patches::GLOCKENSPIEL,
            VSSynth::Patches::GLOCKENSPIEL_ENVELOPE));
        sequencers.push_back(seq);
    }

    addNotesToSequencer(sequencers, midifile);

    VSSynth::Middleware::WAVWriter wavWriter(24000, 2);
    wavWriter.open("MIDI_OUT.wav");

    VSSynth::Synthesizer synth(24000, 50);
    synth.open();
    for (int i = 0; i < 16; i++)
    {
        sequencers[i].setVolume(50);
        sequencers[i].sortEventsByTime();
        synth.addSoundGenerator(&sequencers[i]);
    }
    synth.addMiddleware(&wavWriter);
    synth.unpause();

    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) > 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
        }
    }

    synth.pause();
    synth.close();

    wavWriter.close();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}