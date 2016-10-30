# wmpsong
Provides an exported function to get the current song of the Windows Media Player.

### AHK-Example
    hSong := DllCall("wmpsong.dll\GetWMPSong", "AStr")

    if(hSong == -1 || hSong == 0)
    {
	    MsgBox, 48, Error, Dll not found!
	    ExitApp
    }

    MsgBox, 256, Current Song, %hSong%
    ExitApp
