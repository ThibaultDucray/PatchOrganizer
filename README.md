# PatchOrganizer
Patch organizer helper utility for Hotone Ampero(c) multieffect presets files.

This is PatchOrganizer, a small tool that will help you organize your Ampero(c) presets files on your Mac computer.

# What is it designed for?

The 99 user patches of your Hotone Ampero(c) can be saved as a preset file (.PRST) to your computer and manipulated with the Ampero Editor software.
But there is one main feature missing in this software: moving and inserting patches.

That's where PatchOrganizer starts: move patches inside a 99 patches long preset file.

# How does it work?

You use it at your own risk.
What risks? corrupt your PRST files, or maybe corrupt your Ampero.
It didn't happen to me, but that doesn't mean anything.

# Why is it risky?

There is no specification of the Ampero PRST file format, so I did it with reverse engineering on a few files. Nothing really sure.
But today, it works.

# The file format

The file format was partially decoded, see below. Offsets and value may differ, read description carefully to understand how to manipulate the PRST files.

<pre>
<b>Offset	Content 	Size	Description</b>
<b>0	See below 	  32	Header</b>
0	TSRP              4	PRST : "Preset"
4	05 BE 00 00*      4	48645 : Content size from offset 8 to 3 bytes before end of file (see Tail section). Value may differ but should be (filesize - 8 - 3)
8	d-PM0 	          5	? Maybe 0MP-d, version of Ampero ?
13	01	          1	? Separator ?
14	63 00 	          2	99 : number of patches in file
16	00...00	          16	Filler ?
32	38 03 00 00 	  4	824 : offset of the 1st patch description
36	D3 01 00 00*      4	467 : length of the 1st patch description. Value may differ : 467 for builtin IR, other for User IR.
40	0B 05 00 00*      4	1291 : offset of the 2nd patch description (value may differ)
44	FD 01 00 00*      4	509 : length of the 2nd patch description (value may differ)
48	... 	...	        Rest of the patch offsets and lengths
<b>824	See below         47829	Patch descriptions. Value may differ but should be (filesize - 824 - 3)</b>
824	MRAP  	          4	PARM : "Parameters"
828	CB 01 00 00 	  4	459 : Size of patch description, from next offset to next patch description or to user IR if any.
832	01  	          1	? Separator ?
833	00 00 	          2	Patch position from 00 00 (=0 = position 1-1) to 63 00 (=99 = 33-3). Changing this value affects the ordering of the patch in Ampero.
835	4B* 	          1	? Value may differ.
836	Patch name*  	  32?	Name of patch. Seems to be a null terminated string (to be confirmed). Value may differ.
868	undef 	          423*	Patch parameters. Length may differ but should be (sizeOfPatchDescription - 1 - 2 - 1 - 32)
1291	Next patch	  Repeat description from offset 824
... ...
<b>1291	See below	  42*	Optional. In case of user IR</b>
1291	RISU	          4	USIR : "User IR"
1295	22 00 00 00*      4	34 : length of User IR reference description. Value may differ.
1299	undef             34*	User IR reference description. Length may differ
1333	Next patch	...   Repeat description from offset 824
... ...
<b>48653	See below	  3	Tail. Offset may differ but should be (filesize-3)</b>
48653	01	          1	? Separator ?
48653	06 0A*	          2	? Value may differ.
</pre>

# Is it free?

Yes, under the GPL - GNU GENERAL PUBLIC LICENSE.
See https://www.gnu.org/licenses/gpl.txt
