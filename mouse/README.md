# mouse
Some code to read data packets from serial-interfaced mice and trackballs.

## Microsoft Mice and Trackballs ##

The Microsoft mouse protocol is 1200 baud and can support two buttons.
The mouse will identify itself if the RTS line is toggled.

I have a Microsoft Easyball, which is a large one-button trackball.
The Truedox 300 trackball has three buttons but only seems to support
a two-button message in Microsoft format.
The A4 Tech WT-7P serial trackball in "MS" 2-button mode sends
Microsoft format packets.

## Mouse Systems Mice ##

Some of the first optical mice were made by Mouse Systems, and their
PC Mouse had a serial interface.
The packet sent by the PC Mouse is five bytes, with the three bits for
the buttons sent active-low.

My Mouse Systems mouse is a model M4.
I have an A4 Tech WT-7P serial trackball that can operate in
"PC" 3-button mode, where it sends Mouse Systems five-byte packets.
My old three-button serial mouse is unbranded but has the model number
MUS2S.
It sends Mouse Systems packets, after I modified it years ago to support
the middle button (when I used it on an early Linux X-Windows system).

## Any Others? ##

I don't have a Logitech serial mouse, but I understand they have
another, different protocol.
