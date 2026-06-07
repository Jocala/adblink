# adblink — agent guidance

## Build & test

```sh
cmake -S . -B "$Env:USERPROFILE\build-adblink" -DCMAKE_PREFIX_PATH="C:\Qt\Qt.6.11.1-static"
cmake --build "$Env:USERPROFILE\build-adblink" --config Release
ctest --test-dir "$Env:USERPROFILE\build-adblink" --output-on-failure -C Release
```

Single test:
```sh
cmake --build "$Env:USERPROFILE\build-adblink" --target <test_name> --config Release
```

Or use the build script:
```sh
X:\adblink\build-adblink-windows.ps1
```

11 test executables: `adbstring`, `database`, `jsonconfig`, `logfile`, `getadbpath`, `devicerecord`, `iptablesort`, `stringutils`, `getreturncode`, `devicerecorddialog`, `logviewerdialog`.

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
- `onTrackOutput()`: reads lines (format `SERIAL\tSTATUS`), strips leading digit prefixes from serial numbers, updates `m_usbStatusCache` (QHash<QString,QString>), starts 150ms debounce timer.
- `applyDebouncedRefresh()` (debounce timeout): iterates cache, skips network devices (contains `:`), skips non-`"device"` status, skips existing DB records, then INSERTs a new device record with default Kodi values. Duplicate descriptions get `-2`, `-3`, etc. suffix.
- `m_quitting` guard in `onTrackFinished` prevents restart during shutdown.

### Status display
`usbStatus()` in `stringutils.cpp`: maps cache values to display text — `"device"` → `"Connected"`, missing → `"Disconnected"`, otherwise capitalize first letter (`"unauthorized"` → `"Unauthorized"`).

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

## Database schema & patterns

### `device` table (20 columns)
Columns: `Id INTEGER PRIMARY KEY AUTOINCREMENT`, `daddr TEXT`, `description TEXT NOT NULL UNIQUE`, `pulldir`, `xbmcpackage`, `data_root`, `buffermode INTEGER`, `buffersize`, `bufferfactor`, `filepath`, `port`, `isusb INTEGER`, `ostype TEXT`, `logfilename TEXT`, `disableroot INTEGER`, `flag1` (scoped), `flag2` (wsa), `flag3`, `flag4`, `flag5` (scrcpyarg).

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
- `adbString(input)`: escape single quotes, wrap in double quotes.

## Preferences (`adblink.json`)

Managed by `PreferencesManager`. Dialog (`adbprefDialog`, fixed 300x500) has:
- 3 checkboxes (version check, scrcpy args, Kodi startup view)
- 5 combos (default window size, 4 font sizes)
- Terminal type (macOS: Terminal/iTerm2; Linux: Gnome/XFCE4/Konsole)
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
- Packaging: NSIS (Win), DragNDrop (macOS), TGZ (Linux).

## Code conventions

- `QMessageBox` must use explicit `setWindowModality(Qt::WindowModal)` — never static `QMessageBox::*` helpers.
- `adb` references are lowercase (`adblink`, not `adbLink`).
- `NoHScrollTableWidget` disables horizontal scrollbar via `ScrollBarAlwaysOff`.
- Oculus VR slot (`on_actionOculus_VR_triggered`) compilable but not connected to any menu action — dead code.
- **No comments** in new code unless clarifying non-obvious intent.

## Help system

- HTML help files in `assets/`, loaded via `qrc:/assets/<file>.html`.
- Shared stylesheet: `assets/style.css`. Help dialog uses `QTextBrowser` (not QTextEdit) with `setSource(QUrl("qrc:/assets/menu.html"))` for hyperlink navigation.
- `buttons.html` is the primary index for action-linked pages.
- `menu.html` is the top-level topic index.

## Key paths

- `version.h` → `VERSION "8.0"`
- `program.h` → `PROGRAM "adblink"`
- `changelog.txt` — full history
- `adbfiles/<platform>/` — bundled ADB and tool binaries per OS
- `logfile.cpp` — thread-safe logging singleton
- `adbutils.cpp` — `syncWaitForProcess()`, `isScopedStorage()`, `resolveKodiPath()`
- `kodidatamanager.cpp` — CRUD for device records, schema creation, JSON config defaults
- `mainwindow.cpp` (2815 lines) — all button slots, track-devices, view switching, layout
