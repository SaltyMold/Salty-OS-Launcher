<img src="github_assets/banner.png" alt="Salty-OS">

<p align="center">
  <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="CC BY-NC-SA 4.0" src="https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
  <a href="https://github.com/SaltyMold/Salty-OS/releases"><img alt="Latest Release" src="https://img.shields.io/github/v/release/SaltyMold/Salty-OS?label=Release&labelColor=292929&color=green&logo=github&style=for-the-badge" /></a>
  <br/>
  <a href="https://discord.gg/hpeW5Jud6p"><img alt="Membres Discord" src="https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fdiscord.com%2Fapi%2Finvites%2FhpeW5Jud6p%3Fwith_counts%3Dtrue&query=%24.approximate_member_count&logo=discord&logoColor=white&color=blue&labelColor=292929&label=Membres&style=for-the-badge" /></a>
  <a href="https://github.com/SaltyMold/Salty-OS-Launcher"><img alt="GitHub Stars" src="https://img.shields.io/github/stars/SaltyMold/Salty-OS-Launcher?label=Stars&labelColor=292929&color=yellow&logo=github&style=for-the-badge" /></a>
</p>

## About

It's an app that easily installs and starts [Salty-OS](https://github.com/SaltyMold/Salty-OS) on your numworks calculator.

## Installation

To install this app, you'll need to:
1. Download the **`Salty-OS-Launcher-Salty-OS-{version}-{model}.nwa.nwa` file**, which you can download from the [Releases](https://github.com/SaltyMold/Salty-OS-Launcher/releases/).
2. Connect your **NumWorks calculator** to your computer using a USB cable.
3. Head to **[my.numworks.com/apps](https://my.numworks.com/apps)** to send the **`nwa` file** on your calculator along the **`mjpeg` video**.

## How to use

| Key   | Action        |
|-------|---------------|
| OK  | Continue      |
| Back  | Quit      |

## Documentation

The app writes on the flash using SVC calls. See https://nwagyu.org/reference/apps/syscalls.html.

The app follows these steps:
1. It checks if the userland in the app's data is there.
2. It checks if the calculator model is the same as the userland model in the app's data.
3. It checks if the calculator is in slot A.
4. It erases the slot B flash.
5. It writes the userland in the slot B flash.
6. It erases the userland in the app's data.
7. It writes a sample of themes in the theme area of Salty-OS.
8. It jumps to the userland in the slot B flash.

See more at: https://nwagyu.org/reference/firmware/slots.html

## Build the app

I made tutorials here :
- [C-App-Guide-for-Numworks](https://github.com/SaltyMold/C-App-Guide-for-Numworks)