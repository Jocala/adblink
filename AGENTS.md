# adblink — agent guidance

## Qt paths (CMAKE_PREFIX_PATH)
- **Windows**: `C:\Qt\Qt.6.11.1-static`
- **macOS**: `/Users/jeff/Qt/Qt.6.11.1-static`
- **Linux**: `/home/jeff/Qt/Qt.6.11.1-static`

## Build & test

### macOS
```sh
cmake -S /Users/jeff/source/adblink -B /Users/jeff/source/builds/adblink
cmake --build /Users/jeff/source/builds/adblink
ctest --test-dir /Users/jeff/source/builds/adblink --output-on-failure
cpack --config /Users/jeff/source/builds/adblink/CPackConfig.cmake
```
Or use: `/Users/jeff/source/adblink/build-adblink-macos.sh` (build + test)
Package only (after build): `/Users/jeff/source/adblink/package-adblink-macos.sh`

### Linux
```sh
cmake -S /zstore/source/adblink -B /home/jeff/build-adblink
cmake --build /home/jeff/build-adblink
ctest --test-dir /home/jeff/build-adblink --output-on-failure
cpack --config /home/jeff/build-adblink/CPackConfig.cmake
```
Or use: `/zstore/source/adblink/build-adblink-linux.sh` (build + test)
Package only (after build): `/zstore/source/adblink/package-adblink-linux.sh`

### Windows
```sh
cmake -S C:\source\adblink -B "%USERPROFILE%\build-adblink" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\Qt.6.11.1-static"
cmake --build "%USERPROFILE%\build-adblink"
ctest --test-dir "%USERPROFILE%\build-adblink" --output-on-failure
cmake --build "%USERPROFILE%\build-adblink" --target package-win
```
Qt Creator uses the same build dir with Ninja — run vcvars64.bat first or let Qt Creator handle it.
Build + test: `C:\source\adblink\build-adblink-windows.ps1`
Package only (after build): `C:\source\adblink\package-adblink-windows.ps1`
The `package-win` target wraps `iscc` and produces `packages/adblink-8.3-win64.exe`.

Single test (any platform):
```sh
cmake --build <build_dir> --target <test_name>
```

12 test executables: `adbstring`, `database`, `jsonconfig`, `logfile`, `getadbpath`, `devicerecord`, `iptablesort`, `stringutils`, `getreturncode`, `devicerecorddialog`, `logviewerdialog`, `refreshadb`.

Tests use Qt Test (`Qt6::Test`). Each test file ends with `#include "tst_xxx.moc"` and uses `QTEST_MAIN` or `QTEST_APPLESS_MAIN`. A static `testsupport` lib supplies shared test deps.

## Architecture

### Entry & UI stack
- `main.cpp` sets Fusion style, loads `:/style.qss`, sets DejaVuSans font (12px).
- **MainWindow** owns ~30 manager objects (one per feature), each in its own `.cpp`/`.h`, allocated in the constructor member initializer list.
- Constructor sequence: OS detection → path derivation → keyboard shortcuts → status bar → log rotation → DB init → table creation → JSON config → menu setup → window sizing → track-devices start → version check.
- Layout: `mainLayout` (QVBoxLayout, margins 28, spacing 18) → `upperLayout` (HBox: device table 55-63% + right column with 3×2 button grid + Ad Hoc IP field) → `stackedWidget` (QStackedWidget: Kodi grid at index 0, Android grid at index 1).
- 3-level window sizing (small/medium/large) read from JSON `"defaultwindow"` key, applied via `setWindowSize()` which calls `setupUI()`.

### Views
- **Kodi view** (index 0): 4×4 button grid (16 buttons). Kodi menu visible.
- **Android view** (index 1): 4×3 button grid (12 buttons). Kodi menu hidden via `menuKodi->menuAction()->setVisible(false)`.
- Switched via Utility → Switch View. Persisted to JSON `"startview"` key.

### Device storage
- **SQLite** at `~/.jocala/adblink.db` (macOS/Linux) or `%APPDATA%/.jocala/adblink.db` (Windows).
- **JSON config** at `~/.jocala/adblink.json` — flat object with ~16 keys (checkversion, scrcpy, startview, defaultwindow, font indices, terminal pref, paths, localadb, startapp, stopapp).
- **Logs** at `~/.jocala/adblink.log`, rotated to `.old.log` at startup (only one backup kept).

### Platform detection
- `m_os` integer: 0 = Linux, 1 = Windows, 2 = macOS.

## ADB communication layer

Four execution patterns, use the right one:

| Pattern | When | File | Key detail |
|---|---|---|---|
| `getadbOutput(cstring)` | One-shot adb command, ignore exit code | `getadbdata.cpp` | Uses `startCommand()` (shell splitting), `syncWaitForProcess` |
| `getreturncode(cstring)` | Need exit code only | `getreturncode.cpp` | Uses `QProcess::splitCommand()` + `start(program, args)` — no shell. Empty-arg guard returns false. |
| `connectadb(adbPath, args)` | Network connect with 5s timeout | `connectadb.cpp` | QEventLoop + single-shot QTimer(5000). Kills process on timeout (SIGKILL/TerminateProcess). |
| `AdbConnection::runCommand(args)` / `AdbDevice::runCommand(binary, args)` | OOP wrapper via a manager | `adbconnection.cpp` / `adbdevice.cpp` | Splits manually with `QProcess::splitCommand` |

### `syncWaitForProcess()` (adbutils.cpp)
Core blocking wait: polls `waitForFinished(100)` in a loop, calls `QCoreApplication::processEvents()` each iteration to keep UI alive. Optional timeout kills the process. Default timeout = -1 (no timeout). Always uses `MergedChannels` (stdout+stderr combined).

### ADB path construction
`MainWindow::getadb()` builds the adb prefix string: `"\"<adb_path>\" -s <serial_or_ip:port>"`. USB and TCP devices are distinguished by the `isusb` field.

### `splitCommand()` vs `startCommand()`
- `startCommand()` delegates to `/bin/sh -c`/`cmd.exe /c` — shell interprets quotes and pipes.
- `splitCommand()` + `start(program, args)` — no shell, arguments passed literally. Preferred for safety.

### ADB binary discovery priority
1. User-specified path from `adblink.json` key `"localadb"` + `/adb` (or `adb.exe` on Windows)
2. Bundled `<appDir>/adbfiles/adb` (or `adbfiles/adb.exe` on Windows)

## USB auto-discovery

### Architecture
Persistent `QProcess` running `adb track-devices`, not polling. Streams device state changes continuously.

- `startTrackDevices()`: creates QProcess, connects `readyReadStandardOutput` → `onTrackOutput`, connects `finished` → `onTrackFinished` (auto-restart after 1s).
- `onTrackOutput()`: appends `readAllStandardOutput()` to `m_trackBuffer` and passes it to `parseTrackDevices()` (`stringutils.cpp`), which consumes adb's 4-hex-digit length-prefixed chunks. One chunk carries the **whole** device list, so the prefix appears once per chunk — never per line. Each payload line is `SERIAL\tSTATUS`. Updates `m_usbStatusCache` (QHash<QString,QString>), starts 150ms debounce timer.
- **Windows frames are CRLF.** adb writes this stream with `fwrite(..., stdout)` (`DefaultStandardStreamsCallback::OnStream`); only the shell/`exec-out` paths call `_setmode(STDOUT_FILENO, _O_BINARY)`, so on Windows every `\n` is relayed as `\r\n` while the header counts the `\n` only. `parseTrackDevices()` therefore strips `\r` before framing and rejects a header that does not frame a device list — the byte-exact parser drifted a byte per frame without that, slicing the header into the serial and auto-adding duplicate records named `0018G074TS0361670XW6`. Do not "simplify" either step away; the LF-only unit tests cannot catch it. Measured on win10: `adb devices` stdout ends `...100,13,10,13,10`.
- `applyDebouncedRefresh()` (debounce timeout): iterates cache, skips network devices (contains `:`), skips devices whose status is not connected/recovery/rescue (`isConnectedStatus()`), skips existing DB records, then INSERTs a new device record with default Kodi values. Duplicate descriptions get `-2`, `-3`, etc. suffix.
- `m_quitting` guard in `onTrackFinished` prevents restart during shutdown.

### Status display
`usbStatus()` in `stringutils.cpp`: maps cache values to display text — `"device"` → `"Connected"`, missing → `"Disconnected"`, otherwise capitalize first letter (`"unauthorized"` → `"Unauthorized"`, `"recovery"` → `"Recovery"`).

`isConnectedStatus()` (same file): case-insensitive predicate, true for `Connected`/`Recovery`/`Rescue`. This is the gate for every "is the selected device usable" check — `validateDeviceSelection()`, double-click, Disconnect, and the Delete-record block. Do not compare a status cell against the literal `"Connected"`.

## Device table

`NoHScrollTableWidget` (class in `mainwindow.h`): 3 columns (Device | IP | Status), single-selection, row-select, no horizontal scrollbar. Column widths: `tableWidth / 3` (tableWidth = 55-63% of window). Min section 150px, max 250px.

`DeviceTableLoader::loadTable()`: preserves selection and scroll state across reloads. Queries all devices from SQLite, populates rows, applies IP-aware sorting via `IpTableWidgetItem`. USB device IP column shows `"USB"`, network devices show IP or `"N/A"`.

Double-click on connected row → disconnect; on disconnected row → connect.

## Kodi data path resolution

Three paths, checked in order:

1. **Custom env file**: If device has `/sdcard/xbmc_env.properties`, parse `xbmc.data=` line + `/.kodi`
2. **Scoped storage** (Android 11+): `<data_root>/kodi_data/<package>/files/.kodi`
3. **Legacy**: `<data_root>/Android/data/<package>/files/.kodi`

`isScopedStorage()` (adbutils.cpp): checks `ro.build.version.sdk` (API 30+ → scoped; API 29 → tests write permission to `/sdcard/Android/data/`). Also checks if `ls -ld /sdcard/` shows world-writable permissions (vendor bypass).

`BackupManager::kodiDataRoot()` and `kodiBaseRoot()` use same scoped vs. unscoped logic.

## Metadata & backup file cleanup

`removeMetadataFiles()` in `adbutils.cpp` is a shared utility that removes editor backup/swap files and Apple metadata files from a directory tree. Called automatically:

- **After backup pull** (`backupmanager.cpp`): cleans the local backup directory so junk files never persist
- **Before restore push** (`backupmanager.cpp`): cleans the selected backup directory before sending to device, handling existing dirty backups

Files removed:
- Editor: `*~`, `*.swp`, `*.swo`, `#*#`
- Apple: `.DS_Store`, `._*` (AppleDouble resource forks), `__MACOSX/`, `.AppleDouble/`, `.localized`

The function uses `QDirIterator::Subdirectories` with `prepend` (reverse order) to process children before parents. Directories are removed via `QDir::removeRecursively()`, files via `QFile::remove()`. It logs each removed entry.

### Restore file protection
`restoreDevice()` (backupmanager.cpp) shows `RestoreOptionsDialog` (restoreoptionsdialog.cpp) before wiping the target — 9 checkboxes (guisettings.xml, advancedsettings.xml, sources.xml, favourites.xml, profiles.xml, RssFeeds.xml, mediasources.xml, passwords.xml, Lircmap.xml) for files to keep from the device during restore. Selection persisted in `adblink.json` key `protectfiles` (comma-separated; `KodiDataManager::readProtectFiles()/writeProtectFiles()`). Checked files are pulled to `QDir::tempPath()/adblink-protect-<daddr>` before `rm -r` and pushed back after the restore push; missing files skipped, failures logged but never abort a successful restore.

## Database schema & patterns

### `device` table (20 columns)
Columns: `Id INTEGER PRIMARY KEY AUTOINCREMENT`, `daddr TEXT`, `description TEXT NOT NULL UNIQUE`, `pulldir`, `xbmcpackage`, `data_root`, `buffermode INTEGER`, `buffersize`, `bufferfactor`, `filepath`, `port`, `isusb INTEGER`, `ostype TEXT`, `logfilename TEXT`, `disableroot INTEGER`, `flag1` (scoped), `flag2` (comments, was wsa), `flag3`, `flag4`, `flag5` (scrcpyarg).

### DB access patterns
- **Device selection → query**: `MainWindow::queryDeviceRecord(selectedDescription)` from device table column 0 text.
- **New device**: `INSERT INTO device (11 columns) VALUES (...)`. Duplicate description check first.
- **Edit device**: `UPDATE device SET (11 columns) WHERE description = ?`. No duplicate check.
- **Ad hoc**: `INSERT OR REPLACE INTO device (7 columns) VALUES (...)`. Description hardcoded `"Ad hoc"`.
- **Delete**: `DELETE FROM device WHERE description = ?`. Blocked if device status is `"Connected"`.
- **Lookup by serial**: `queryDeviceByDaddr(serial)` used by USB auto-add.

### KodiDataManager methods
`queryDeviceRecord()`, `queryDeviceByDaddr()`, `descriptionExists()`, `deleteRecord()`, `dataBaseDir()`.

## Logfile utility (`logfile.cpp`)

Thread-safe via `static QMutex` + `QMutexLocker`. Appends to `<logdir>/adblink.log` with `QFile::WriteOnly | QFile::Text | QFile::Append`. `rotateLogFile()`: delete existing `.old.log`, rename `.log` → `.old.log`. Called once at startup.

## String utilities (`stringutils.cpp`)

- `stripString(str)`: `simplified()` then remove all spaces.
- `checkslash(path)`: ensure leading and trailing `/`.
- `validateIPAddress(ip)`: regex matching IPv4 dotted-decimal or DNS hostname.
- `usbStatus(cache, daddr)`: map track-devices state to display text.
- `isConnectedStatus(status)`: true for `Connected`/`Recovery`/`Rescue` (a recovery-mode device runs adbd, so shell works).
- `adbString(input)`: escape single quotes, wrap in double quotes.

## Preferences (`adblink.json`)

Managed by `PreferencesManager`. Dialog (`adbprefDialog`, fixed 300x500) has:
- 3 checkboxes (version check, scrcpy args, Kodi startup view)
- 5 combos (default window size, 4 font sizes)
- Terminal type (macOS: Terminal/iTerm2/Ghostty Terminal; Linux: Gnome/XFCE4/Konsole/Ghostty Terminal) — with guardrails: `ConsoleManager::isTerminalAvailable()` checks bundle (`/Applications/...`) + `QStandardPaths::findExecutable`; missing selection logs and falls back to Terminal/gnome-terminal with a `QMessageBox::Warning` (“%1 not found…”) and `adblink.log` entry; `terminalCommand/launchTerminal/openConsole/openAdbShell/openScrcpy` all handle fallback + `startDetached` failure; `adbprefDialog` marks missing entries “(not installed)” and warns on accept
- 4 directory pickers (pull, APK, backup, external ADB)
- 3 buttons (Cancel, Updates check, OK)

Defaults set in `KodiDataManager::createJsonConfig()`. Font index-to-pixel mapping: `{0:10/12/14/12, 1:12/14/16/16, 2:14/16/18/18, 3:16/18/20/22}` for (sm/md/lg/fm).

## DeviceRecordDialog

Visible fields: Description, IP Address, Port, USB checkbox, Package Name, Data Root, File Path, Pull Directory, Scrcpy Args.
Hardcoded (widgets removed, setters are no-ops): `ostype()` = `"0"`, `scoped()` = `false`, `wsa()` = `false`, `disableroot()` = `false`.

## Qt/CMake quirks

- `CMAKE_AUTOMOC ON` + `CMAKE_AUTORCC ON`. No manual moc/rcc.
- CMake 3.16+, C++17, Qt 6.
- **No generator expressions in install targets** — Ninja on Windows has quoting issues. Use `CMAKE_CURRENT_BINARY_DIR` instead.
- Packaging: Inno Setup (Win), DragNDrop (macOS), TGZ (Linux). CMake custom target `package-win` on Windows; CPack `package` target on macOS/Linux.

## Code conventions

- `QMessageBox` must use explicit `setWindowModality(Qt::WindowModal)` — never static `QMessageBox::*` helpers.
- `adb` references are lowercase (`adblink`, not `adbLink`). The `.desktop` file at `adbfiles/adbfiles.linux/icons/adblink.desktop` must also use lowercase `adblink` for `Name`, `GenericName`, and `Comment` fields.
- `NoHScrollTableWidget` disables horizontal scrollbar via `ScrollBarAlwaysOff`.
- Oculus VR slot (`on_actionOculus_VR_triggered`) compilable but not connected to any menu action — dead code.
- **No comments** in new code unless clarifying non-obvious intent.

## Help system

- HTML help files in `assets/`, loaded via `qrc:/assets/<file>.html`.
- Shared stylesheet: `assets/style.css`. Help dialog uses `QTextBrowser` (not QTextEdit) with `setSource(QUrl("qrc:/assets/menu.html"))` for hyperlink navigation.
- `buttons.html` is the primary index for action-linked pages.
- `menu.html` is the top-level topic index.

## Key paths

- `version.h` → `const QString version = "8.3"`
- `program.h` → `PROGRAM "adblink"`
- `buildinfo.h.in` → configured by CMake to `<builddir>/buildinfo.h`, a compile-time `buildStamp` (`built <UTC> , git <sha>`); `mainwindow.cpp` logs it directly under the version line, so a silent rebuild is identifiable
- `changelog.txt` — full history
- `adbfiles/<platform>/` — bundled ADB and tool binaries per OS (8.3 payload: adb 37.0.1-15733141 all platforms; Windows scrcpy 5.0.1)
- `logfile.cpp` — thread-safe logging singleton
- `adbutils.cpp` — `syncWaitForProcess()`, `isScopedStorage()`, `resolveKodiPath()`
- `kodidatamanager.cpp` — CRUD for device records, schema creation, JSON config defaults
- `mainwindow.cpp` (2815 lines) — all button slots, track-devices, view switching, layout
- `deploy.sh` — jocala.com website deployment script (runs from Debian only)
- `gh-deploy.sh` — GitHub release script (runs from macOS, creates release with 3 installer assets)

## jocala.com website

### Git repo
- **Bare repo (server)**: `jeff@192.168.1.39:/zstore/source/git/jocala.com.git`
- **Debian working tree**: `/zstore/source/www/jocala.com/` (auto-updated via post-receive hook)
- Latest commit: `68099dd` move deploy script to adblink repo (Jun 2026)

### Local mirror
- **URL**: `http://debian.local/www/jocala.com/` — local mirror of the production site
- Served from the Debian working tree (`/zstore/source/www/jocala.com/`) via Apache
- Source of truth for `deploy.sh` — changes staged here are pushed to production with `deploy.sh all|html|builds`

### Production server
- **Host**: `jeff@jocala.com` (68.67.75.218)
- **Docroot**: `/var/www/jocala.com/public_html/`
- **Web server**: Apache with SSL (Let's Encrypt)
- **SSH**: From Debian → jocala.com works passwordlessly

### Deployment, staging & release scripts
- **`deploy.sh`** (Debian only): pushes builds + HTML to production jocala.com
- **`stage.sh`** (run from macOS): builds all platforms, stages packages + website edits locally on Debian
- **`gh-deploy.sh`** (run from macOS, after staging): pushes source to GitHub + creates GitHub release with all 3 installer assets
- **Usage** (run from Debian):
  ```sh
  cd /zstore/source/adblink
  ./deploy.sh html                    # deploy HTML + version.txt
  ./deploy.sh builds <files>          # deploy packages
  ./deploy.sh all <files>             # deploy both
  ```
- **No post-receive auto-deploy** — user deploys manually on purpose

### Site structure
- Static HTML site (no PHP/CMS)
- 21 root files (20 `.html` + `version.txt`), 39 help pages, 31 referenced images
- No video files, no orphaned pages, no unused images (all cleaned Jun 2026)
- GA4 ID: `G-B67VTTJQ1L`
- No `.htaccess` on production (user opted out)

### Release workflow
1. Run `./stage.sh <version>` from macOS — builds all platforms, copies packages to Debian downloads dir
2. On Debian: edit `index.html`, `changelog.txt`, `version.txt` in the website working tree
3. On Debian: `cd /zstore/source/www/jocala.com && git add -A && git commit -m "v<ver> release"`
4. On Debian: `cd /zstore/source/adblink && ./deploy.sh all <packages>` — deploys builds + HTML to production
5. On Debian: `cd /zstore/source/www/jocala.com && git push`
6. On macOS: `./gh-deploy.sh` — creates GitHub release with all 3 installer assets

## Current status (Oct 2026)

### Source locations
| Platform | Source | Build |
|---|---|---|
| macOS | `/Users/jeff/source/adblink` | `/Users/jeff/source/builds/adblink` |
| Linux | `/zstore/source/adblink` | `/home/jeff/build-adblink` |
| Windows | `C:\source\adblink` | `%USERPROFILE%\build-adblink` |

Builds are out-of-source (Samba share → local disk). Each platform has a build script in the source root. **Always use the platform build script** for builds, tests, and packaging — they handle environment setup (vcvars64.bat on Windows, etc.) and ensure consistent results. On Windows, packaging is split into a separate `package-adblink-windows.ps1` script; run it after a successful `build-adblink-windows.ps1` to produce the installer.

### Remote build hosts
| Platform | Host | User | SSH key |
|---|---|---|---|
| Debian | `192.168.1.39` | `jeff` | `/Users/jeff/.ssh/id_ed25519` |
| Windows (primary) | `192.168.1.170` (win10 VM) | `jeff` | `/Users/jeff/.ssh/id_ed25519` |
| Windows (backup) | `192.168.1.137` (win11) | `jeff` | `/Users/jeff/.ssh/id_ed25519` |

Easy switch: `WINDOWS_HOST=backup ./stage.sh` or `WINDOWS_HOST=backup ./syncdb.sh` uses backup (137); default is primary (170). `WINDOWS_HOST=jeff@192.168.1.170` (or any `user@host`) overrides with any host.

### Distributing source to the build hosts

macOS is the authoritative source. Linux and Windows compile their **own** trees, so
every all-platform build requires the macOS commits to reach both hosts before they
are built:

```sh
git push origin main                  # Debian bare repo; no post-receive hook
ssh jeff@192.168.1.39 "cd /zstore/source/adblink && git stash && git pull"
ssh jeff@192.168.1.170 "cd C:\\source\\adblink && git stash && git pull"
```

`stage.sh` performs both pulls itself (`stage.sh:44,48-50`), so running it needs no
manual step. The Windows path needs doubled backslashes over ssh, and the pulls are
separate ssh calls rather than one `;`-chained command (`d2bd0b9`) — cmd mangles them.

This is a required part of every all-platform build, not a decision point, and skipping
it **fails silently**: the host compiles the previous commit and the installers look
perfectly normal. Confirm each host with `git log --oneline -1` after the pull.

### Packaging
| Platform | Generator | Output |
|---|---|---|
| macOS | DragNDrop | `.dmg` with Applications symlink |
| Linux | TGZ | `.tar.gz` |
| Windows | Inno Setup | `.exe` installer, no license page |
| GitHub | `gh-deploy.sh` | Release with all 3 installer assets |

### CMakeLists.txt key details
- `CPACK_BINARY_TZ OFF` — prevents `.tar.Z` on all platforms
- `CPACK_BINARY_STGZ OFF` on all platforms
- macOS: `install(PROGRAMS ...)` for adbfiles (enforces +x), POST_BUILD copies adbfiles + icns into bundle
- Linux: `install(PROGRAMS ...)` for adbfiles, `install(DIRECTORY ...)` for icons
- macOS icon copied via POST_BUILD to `Contents/Resources/adblink.icns`
- `install(PROGRAMS ...)` for macOS/Linux adbfiles enforces +x, so no
  permission-fix step is needed before packaging
- `packaging/sign-after-install.cmake` — CPack pre-build script
  (`CPACK_PRE_BUILD_SCRIPTS`, macOS only), signs the staged bundle with
  Developer ID + runtime + entitlements before DMG creation
- `packaging/adblink.entitlements` is shared by the dev bundle
  (`build-adblink-macos.sh`) and packaging (`sign-after-install.cmake`), so dev
  and release cannot drift. `com.apple.security.automation.apple-events` is
  **required**: a hardened-runtime app that sends Apple Events without it is
  re-prompted for automation on every use (tccd logs "requires entitlement
  com.apple.security.automation.apple-events but it is missing"). adblink sends
  Apple Events for the terminal launch (`open -a <terminal>`) and for every
  `QDesktopServices::openUrl` link (help, changelog, donate, Kodi download).
  Dropping the key silently reintroduces those prompts. It is a
  hardened-runtime resource-access entitlement, not a sandbox one, and is
  unrestricted for Developer ID (no provisioning profile, no notarization
  change).
- `packaging/Info.plist.in` — keep `NSAppleEventsUsageDescription` in step with
  that entitlement; without it the automation request has no user-facing reason.
- No `CPACK_RESOURCE_FILE_LICENSE` set (no license page on any platform)

### macOS bundle verification
Built app is at `/Users/jeff/source/builds/adblink/adblink.app/`. Contents:
- `Contents/MacOS/adblink` — arm64 binary
- `Contents/MacOS/adbfiles/` — aapt, adb, busybox, fastboot, sqlite3 (all +x)
- `Contents/Resources/adblink.icns` — application icon
- `Contents/Info.plist` — references `adblink.icns`, bundle ID `jocala.com.adblink`

DMG at `/Users/jeff/source/builds/adblink/packages/adblink-8.3-Darwin.dmg` contains `adblink.app` + `Applications -> /Applications`.

## Known issues / held-out fixes

### Uninstall dialog empty (Sep 2026) — fixed in 8.1 silent rebuild 2026-09-09, all platforms
- Symptom: uninstall dialog lists no program ids (user report: FireTv4K `192.168.200.240:5555`, mantis, API 25, screenshot empty, `adblink.log` shows connect ok then `open uninstall dialog` with empty list).
- Root cause (code inspection, no local repro): `UninstallManager::uninstallPackage` passed `""` for port (`uninstallmanager.cpp:22`), so `uninstalldialog.cpp:113` built `adb -s <bare-IP>` without `:port` for TCP devices. `MainWindow::getadb()` appends `:port` (default 5555); dialog did not. Also: `loadList`/`loadBox` never surfaced adb errors (offline/unauthorized/not-found all rendered as empty); `remove(0,8)` assumed `package:` prefix; adb path unquoted vs quoted in `getadb()` (fails under `C:\Program Files\`); filter used `| grep` (missing on stock Windows); Clear did not reload.
- Fix (all platforms, 8.1): `uninstallmanager.cpp` now passes `adbPrefix` (quoted `"adb" -s ip:port`) and logs `open uninstall dialog: daddr adbPrefix=`; `uninstalldialog.h` adds `m_adbPrefix` and ctor `uninstallDialog(adbPrefix)` (legacy `daddr/port` overload kept); `uninstalldialog.cpp` uses `m_adbPrefix + " shell pm list packages"` (quoted), logs cmd/bytes/head, guards `startsWith("package:")`, skips error lines, filters in-process (CaseInsensitive `contains`), Clear reloads full list, logs `filter matched` and `loadBox added`. Applies to Linux/macOS equally (port bug and error handling identical; quoting/filter more critical on Windows but correct everywhere).
- Rebuild 2026-09-09 (silent 8.1, no website version bump): rebuilt DMG/TGZ/EXE with same filenames, updated `changelog.txt` (09/09/26 entry) on test+prod, redeployed builds to `jocala.com/downloads/` and GitHub `v8.1` assets. Web `adblink.html` unchanged. Initial Windows test package had same fix (21,392,636 bytes SHA256 `5A1081C...` on win10).

### Per-device scrcpy args ignored unless the "Ask for Scrcpy arguments" prompt is on (fixed 2026-10-13, all platforms)
- Symptom (user report, unconfirmed by reporter): saved connection Scrcpy args, e.g. `--no-audio`, never reach scrcpy on Windows; they do not appear in the shell window the .bat opens. Swapping in scrcpy 5.0.1 made no difference.
- Root cause: `ConsoleManager::openScrcpy()` put the device's saved arg into `line` (used only to pre-fill the prompt) while the value actually written to `scrcpy.bat` / `scrcpy.sh` was `argval`, which was only ever assigned inside the `if (scrcpy)` prompt branch. With the "Ask for Scrcpy arguments" preference off (`adblink.json` key `"scrcpy"`, default `true`), `argval` stayed empty and the saved arg was silently dropped. Shared pre-platform-branch code, so Windows/macOS/Linux were all affected; the 2023 `27a08c8` extraction carried it unchanged.
- Fix (all platforms, unversioned): `consolemanager.cpp` — `QString argval = scrcpyarg;`. The prompt still wins when enabled. Not scrcpy-version related.
- Verification: on win10, set a device's Scrcpy arg, uncheck the preference, launch scrcpy, confirm `%APPDATA%\.jocala\scripts\scrcpy.bat` contains the arg. Before the fix it read `scrcpy.exe -s <ip:port> ` with a trailing space.
- **Unfixed, tracked separately:** the arg string is concatenated unquoted into `.bat`/`.sh`. Multi-token values containing spaces break, and shell metacharacters execute — `scrcpy.sh` gets the operator's shell, `scrcpy.bat` gets cmd, and the per-device arg lives in `adblink.db`, which `syncdb.sh` copies between hosts. Tokenize with `QProcess::splitCommand()` then quote per shell (and do not reuse `adbString()`, whose POSIX escaping is wrong). Full plan, tests and adjacent findings: `~/.opencode/plan/adblink-scrcpy-args-todo.md`.

### Record delete fired a bogus `adb disconnect` (fixed 2026-10-16, all platforms)

- Symptom: deleting any device record logged an adb error, e.g. `error: no such device 'USB:5555'` followed by `disconnect: USB`.
- Cause: `DeleteRecordManager::deleteSelectedDevice()` re-read the **IP table cell** after `deleteRecord()` and ran `adb disconnect <that cell>` — with no USB guard (unlike `disconnectmanager.cpp:41` and `connectmanager.cpp:61`), no port (so adb appended its own default), and for USB records the cell is the literal string `USB`. The delete gate refuses any device whose status is connected/recovery/rescue, so the target was never connected: the call was either a no-op that logged an error, or — if the displayed status was stale — it silently dropped a live transport the user may have been using from a terminal.
- Fix: the disconnect was removed; nothing replaced it. If a teardown is ever wanted it must use the record's own `daddr:port` (the DB keeps the port separate from `daddr`) and skip USB records.

### Deliberate behaviour — do not "fix"

- **adblink kills the adb server on exit.** `onApplicationQuit()` runs `adb kill-server`, then `taskkill /f /im adb.exe` (Windows) / `pkill -9 adb` (POSIX) (`mainwindow.cpp:428-443`). Fixed policy: adblink does not leave a server running. The consequences are intended — IP devices need Connect again after an app restart, and always after a device reboot, because an adb TCP transport never survives one. Note the POSIX/Windows kill matches by process name, so it also stops adb processes adblink did not start; that breadth is deliberate too.
- **Ghostty's "Allow Ghostty to execute `<path>`?" dialog is out of scope.** It is Ghostty's own script-execution gate, raised because the macOS Ghostty path opens `~/.jocala/scripts/console.sh` as a document (`consolemanager.cpp:497`). Ghostty offers no persisted allow, so it appears on every ADB Shell click. Known workarounds: Ghostty's AppleScript API (`new surface configuration` + its `command` field, Ghostty 1.3+, needs the automation consent above) or `open -na Ghostty.app --args` (a separate instance, extra Dock icon). Both were considered and declined — suppressing another application's security prompt is not adblink's job. Do not re-investigate this as an adblink bug.

### "Refresh ADB" — CLOSED (2026-10-16)

Reported 2026-10-13 as "removed in 6.5; I have to reconnect a device constantly". The premise is wrong (an adb TCP transport never survives a device reboot) and the behaviour is now additionally by design (kill-on-exit, above). Nothing remains open:

- The widget was `refreshConnectedDevices` (`27a08c8:mainwindow.ui:313-333`), removed in the May 2025 UI rework (`c1f6c38` → `a4adba4`). It only ran `adb devices` to re-sync the view; it never ran `adb connect`.
- Restored in 8.2 as a main-window grid button backed by `RefreshAdbManager` (`refreshadbmanager.cpp`), with `tst_refreshadb`.
- Status regression fixed: the loader no longer derives IP-device status from the previous table contents (`connectedDeviceIds` is gone); it consults `m_usbStatusCache` with the `daddr:port` key, so the Status column reflects adb rather than a sticky UI flag.
- `assets/adbserver.html` no longer documents the removed buttons.
- Bulk auto-`adb connect` was deliberately **not** implemented and should not be re-opened: it cannot help Android 11+ wireless-debugging devices, whose port is random per boot (`onntab1`/`onntab2`), so their reconnect is always a discovery-driven, manual act.

### Recovery / Rescue counts as connected — landed 2026-10-16

`isConnectedStatus()` (`stringutils.cpp`) is the single predicate for "usable" (true for connected/recovery/rescue); `usbStatus()` still renders "Recovery" so the operator can see the device booted into recovery. Hardware-verified on a Lenovo TB-X606F in recovery. Companion work and the gotchas learned (status is a display string, not an enum; `sideload` is excluded on purpose) are in `~/.opencode/plan/adblink-recovery-status-todo.md`.

### Windows CRLF in the track-devices stream — fixed 2026-10-18 (8.3), Windows only

`cb1da66` replaced the line-based USB parser with a byte-exact frame reader and verified it live on macOS. adb on Windows writes the same stream through a text-mode stdout, so the wire carries CRLF while the header counts LF, and the parser drifted one byte per frame — duplicate records named `0018G074TS0361670XW6` / `001aG074TS0361670XW6`, with the device's real row reading Disconnected (the mangled keys are what lands in `m_usbStatusCache`). Fixed by stripping `\r` before framing plus a frame-validity check; both are load-bearing, and an LF-only test suite cannot catch a regression. The version confusion around the same defect: 8.2 was silently rebuilt on 2026-10-16 under the same version string, so "8.2" named two different builds — hence `buildStamp` in the log from 8.3 on. Reporter's other two findings are by design: Refresh ADB rewrites only the status cache (rows are user config) and `deleterecordmanager.cpp` refuses to delete a connected row.
