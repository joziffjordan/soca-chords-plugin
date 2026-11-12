// Runs the audio engine without a host: checks MIDI out is balanced and the preview synth makes sound.
#include "../Source/PluginProcessor.h"
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
#include <iostream>

int main()
{
    juce::ScopedJuceInitialiser_GUI init;
    std::unique_ptr<juce::AudioProcessor> base (createPluginFilter());
    auto* p = dynamic_cast<SocaChordsProcessor*> (base.get());
    const double sr = 48000.0; const int block = 512;
    p->setPlayConfigDetails (0, 2, sr, block);
    p->prepareToPlay (sr, block);
    int failures = 0;

    for (int prog : { 0, 23, 70, 100 })
        for (int rhythm = 0; rhythm < soca::kNumRhythms; ++rhythm)
        {
            p->progression = prog; p->rhythm = rhythm; p->notesChanged(); p->playing = true;
            int ons = 0, offs = 0, held[128] = {}; float peak = 0; bool stuck = false;
            juce::AudioBuffer<float> buf (2, block); juce::MidiBuffer midi;
            const int blocks = (int) (sr * 60.0 / 120.0 * 40.0 / block);   // 40 beats at 120 bpm: > 2 loops
            for (int b = 0; b < blocks + 20; ++b)
            {
                if (b == blocks) p->playing = false;
                midi.clear(); p->processBlock (buf, midi);
                for (const auto meta : midi)
                {
                    auto m = meta.getMessage();
                    if (m.isNoteOn())  { ++ons;  if (++held[m.getNoteNumber()] > 1) stuck = true; }
                    if (m.isNoteOff()) { ++offs; --held[m.getNoteNumber()]; }
                }
                peak = juce::jmax (peak, buf.getMagnitude (0, block));
            }
            int hanging = 0; for (int h : held) hanging += h;
            const bool ok = ons > 0 && ons == offs && hanging == 0 && ! stuck && peak > 0.01f && peak < 1.0f;
            if (! ok) ++failures;
            std::cout << (ok ? "ok   " : "FAIL ") << soca::kLibrary[prog].name << " / " << soca::kRhythms[rhythm].name
                      << "  ons " << ons << " offs " << offs << " hanging " << hanging << " overlap " << stuck << " peak " << peak << "\n";
        }
    std::cout << failures << " failures\n";
    return failures;
}
