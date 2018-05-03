# final_year_synthesiser

If you are using Windows and already know how to load and use a VST plugin, and have a VST host on your computer, please go to the /App directory and find the "FYP.dll" file. This is the preferred way of using the software as it is designed to be a plugin. If you wish to try a DAW, download FL Studio 12 demo version from the Image-Line website. Otherwise:

1. Run the "FYP.exe" executable found in the /App directory.
2. In the synth's top left corner, click Options->Audio/MIDI settings..., and follow the next instructions.
3. If you are using Windows, change the Audio device type to Windows Audio. Otherwise choose the audio device that works best for you.
4. Set the audio buffer size to the maximum that is available.
5. Set the output to the speakers/headphones of your choice.
6. Close the Options menu.

You have set up the synth! You can use the PC keyboard to play notes: the valid keys are the middle row of the keyboard from the 'A' character to the ';' character. These correspond to white notes on a piano. The black notes are buttons a row above the middle row, with a few gaps that correspond to the gaps of a piano. Alternatively, you can click buttons on the synth's graphical keyboard to play notes.

The Options button in the top left of the window allows you to save and load a plugin state. I have provided some example states in the /App/presets folder that can be loaded into the plugin, showcasing some of the capabilities of the synth.

NB: if you are running the executable instead of the VST plugin, you may encounter minor bugs, eg:
-interacting with the synth's drop-down menu while holding down a note/s may cause the note/s to freeze. In this case, click on the note/s on the graphical keyboard that are frozen in order to release them.
-If the synth does not respond to keyboard presses, you may be pressing an invalid key. Otherwise click the graphical keyboard to reenable keyboard input.
