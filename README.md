# NodeUI

[![Launcher on my desktop](http://i.imgur.com/2w28m6t.jpg?1)](https://www.youtube.com/watch?v=ePnHs1_Y8Jg)

NodeUI is a launcher application inspired by Android's lock screen pattern. You can designate different paths through the nodes to activate different applications.

For example, in the video, the path `Down Right -> Up -> Left` launches `gnome-calculator`, while `Down Right -> Up -> Up` would launch `gnome-screenshot`. I have commonly-used applications (`gnome-terminal`, `nautilus`, `chromium`) mapped to shorter paths (`Up`, `Up Right`, `Down`). The list of applications can be configured via `assets/config/applications.json`. For now, the current `applications.json` just has my testing configuration.

The keybindings can be modified via `assets/config/config.json`. The current control scheme is a mix of numpad, nethack (vi), WASD, and arrow keys. You can also configure the hotkey to open the launcher. In the future, I plan to add a couple esoteric input mechanisms that will hopefully bring the NodeUI closer to natural interaction.

