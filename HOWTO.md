Table of Contents
=================

* [Trader's Home Task (THT) How\-To Guide and Frequently Asked Questions (FAQ)](#traders-home-task-tht-how-to-guide-and-frequently-asked-questions-faq)
  * [Features overview](#features-overview)
  * [Finviz notice](#finviz-notice)
  * [Outdated features](#outdated-features)
  * [What trading platforms does THT support?](#what-trading-platforms-does-tht-support)
  * [How does THT link windows together?](#how-does-tht-link-windows-together)
  * [How do I link several windows?](#how-do-i-link-several-windows)
  * [What input language must be used?](#what-input-language-must-be-used)
  * [How do I reset links?](#how-do-i-reset-links)
  * [How do I save and add link points?](#how-do-i-save-and-add-link-points)
  * [How do I use the screenshot editor?](#how-do-i-use-the-screenshot-editor)
  * [How do I use ticker lists?](#how-do-i-use-ticker-lists)
  * [How do I export tickers?](#how-do-i-export-tickers)
  * [How do I add new tickers?](#how-do-i-add-new-tickers)
  * [How do I add tickers directly from Finviz?](#how-do-i-add-tickers-directly-from-finviz)
  * [How do I drag\-and\-drop tickers from other applications?](#how-do-i-drag-and-drop-tickers-from-other-applications)
  * [How do I use priorities?](#how-do-i-use-priorities)
  * [Does THT have a settings dialog?](#does-tht-have-a-settings-dialog)
  * [THT Options \- Explanations](#tht-options---explanations)
  * [Does THT support keyboard shortcuts?](#does-tht-support-keyboard-shortcuts)
    * [When navigating through a list:](#when-navigating-through-a-list)
    * [Window shortcuts:](#window-shortcuts)
    * [Mini ticker entry shortcuts:](#mini-ticker-entry-shortcuts)
    * [Screenshot editor shortcuts:](#screenshot-editor-shortcuts)
    * [Screenshot editor shortcuts, text entering dialog:](#screenshot-editor-shortcuts-text-entering-dialog)
    * [Global shortcuts:](#global-shortcuts)
  * [External linking](#external-linking)
  * [Remote control channel](#remote-control-channel)
  * [Tricks](#tricks)
  * [Does THT have any known conflicts with other software?](#does-tht-have-any-known-conflicts-with-other-software)
  * [What are the functional differences between installable and standalone packages?](#what-are-the-functional-differences-between-installable-and-standalone-packages)

# Trader's Home Task (THT) How-To Guide and Frequently Asked Questions (FAQ)

## Features overview

- Indirectly links Advanced Get/ESignal, Arche Pro, Graybox, Laser Trade, ROX, Sterling Trader Pro, Takion, Thinkorswim and other platforms
- External linking with certain applications including Thinkorswim and Excel
- Selects tickers from a specific sector or industry (press the **K** hotkey)
- Adds tickers directly from Finviz (Elite account is required)
- Tool to take screenshots, draw arrows and text comments on them to document trades
- System tray support
- Up to 8 ticker lists with priorities
- UI styles

## Finviz notice

Finviz requires having an Elite account in order to give access to filters (new highs and lows etc.) and tickers. You can enter your Elite account
credentials in `Top toolbars -> Add tickers -> Add from Finviz -> Customize -> Access`.

## Outdated features

THT is no longer actively developed. Pull requests to fix any issues are highly welcomed. The following features no longer work correctly:

- Dropbox support
- Briefing Stock Splits plugin
- Stocks In Play plugin

## What trading platforms does THT support?

- [Advanced Get/ESignal charts](http://www.youtube.com/watch?v=idtUAU4p75s&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=7)
- [Arche Pro](http://www.youtube.com/watch?v=uIubQGEnrTQ&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=8)
- [Aurora](http://www.youtube.com/watch?v=I-fNVQ3V4j4&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=20)
- [DAS Trader Pro (SpeedTrader, F-Trader)](http://www.youtube.com/watch?v=caCBgbfNQm8&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=9)
- [eSignal 10](http://www.youtube.com/watch?v=dm9oOKJ3khU&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=18)
- [eSignal 11](http://www.youtube.com/watch?v=OPtPOcCSJo0&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=19)
- [Fusion (charts and prints only)](http://www.youtube.com/watch?v=zYNcsQiG9dU&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=2)
- [Graybox Market Makers, Graybox Time & Sales](http://www.youtube.com/watch?v=UBgSxmDNyBk&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=1)
- [Laser Trade](http://www.youtube.com/watch?v=9Td1hhIEFvI&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=10)
- [Lightspeed Trader](http://www.youtube.com/watch?v=NWdDxkU8Sp0&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=12)
- [MBT Desktop, MBT Desktop Pro](http://www.youtube.com/watch?v=7VS-Fvl2Iac&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=4)
- [RealTick](http://www.youtube.com/watch?v=YpeLMJWSntU&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=17)
- [ROX](http://www.youtube.com/watch?v=cXZm0GT-oaY&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=13)
- [SDG Open Book](http://www.youtube.com/watch?v=UszrZHDW1ms&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=5)
- [Sterling Trader Pro](http://www.youtube.com/watch?v=Lzju1iD4qeg&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=15)
- [Takion](http://www.youtube.com/watch?v=D1KHt5amgrk&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=11)
- [Thinkorswim](http://www.youtube.com/watch?v=c6G5PbT8BzA&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=6)
- [Turbo Tick Pro (GT Trader)](http://www.youtube.com/watch?v=quHitQJaNWE&list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm&index=16)

External linking is also supported. See the corresponding chapter below.

:warning: Linking THT with Labdea does not work well. Some of the keyboard handling algorithms implemented in Labdea are non-standard,
and these algorithms make it virtually impossible to properly link Labdea with THT.

## How does THT link windows together?

THT links windows together by emulating keyboard input.

## How do I link several windows?

Please view this YouTube playlist, with multiple videos showing how to link supported platforms:
[YouTube](http://www.youtube.com/playlist?list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm)

:warning: Important notes:

- The main Advanced Get window must also be visible (i.e. not minimized) or links will be established but will not work correctly
- If you run the application you want to link to (e.g. Advanced Get or Graybox) as an administrator, you also need to run THT as an administrator.
  Otherwise linking will not work
- In order to link a Thinkorswim chart window, it must be detached (i.e. be a regular standalone window).
  Linking Thinkorswim charts embedded into a single large window might not work correctly
- Do not assign Latin letters as hotkeys in Graybox and other trading platforms you want to link to.
  If you assign a Latin letter in a platform you want to link to (e.g. "B" to open a new BUY position) and try to link the "BWA" ticker,
  Graybox (or the other platform) will open a new position (react to "B"), and will try to load the "WA" ticker, and thus will not be working
  the way it should be

## What input language must be used?

The input language must be English in all windows you want to link.

## How do I reset links?

Right-click on a THT window and select "Clear links". You can also middle-click on the black target icon.

## How do I save and add link points?

Please view this YouTube tutorial to determine how to save and add link points: [YouTube](http://www.youtube.com/watch?v=1PlpDwhgLEs).

## How do I use the screenshot editor?

Please view this YouTube tutorial to learn how to use the screenshot editor: [YouTube](http://www.youtube.com/watch?v=iE9g_5MvHi4).

## How do I use ticker lists?

Left-click on a ticker, or use the "up" and "down" arrows on your keyboard to navigate through the list.
When the ticker is selected, THT will load it to all linked windows.

## How do I export tickers?

Click the "Export tickers" button. You can then export the ticker to a file or to the system clipboard.

## How do I add new tickers?

You can add new tickers by clicking the "Add tickers" button. You can then import the ticker from a file or from the system clipboard.

For example: open Finviz, choose your filters from the screener and open "Tickers". Select all the found tickers,
copy them to the system clipboard, go back to THT and paste the tickers from the system clipboard.

## How do I add tickers directly from Finviz?

Please view this YouTube tutorial to learn how to add tickers directly from Finviz: [YouTube](http://www.youtube.com/watch?v=r1Y7iNM7_9k).

:warning: Finviz requires having an Elite account in order to give access to filters (new highs and lows etc.) and tickers. You can enter your Elite account
credentials in `Top toolbars -> Add tickers -> Add from Finviz -> Customize -> Access`.

## How do I drag-and-drop tickers from other applications?

Please view this YouTube tutorial to learn how to drag-and-drop tickers from other applications: [YouTube](http://www.youtube.com/watch?v=5cTvKkibyWo).

## How do I use priorities?

You can increase or decrease priorities for tickers by using hotkeys (see below). This will help you to remember tickers with interesting levels,
new highs or other relevant parameters.

## Does THT have a settings dialog?

Yes. You can access the settings dialog by right-clicking on a THT window and selecting "Options...".

## THT Options - Explanations

**Number of lists** - This setting allows you to specify the number of ticker lists you prefer.

**Always on top** - Selecting this option ensures the THT window will always be on top of other windows.

**Hide to tray** - This setting minimizes the THT window to the system tray when the user closes the THT window.

**Save position and size** - Selecting this option saves the current THT window position and size.

**Automatically save tickers** - Select this option so that when you add, delete, or move tickers,
THT will save the ticker lists automatically. If you do not select this option, you will need to save the lists manually.

NOTE: When a ticker list is not saved, you will see a red line on top. Click "Save tickers" to save the list and remove the red line.

**Allow duplicates in a ticker list** - If this option is checked, THT will allow you to add the same ticker multiple times to the same list.

**Fast ticker entry** - Selecting this option will prompt THT to show a small ticker entry window below the ticker list.

**Show list title** - If you select this option, each list will have a title. You can edit the title with F2.

**Always show ticker comments** - This setting prompts THT to show ticker comments in a popup window at all times.

## Does THT support keyboard shortcuts?

Yes, THT supports a rich set of keyboard shortcuts. Please continue reading for the list of shortcuts.

### When navigating through a list:

<p align="center">
  <img alt="List shortcuts" src=".github/list-shortcuts-screenshot.jpg?raw=true"/>
</p>

- **Down Arrow** - load the next ticker to all linked windows
- **Up Arrow** - load the previous ticker
- **Left Arrow** - copy the current ticker to the list leftwards
- **Right Arrow** - copy the current ticker to the list on the right
- **Ctrl+Up Arrow/Down Arrow** - move the ticker up/down
- **Ctrl+Home/End** - move the ticker to the top/bottom of the list
- **Ctrl+PageUp/PageDown** - move the ticker to the previous/next page
- **Home** - load the first ticker
- **End** - load the last ticker
- **Enter** - reload the current ticker
- **Delete** - delete the current ticker
- **Ctrl+N** - clear the list
- **Ctrl+S** - save the list
- **Ctrl+Z** - undo clearing the list
- **O** OR **Insert** - add one ticker
- **Ctrl+O** OR **A** - add tickers from a file (or from multiple files)
- **Ctrl+V** OR **P** - paste tickers from the system clipboard
- **Space** - show the company name (this shortcut also works in the "Industries" window)
- **1,2,3,4,5,6,7,8** - copy the current ticker to the 1st, 2nd, 3rd, 4th, 5th, 6th, 7th or 8th list respectively
- **Plus** OR **Equal** - increase priority
- **Minus** - decrease priority
- **Alt+1** - set normal priority
- **Alt+2** - set medium priority
- **Alt+3** - set high priority
- **Alt+4** - set the highest priority
- **F2** - update the list title
- **U** - reset priority to the default value
- **R** - sort list
- **E** - export tickers to a file
- **C** - export tickers to the system clipboard
- **K** - show or search industry & sector information
- **Alt+U** - reset all tickers' priorities
- **F** - open the current ticker in Finviz
- **G** - open the current ticker in Google Finance
- **Y** - open the current ticker in Yahoo Finance
- **X** - show ticker's comment
- **Alt+X** - edit ticker's comment
- **V** - add tickers from Stocks In Play
- **Z** - add tickers from Finviz

### Window shortcuts:

<p align="center">
  <img alt="Window shortcuts" src=".github/window-shortcuts-screenshot.jpg?raw=true"/>
</p>

- **Q** - load "$COMPQ"
- **I** - load "$INDU"
- **S** - load "$SPX"
- **T** - load "$TVOL"
- **D** - load "SPY"
- **L** - load a special ticker
- **Alt+N** - clear the ticker lists
- **Ctrl+L** - lock the links (THT won't load any ticker)
- **Ctrl+F** - search the ticker in the current list

### Mini ticker entry shortcuts:

- **Up Arrow** - set focus to the ticker list
- **Space** - add ticker to the current list
- **Enter** - load the ticker
- **Escape** - clear entry

### Screenshot editor shortcuts:

- **T** - add text
- **B** - add buy
- **S** - add sale
- **P** - add stop
- **E** - add ellipse
- **Alt+E** - change ellipse color
- **Ctrl+A** - select all items
- **Delete** - delete selected items

### Screenshot editor shortcuts, text entering dialog:

- **Ctrl+B** - change the background color
- **Ctrl+L** - align text to the left
- **Ctrl+E** - align text to the center
- **Ctrl+R** - align text to the right
- **Ctrl+Plus** - increase font size
- **Ctrl+Minus** - decrease font size
- **Ctrl+K** - show the text color dialog

### Global shortcuts:

- **Ctrl+Alt+S** - take a screenshot of your trade
- **Ctrl+Alt+R** - show the THT window (when the window is minimized or hidden in the tray)

## External linking

External linking has been available in THT since version 2.0.0. External linking takes place when you load a ticker in another application
(for example, a chart window in Thinkorswim), and it is automatically loaded into all linked windows. In case of external linking,
you can store and manage tickers in that application (called "master").

Here is a step by step example of how you might use external linking:

1. Run THT, Thinkorswim, and ROX
2. Detach a Thinkorswim chart window you want to make a master (source) window
3. Press `ALT` in the THT window and grab the red target with a mouse
4. Drop the red target on the detached Thinkorswim chart window to make it master window
5. Grab the black target in the THT window with a mouse (pressing `ALT` is not needed)
6. Drop the black target on the ROX order book (depth of market) window
7. Enter a ticker name in the detached Thinkorswim chart window
8. Result: ROX must load the entered ticker

Here are some tips that may be useful when using the external linking feature:

- Only one master window is allowed
- The THT window must be visible
- If you run the application you want to link externally (e.g. Thinkorswim) as an administrator, you also need to run THT as an administrator.
  Otherwise external linking will not work
- The window that you want to be a master should be a regular standalone (detached) window with a real window title.
  Please note that some platforms such as Lightspeed emulate window titles, and they will not work correctly for this reason
- The window needs to have a ticker name in the window title, and the ticker name must either be a first word or be inside bracket
  (if it is inside brackets it can be in any place in the window title). For example, all the following windows can be a master:
  "A,Agilent", "A|Agilent", "A Agilent", "A: Agilent", "A; Agilent", "Level2 \[A\]", "Chart - 5min (A)". The following can not: "Agilent A" or "Stock A: Agilent"
- Not all windows in trading platforms can be a master, even if they have a proper window title.
  Some platforms may change a window title in a way that THT does not recognize, and THT will not be able to use that title
- You can squeeze the THT window to free space on your desktop with by holding `Alt + Mouse Wheel` while holding your mouse pointer over the window title

Some video tutorials:

- [YouTube tutorial](http://www.youtube.com/watch?v=33LEP-Gnhes)
- [External linking Thinkorswim and ROX (in Russian)](http://www.youtube.com/watch?v=hV7C87Mj2-U)
- [External linking Thinkorswim and Aurora (in Russian)](http://www.youtube.com/watch?v=aHWdTPyF4vM)
- [External linking with eSignal 10](http://www.youtube.com/watch?v=nIF517TTD8g)
- [External linking with Excel](http://www.youtube.com/watch?v=JH2YuYexfxE)

## Remote control channel

A remote control channel has been available in THT since version 2.3.0. You can use it to control the running instance of THT
from another application or with a real remote control (without keyboard emulation).

Here is a step by step example of how you might run an emulator:

1. Run THT from the installed location, for example `"C:\Program Files\Trader's Home Task\THT.exe"`
2. Set the current list: `"C:\Program Files\Trader's Home Task\THT.exe" --ipc set-current-list 2`
3. Clear the current list: `"C:\Program Files\Trader's Home Task\THT.exe" --ipc clear`

Path to the installed THT instance can be retrieved from the system registry from the following key:
`HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{16AE5DDE-D073-4F5F-ABC3-11DD9FBF58E3}_is1\DisplayIcon`.
It contains something like that: `C:\Program Files\Trader's Home Task\THT.exe`. From the command line you can use `reg query`:

```
> reg query HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{16AE5DDE-D073-4F5F-ABC3-11DD9FBF58E3}_is1 /v DisplayIcon

HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{16AE5DDE-D073-4F5F-ABC3-11DD9FBF58E3}_is1
    DisplayIcon    REG_SZ    C:\Program Files\Trader's Home Task\THT.exe
```

Commands:

-  `clear` - clear the current list
-  `undo-clear` - undo clearing of the current list
-  `copy` - copy tickers from the current list to the system clipboard
-  `copy-left` - copy the current ticker to the list on the left
-  `copy-right` - copy the current ticker to the list on the right
-  `copy-to <list number>` - copy the current ticker to the list \[1...N\]
-  `delete` - delete the current ticker from the current list
-  `load-first` - load the first ticker in the list
-  `load-last` - load the last ticker in the list
-  `load-previous` - load the previous ticker in the list
-  `load-previous-page` - load the ticker on the previous page (like PageUp)
-  `load-next` - load the next ticker in the list
-  `load-next-page` - load the ticker on the next page (like PageDown)
-  `load <ticker>` - load the specified ticker
    * Example: `load PG`
    * Example: `load MCD`
-  `paste` - paste the tickers from the system clipboard to the current list
-  `priority-up` - increase the current ticker's priority
-  `priority-down` - decrease the current ticker's priority
-  `priority-set <priority>` - set the current ticker's priority \[1...4\] (1 - normal, 4 - red)
    * Example: `priority-set 2`
-  `reset-priorities` - reset the priorities in the current list
-  `set-current-list <list number>` - set the keyboard focus to the specified list \[1...N\]
    * Example: `set-current-list 1`
-  `sort` - sort the current list
-  `activate-window` - bring the THT window to front (if the window is minimized, it will be restored)

## Tricks

- THT logs some actions to the file %TEMP%/tht.log.
- To disable logging you need to set the environment variable `THT_NO_LOG` to 1.
- Set the environment variable `THT_CHAT_QXMPP_LOG` to 1 to force the chat plugin log its network traffic to the file %TEMP%/tht-chat-qxmpp.log.
- When you do external linking with ALT and dragging the red target, THT will check for a pressed ALT button only when you drop the target onto a window. You can start dragging the target even without ALT pressed.
- THT uses two databases with tickers - the built-in and the downloadable. Each database has a timestamp. You can see the timestamps in the "About THT..." window when you press F1 in the dialog.
- The master window is marked with the square brackets instead of round brackets in the link point manager.
- THT will show you a message on April, 1, but only once.

## Does THT have any known conflicts with other software?

The only conflict known at this time is with Punto Switcher. To avoid this conflict, add THT to the list of exceptions in the options for Punto Switcher.

## What are the functional differences between installable and standalone packages?

There are no functional differences between installable and standalone packages.
