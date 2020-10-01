# AE_Bot

This is a bot for Another Eden. It has several running modes
1) Fishing mode: It completely automates the entire fishing process, including returning to the fish vendor, traveling to different fishing spots and fighting fishing horrors
2) Grinding mode: It completely automates the grinding in the overworld, including returning to the spacetime rift to restore, traveling to different grinding locations and using skills to kill mobs / horrors
3) Hitting bell mode: It can hit the bell in Nagsham up to 999 hits or hit up to 31 hits and repeat the loop
4) Jumping rope mode: It can jump rope in either Baruoki or Ratle endless
5) Separating grasta mode: It can separate specified grasta in Izana
6) Screen capture mode: It can capture the screen

# How to run it
It runs with emulator. Both Nox player and LD Player is suppported.

Finish the story up to 2.2 so that all the areas are open.

I suggest to set emulator at least 1280 x 720. Because I use OpenCV to catch the image for comparison, any resolution lower than 1280 x 720 might not make the bot work.

# How to start it
First, open up the config_setup.txt file and read through it. This is the main config file to set the running mode. 
The bot needs to be able to find the window the game is running in, which it does so with the window name. You can find this in the top left corner of the window (see screenshot where my window name is "Suz2"). By default this is "LDPlayer", but you may have renamed it. Put this in as your "Window Name" in the config file.

There are several other sub configuration files for different modes:
1) config_fishing is for fishing related setting. The instructions should be pretty self explanatory 
2) config_path is for path configuration for grinding or fishing. You don't need to change it unless you want to add another path for your own grinding. If you add it, you just make sure you follow the similar pattern of path, and add this path to config_setting to enable grinding
3) config_setting is for the major configuration. The instructions should be pretty self explanatory. 

Once that's done, start your emulator. The emulator itself needs to be set to at least around 1280x720 resolution; this is because the bot is reading the screen, so if its hard to read for you, its hard to read for the bot as well. Feel free to experiment but if the bot starts to act odd, you've most likely chosen too low of a resolution. From here, you're free to resize the window, however the same as before applies; the smaller you make the window, the harder it will be for the bot to successfully read it. 1280x720 is what I recommend as that's the one I've tested everything on, although other resolutions may work.

Now start your game and get to the normal state where you're free to run around. The menu should not be open. Make sure the emulator is not minimized. The other thing to remember is you cannot minimize the emulator. Why? When minimized, the window no longer renders, so the bot is unable to take screenshots. So don't minimize, just put it behind another window. You also cannot resize the window after starting the bot. There are certain calculations done at the start of the bot to account for screen size that are never done again. If you need to resize the window, you need to restart the bot afterwards.

Based on the different modes you want to use, you can start the bot at the respective place. Idealy you should start the bot in a Commander with admin rights. Please read config_setup.txt for different starting point of the bot for different modes
