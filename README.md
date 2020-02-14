# Anarchy Online Patch Project

Patch to fix various bugs and add quality of life improvements to the game. Since the game appears to be on life support I thought I would contribute something along the lines of what clicksaver, Anarchy Online Item Assistant Plus, et al have done for the community.

## Features

* Framerate cap is raised from 100fps to 144fps (ctrl+alt+f to show fps meter)
* Flash window navbar and titlebar when invited to a group or battlestation queue pops
* Numpad keys work correctly when typing in chat or the gmi interface
* Autorun hotkey has been improved
  * Hotkey now acts as a toggle as is commmon in all other mmos
  * You can now press the hotkey while manually running foward to engage autorun without having to stop
* When interacting with ingame help bots automatically open the text responses they send you
  * Demo here (https://i.imgur.com/8MfB3w0.mp4)
  * Only opens text links when the last message you sent was to the bot
* Bots will no longer appear in your reply list, /r and shift+r have a use again

## Bugs Fixed

* Crash to desktop when opening the map
  * Crash is caused by memory fragmentation causing malloc to fail when attempting to allocate enough memory for the map file
  * Workaround is to enable the `largeaddressaware` flag in the AnarchyOnline executable
  * If we could get the source of the map, we could also try splitting the map into smaller submaps as was done by bitnykk for his shadowlands map
  
* Mouse buttons 4 and 5 (Xbutton1 and Xbutton2 )double click is converted to single click since you cannot bind actions ingame to double clicks
  * This means rapid pressing button 4 and 5 no longer produce dead presses that result in nothing happening

## Planned features

* More hotkeys
  * I'd like to add hotkeys for interacting with the trade skill window, move item to slot1, move item to slot2, complete tradeskill, clear tradeskill
  * Move item to backpack hotkey, move currently picked up item to backpack
  * Anything that would reduce stress caused by repetitive motion really
  
* Logging
  * Currently there is no logging so if something doesn't work right there's no way to know what went wrong
* Configuration
  * Currently there is no way to configure which patches you want enabled or disabled
  * Considering adding a / command or just having a ini file that you can edit and have the results applied in realtime
  
* Improve Map crash logic
  * Currently the patcher launches a copy of the AnarchyOnline.exe that is created in the appdata folder so that the checksum error does not occur when running multiple copies of AO
  
* Tab targeting filter
  * It'd be nice to filter out targets that would instantly kill you. Spririts in inf missions, towers in sector 35
  
* Have xp bar show progress to next research target when research is set to 100%

* Flash titlebar/taskbar/play sound when receiving a private message
  * Need to setup a way to add filters for this so darknet, raidbot private messages do not trigger it

## System requirements

* MS Visual C++ Redistributable for Visual Studio 2019 (https://aka.ms/vs/16/release/VC_redist.x86.exe)


## Download

You can download releases from the [release page][releases].


## How to use

This project is aimed at the old client, while most of the patches should work on the beta client they have not been tested as extensively.

To use this plugin:
* Make sure the game is not running
* Drop `mswsock.dll` into Anarchy Online's base directory (next to Anarchy.exe and AnarchyOnline.exe) 

To uninstall simply delete `mswsock.dll`.

## Troubleshooting

More details coming soon.

## How to build

These instructions are only relevant to you if you want to modify the plugin. To install it, go to the [release page][releases] instead.

* Install Visual Studio 2019 (earlier versions might work as well but haven't been tested) 
* Checkout source codes with git, or download source code as ZIP and extract.
* Use vcpkg (https://github.com/microsoft/vcpkg) to install the required libraries
```
vckpg update
vckpg install polyhook2:x86-windows-static
vckpg install gumbo:x86-windows-static
```

Once finished, the plugin file `mswsock.dll` will appear in the `Release` folder. 
You have to compile in release mode otherwise the data structures of the standard library will not align correctly.

## License

GPL v3 license. See LICENSE.txt for details.

[releases]: https://github.com/xeropresence/AOPP/releases
