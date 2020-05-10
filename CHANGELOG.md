## 2.4.0
- Updated links in the About dialog
- Updated the tickers database
- Qt distribution updated to 4.8.7

## 2.3.0
- THT now has a communication channel for a remote control, see documentation [HOWTO.md](HOWTO.md)
- database updates on the site for old versions of THT have been discontinued
- Dropbox code has been synchronized with Github

## 2.2.0
- THT will update the ticker database by itself. For older versions database updates will still be available for some time, presumably a few months
- THT is running as admin now (when installing with the installer)

## 2.1.0
- added support of styles and documentation about styles in howto
- added dark style
- added Danish translation

## 2.0.2
- added Dutch, Korean and Japanese translations
- fixed loading a ticker from "Industries" window with external linking
- fixed a Dropbox connection limit

## 2.0.1
- Removed =N checkbox
- new look'n'feel for the fast ticker entry window
- for developers: new method PluginImportExport::unembed()
- added Portuguese translation

## 2.0.0
- press Space in a ticker input dialog (called by Insert, O or L) or in 'Industries' window to show the ticker information
- improved quality of the JPEG screenshots
- ability to enter numbers in a ticker field
- ability to disable global hotkeys in Options
- ability to restore link points at startup
- ability to link multiple subcontrols in the same window. This will allow to link MDI applications like RealTick (see http://www.youtube.com/watch?v=YpeLMJWSntU)
- added support of plugins (see [howto-plugins])
- added external linking (see http://www.youtube.com/watch?v=33LEP-Gnhes)
- added external linking for Excel (see http://www.youtube.com/watch?v=JH2YuYexfxE)
- added Dropbox support for the screenshots (see http://www.youtube.com/watch?v=KcHkzeHIjjw)
- added option "Show buttons" for a ticker list
- added plugin to add tickers from the Stock Splits section on Briefing
- added plugin to add tickers from Stocks In Play (http://stocksinplay.ru) (see http://www.youtube.com/watch?v=JvAScfrZFmE)
- added plugin to chat via Jabber (XMPP) (see http://www.youtube.com/watch?v=2VRvOkWX1ZI)
- hotkeys: press "Ctrl+S" in a ticker list to save tickers
- updated Howto
- added some translations from http://www.transifex.com/projects/p/traders-home-task
- added ability to set a background color for the text in the screenshot tool

## 1.5.2
- added ability to drop multiple tickers (from browser for example)
- added FOMC warning

## 1.5.1
- sorting will also sort tickers by priority

## 1.5.0
- hotkeys: press "X" to show ticker's comment
- hotkeys: press "Alt+X" to edit ticker's comment
- added "Always show ticker comments" to Options
- drag a ticker from other application and drop it onto a list to add it to this list
- drag a ticker from other application and drop it onto THT window to load it into all linked windows (see https://www.youtube.com/watch?v=5cTvKkibyWo)

## 1.4.1
- improvements to support x86_64
- hotkeys: press "F" to open the current ticker in Finviz
- hotkeys: press "D" to load SPY (instead of "F")

## 1.4.0
- improvements to support ROX
- new way to show the number of links

## 1.3.0
- added list title (visit Options to enable; press F2 to edit title)
- hotkeys: press "Space" to show ticker information in 'Industries' window
- added support of Fusion charts, DAS Trader Pro, Arche Pro, Takion, Lightspeed
- <font color=red>the linking mechanism has changed. Reconfigure your link points, also see http://www.youtube.com/playlist?list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm</font>
- hotkeys: press "G" to open the current ticker in Google Finance
- global hotkeys: press "Ctrl+Alt+R" to show the THT window (when the window is minimized or hidden in the tray)

## 1.2.0
- small fixes
- added Finviz client (see [howto])
- hotkeys: press "Z" to add tickers from Finviz
- now it is possible to load predefined tickers (like $INDU or $VIX) into all supported windows (TOS, MBT...)
- added small screenshot editor

## 1.1.1
- check for updates only when "About" dialog is opened

## 1.1.0
- ability to save & load link points (see [howto])
- fixed support of the slave monitors in a multi-monitor configuration
- hotkeys: press "Up" in the ticker mini-input to set focus to the ticker list

## 1.0.3
- save Industries position and size
- added option to restore "Industries" window at startup

## 1.0.2
- added "U.S. only" filter in Industries window

## 1.0.1
- fixed screenshot tool with multiple monitors

## 1.0.0
- hotkeys: N has been removed, use Ctrl+N instead
- hotkeys: X and H have been removed
- hotkeys: L to load a special ticker
- hotkeys: K to show or search industry & sector information
- hotkeys: Alt+N to clear the ticker lists
- hotkeys: Alt+1 to set normal priority, Alt+2 - medium, Alt+3 - high, Alt+4 - the highest
- hotkeys: Ctrl+F to search the ticker in the current list
- hotkeys: if you clear the list by mistake, you can undo with Ctrl+Z
- added a setting for duplicate tickers, visit Options
- added a new priority level
- allow to copy tickers from the first list to last and from the last to first
- fixed exporting to file
- new way to show the number of tickers
- added mini ticker entry (see [howto])
- added sector & industry information (press Space)
- added ability to select a language
- the maximum number of ticker lists is 8

## 0.8.3
- added support of MBT Desktop and MBT Desktop Pro (see [howto])

## 0.8.2
- fix for Twinkorswim@paperMoney

## 0.8.1
- hotkeys: Alt+U to reset priorities
- hotkeys: Ctrl+L to lock the links

## 0.8.0
- hotkeys: added Ctrl+Up/Down/Home/End/PageUp/PageDown to move the ticker
- added ticker priorities (see howto)
- hotkeys: U to reset ticker's priority

## 0.7.4
- Added "SPY" predefined ticker
- small fixes

## 0.7.3
- open "About" dialog to check for updates. If updates are available, you will see an exclamation icon on the right. If you are in a corporate network with a firewall, this may not work correctly (see howto)
- fixed window positioning with broken coordinates
- move the predefined tickers from the list to the global THT menu (right click)
- last used directory is saved correctly (when adding tickers from a file and saving screenshots)

## 0.7.1
- fixed small descriptor leak
- fixed setting a foreground window when running a second instance

## 0.7.0
- allow to drag-and-drop tickers from one list to another
- use INI file to save settings, it is much faster than registry
- allow to add tickers from multiple files
- switched to the dynamic Qt. This will speed up the program startup and operations with the file dialogs
- hotkey: added PageUp and PageDown
- hotkey: added Insert (same as O, add new ticker)
- hotkey: P pastes tickers from a clipboard instead of C
- hotkey: C exports tickers to a clipboard instead of D
- hotkey: Q to load $COMPQ
- hotkey: R to sort the ticker list
- added Ukrainian translation
- added "No links" warning

## 0.6.0
- hotkey: press "V" to load "$VIX"
- hotkey: press "Space" to show the company name
- hotkey: press 1,2,3,4,5 to copy the current ticker to the 1st, 2nd, 3rd, 4th or 5th list
- drag-and-drop tickers to the other window will load the ticker there (see howto)
- single click doesn't load the ticker anymore (needed for DND)
- double click to load the ticker

## 0.5.0
- First public release
