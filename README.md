<h1 align="center">Trader's Home Task</h1>
<h3 align="center">The most popular tool to link stock trading platforms.</h3>
<p align="center">
  <a href="https://github.com/smoked-herring/tht/releases">
    <img alt="Latest release" src="https://img.shields.io/github/v/release/smoked-herring/tht?include_prereleases"/>
  </a>
</p>

Trader's Home Task (THT) helps you preparing for a working day on the U.S. stock market.
It can link Advanced Get/ESignal, Thinkorswim, MBT Desktop Pro, Arche Pro, Takion, Graybox, Laser and other trading
platforms together (see [HOWTO](HOWTO.md)), manage a list of tickers, take screenshots, hide in the system tray, show sector & industry
information, add tickers directly from Finviz, and has a wide list of hotkeys.

YouTube tutorials: [https://www.youtube.com/user/thtng](https://www.youtube.com/user/thtng).

Author: Dmitry Baryshev

https://github.com/user-attachments/assets/6453119b-af66-4efd-9282-be5347c789dd

## Features overview

- Indirectly links Advanced Get/ESignal, Arche Pro, Graybox, Laser Trade, ROX, Sterling Trader Pro, Takion, Thinkorswim and other platforms
- External linking with certain applications including Thinkorswim and Excel
- Selects tickers from a specific sector or industry
- Adds tickers directly from Finviz (Elite account is required)
- Tool to take screenshots, draw arrows and text comments on them
- System tray support
- Up to 8 ticker lists with priorities
- Styles

## Finviz notice

Finviz requires having an Elite account in order to give access to filters (new highs and lows etc.) and tickers. You can enter your Elite account
credentials in `Top toolbars -> Add tickers -> Add from Finviz -> Customize -> Access`.

## Outdated features

THT is no longer actively developed. Pull requests to fix any issues are highly welcomed. The following features no longer work correctly:

- Dropbox support
- Briefing Stock Splits plugin
- Stocks In Play plugin

## YouTube tutorials

- [How to link Advanced Get and Graybox windows](https://www.youtube.com/watch?v=UBgSxmDNyBk)
- [How to link Thinkorswim charts](https://www.youtube.com/watch?v=c6G5PbT8BzA)
- [How to use external linking with Microsoft Excel](https://www.youtube.com/watch?v=JH2YuYexfxE)
- [How to link Thinkorswim and ROX externally (in Russian)](https://www.youtube.com/watch?v=hV7C87Mj2-U)
- [How to link Thinkorswim and Aurora externally](https://www.youtube.com/watch?v=L2KVOYLglE8)
- See more [here](https://www.youtube.com/user/thtng)

## Build requirements

- Windows 7 or later
- MinGW-based Qt 4.8.7 from [here](https://download.qt.io/archive/qt/4.8/4.8.7)
- 32-bit MinGW 4.8.2 DWARF in PATH from [here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.2/threads-posix/dwarf/i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z/download)

## Licensing notes

- GNU GPLv3+ (see LICENSE.txt)
- Qxt parts are under GNU LGPL 2.1 (see LICENSE-LGPL-2.1.txt) or CPL 1.0
- THT-lib is under GNU LGPL 2.1 (see LICENSE-LGPL-2.1.txt)
- QtDropbox is under GNU LGPL v3 (see LICENSE-LGPL-3.txt)
- QXMPP is under GNU LGPL v2.1+ (see LICENSE-LGPL-2.1.txt)
- All UI files are licensed in the same manner as the project where they are included

## Other licensing notes

* "Chart" icons are taken from [here](http://www.gettyicons.com/free-icon/112/finance-icon-set/free-bar-chart-icon-png/)
  - Author: Visual Pharm
  - License: Commercial Use Allowed, Back Link Required
  - [http://www.visualpharm.com/finance_icon_set/](http://www.visualpharm.com/finance_icon_set/)

* "Locked" icon is taken from [here](http://www.iconarchive.com/show/sigma-general-icons-by-iconshock/lock-icon.html)
  - Author: Iconshock
  - License: Free for non-commercial use

* "Ok" icon is taken from [here](http://www.iconarchive.com/show/oxygen-icons-by-oxygen-icons.org/Actions-dialog-ok-apply-icon.html)
  - Author: Oxygen Team
  - License: GNU Lesser General Public License

* "Target" icon is taken from [here](http://findicons.com/icon/267728/target?id=427393)
  - Author: Gentleface.com
  - License: Creative Commons Attribution Non-commercial (by-nc)

* "Stock" icon for TickersDb is taken from [here](http://www.iconfinder.com/icondetails/17225/128/analysis_chart_graph_pie_statistics_icon)
  - Author: Visual Pharm
  - License: Commercial Use Allowed, Back Link Required

* "Wait" icon in "About" dialog is generated at http://ajaxload.info/
  - License: Free for use

* "Transparent" icon in "About" dialog is generated with QImage
  - License: Free for use

* "Finviz" icon is created with GIMP
  - Author: Dmitry Baryshev
  - License: Free for use

* "Briefing" icon is created with GIMP
  - Author: Dmitry Baryshev
  - License: Free for use

* "Buy" and "Sale" icons are based on the icons from Splashy theme
  - [http://findicons.com/pack/1689/splashy](http://findicons.com/pack/1689/splashy)
  - Author: Dat Nguyen
  - License: Free for use

* "Ellipse" icon is taken from GNOME icon theme
  - [http://www.iconfinder.com/icondetails/21556/16/](http://www.iconfinder.com/icondetails/21556/16/)
  - Author: GNOME Project
  - License: GNU GPL

* "Chat" icon is taken from [here](http://findicons.com/icon/73394/chat_bubble?id=332798)
  - Author: Burak OZTURK
  - License: Free

* "Status" icons are taken from Sunken Gems theme
  - [http://www.adiumxtras.com/index.php?a=xtras&xtra_id=3632](http://www.adiumxtras.com/index.php?a=xtras&xtra_id=3632)
  - Author: Dan

* Interface layout is based on the original idea by Krupachev  Konstantin and his project "Clicker"

* "StockInPlay" icon is taken from Silk theme
  - [http://findicons.com/icon/159904/money_dollar?id=372987](http://findicons.com/icon/159904/money_dollar?id=372987)
  - Author: Mark James
  - License: Creative Commons Attribution (by)

* Country flags are taken from
  - http://icondrawer.com/free.php
  - License: Free

* Other icons are taken from Fugue Icons theme
  - [http://p.yusukekamiyamane.com/](http://p.yusukekamiyamane.com/)
  - Author: Yusuke Kamiyamane
  - License: Creative Commons Attribution 3.0 License

* GDI drawing functions are based on Spy
  - [http://www.codeproject.com/Articles/1698/MS-Spy-style-Window-Finder](http://www.codeproject.com/Articles/1698/MS-Spy-style-Window-Finder)
  - Author: Lim Bio Liong
  - License: CPOL

* FindBestChildProc, FindBestChild, RealWindowFromPoint
  - [http://www.codeforge.cn/read/167975/WindowFromPointEx.c__html](http://www.codeforge.cn/read/167975/WindowFromPointEx.c__html)
  - Author: J Brown
  - License: Freeware
