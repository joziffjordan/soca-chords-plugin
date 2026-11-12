# Soca Chords

A chord-progression browser for modern soca. Cycle through 123 progressions, hear them, then drag the MIDI straight onto a track.

- **Formats:** VST3 (Mac + Windows), AU (Mac), standalone app
- **Styles:** Power Soca, Groovy Soca, Bouyon, Dennery Segment, Afro-Soca, Jab Soca, Chutney Soca, Zess / Steam
- **Controls:** key, major/minor filter, 9 rhythm patterns, octave, optional bass note, built-in preview sound
- **Look:** carnival theme, with a piano keyboard that lights up the notes of each chord

## Getting the plugin (no coding needed)

1. Create a free account at github.com and make a new empty repository.
2. Upload everything in this folder to it (including the hidden `.github` folder).
3. Open the repository's **Actions** tab. A build called "Build Soca Chords" runs by itself and takes about 10 minutes.
4. Open the finished run and download **SocaChords-macOS** and/or **SocaChords-Windows** from the Artifacts section.

## Installing

**Mac**
- Copy `Soca Chords.vst3` to `~/Library/Audio/Plug-Ins/VST3/`
- Copy `Soca Chords.component` to `~/Library/Audio/Plug-Ins/Components/`
- The build is not signed by Apple, so macOS will block it the first time. Open Terminal and run:
  `xattr -cr ~/Library/Audio/Plug-Ins/VST3/"Soca Chords.vst3" ~/Library/Audio/Plug-Ins/Components/"Soca Chords.component"`

**Windows**
- Copy the `Soca Chords.vst3` folder to `C:\Program Files\Common Files\VST3\`

Then in Ableton: Settings > Plug-Ins > turn on VST3 (and Audio Units on Mac) > Rescan. Soca Chords appears under Plug-Ins as an instrument.

## Using it

1. Drop Soca Chords on a MIDI track.
2. Pick a style, scale and key. Use `<` `>` (or the arrow keys) to cycle; **Random** jumps anywhere in the filtered list.
3. Press **Play** to hear it. Tempo follows the DAW, and when the DAW is playing the chords lock to its bars.
4. Choose a rhythm pattern, octave and whether you want a bass note.
5. Drag from the **DRAG MIDI** tile onto any MIDI track. The clip contains exactly what you heard, in your key, at the project tempo.

**Hear it through your own instrument:** the chords also leave the plugin as live MIDI. In Ableton, on a second MIDI track set *MIDI From* to the Soca Chords track, pick "Soca Chords" in the chooser below it, set Monitor to *In*, and untick **Preview sound**.

## Adding your own progressions

Open `Source/Library.h`. Each progression is one line, e.g.

    { "My Riddim", Min, Bouyon | Dennery, "i VII VI@2 VII@2" },

The rules are at the top of that file. Push the change to GitHub and a new build is made automatically.

## Notes

- The library is a curated set of the progressions that dominate soca from 2016 on, named by feel rather than by song. It is not a transcription of every release.
- Building locally instead: install CMake and a compiler (Xcode or Visual Studio), then `cmake -B build` and `cmake --build build --config Release`.
- Built with JUCE 8 (free for personal use under its AGPL/Starter terms).
