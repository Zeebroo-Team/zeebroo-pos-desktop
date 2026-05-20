<p align="center">
  <img src="pos-desktop/public/logo.png" alt="Zeebroo" width="220" />
</p>

<h1 align="center">Zeebroo POS Desktop</h1>

<p align="center">
  Qt 6 desktop point-of-sale client for <strong>Zeebroo</strong>.<br>
  Connects to the same REST API as the web <strong>Online POS</strong> (<code>Modules/Pos</code>).
</p>

<p align="center">
  <a href="https://github.com/Zeebroo-Team/zeebroo-pos-desktop/actions/workflows/build-deploy.yml">
    <img src="https://github.com/Zeebroo-Team/zeebroo-pos-desktop/actions/workflows/build-deploy.yml/badge.svg?branch=main" alt="Build" />
  </a>
</p>

<p align="center">
  <a href="https://github.com/Zeebroo-Team/zeebroo-pos-desktop">Repository</a>
  ·
  <a href="#requirements">Requirements</a>
  ·
  <a href="#build">Build</a>
  ·
  <a href="#configuration">Configuration</a>
  ·
  <a href="#deploy">Deploy</a>
</p>

---

## Features

- Sign in with Sanctum API token (`POST /api/v1/pos/auth/token`)
- Business picker (`GET /api/v1/pos/businesses`)
- Three-panel UI aligned with the web terminal: **Current sale**, **Product catalog**, **Checkout**
- Category filters, name search, and SKU scan
- Multi-batch stock layer picker when required
- Cash / card / credit checkout with numpad (`POST /api/v1/pos/online/checkout`)
- Receipt dialog after sale with **thermal printer** support (80mm paper preset)

## Requirements

| Component | Version |
|-----------|---------|
| **Qt** | 6.x (Core, Gui, Widgets, Network, Multimedia, PrintSupport) |
| **CMake** | 3.21+ |
| **Compiler** | C++17 |
| **Backend** | Running **Zeebroo** Laravel app with POS API (`laravel/sanctum`, POS module routes) |

---

## Quick start

```bash
git clone https://github.com/Zeebroo-Team/zeebroo-pos-desktop.git
cd zeebroo-pos-desktop/pos-desktop

brew install qt cmake   # macOS (Homebrew)

cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(qtpaths6 --install-prefix 2>/dev/null || echo /opt/homebrew/opt/qt)"

cmake --build build --config Release
./build/ZeebrooPosDesktop
```

Copy `config.example.json` to your config path (see [Configuration](#configuration)) or enter the API URL on the sign-in screen.

---

## Build

All application source lives under **`pos-desktop/`**.

```bash
cd pos-desktop

cmake -B build \
  -DCMAKE_PREFIX_PATH="$(qtpaths6 --install-prefix 2>/dev/null || echo /opt/homebrew/opt/qt)"

cmake --build build
./build/ZeebrooPosDesktop
```

**macOS (Homebrew):**

```bash
brew install qt cmake
```

**Output:** `build/ZeebrooPosDesktop` (macOS/Linux) or `build/Release/ZeebrooPosDesktop.exe` (Windows, depending on generator).

Optional install:

```bash
cmake --install build --prefix /opt/zeebroo-pos
```

---

## Configuration

Example (`pos-desktop/config.example.json`):

```json
{
  "api_base_url": "http://localhost:8000/api/v1/pos",
  "device_name": "pos-desktop-1"
}
```

| Key | Description |
|-----|-------------|
| `api_base_url` | Full POS API base URL |
| `device_name` | Sanctum token device label for this terminal (use a unique value per machine) |

| Platform | Config file location |
|----------|----------------------|
| **macOS** | `~/Library/Application Support/Zeebroo/PosDesktop/config.json` |
| **Linux** | `~/.config/Zeebroo/PosDesktop/config.json` |
| **Windows** | `%APPDATA%/Zeebroo/PosDesktop/config.json` |

You can also place `config.json` next to the executable, or set the API URL on the sign-in screen.

---

## Deploy

### 1. Deploy the Zeebroo backend

The desktop app does **not** bundle the API. Deploy Zeebroo first so these are available:

- HTTPS base URL (e.g. `https://pos.example.com`)
- POS API routes under `/api/v1/pos`
- Sanctum personal access tokens for terminal sign-in
- POS module enabled (accounts, products, stock layers)

API reference (in Zeebroo): `Modules/Pos/docs/API.md`  
Interactive docs: `GET https://your-app.example.com/api/v1/pos/docs`

### 2. Build and configure the terminal

```bash
cd pos-desktop
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(qtpaths6 --install-prefix 2>/dev/null || echo /opt/homebrew/opt/qt)"
cmake --build build --config Release
```

Set production values in config (see [Configuration](#configuration)):

```json
{
  "api_base_url": "https://your-app.example.com/api/v1/pos",
  "device_name": "pos-desktop-1"
}
```

### 3. Run

```bash
./build/ZeebrooPosDesktop
```

**Checklist**

- [ ] Backend URL uses **HTTPS** in production
- [ ] Firewall allows outbound HTTPS from the terminal to the Zeebroo host
- [ ] Staff user has access to the target business (`X-Business-Id` is set automatically after login)
- [ ] Cash/card checkout accounts exist in POS settings
- [ ] Thermal printer installed and selected in the OS print dialog when printing receipts

---

## CI/CD (build and deploy)

GitHub Actions workflow [`.github/workflows/build-deploy.yml`](.github/workflows/build-deploy.yml) builds and packages the app on **macOS**, **Linux**, and **Windows**.

| Trigger | What runs |
|---------|-----------|
| Push to `main` | Build all platforms; upload artifacts (14-day retention) |
| Pull request | Same builds (no release) |
| Tag `v*` (e.g. `v1.0.0`) | Builds + GitHub Release with platform archives |
| Manual | **Actions → Build and deploy → Run workflow** |

**Artifacts per platform**

| Platform | Archive |
|----------|---------|
| macOS | `ZeebrooPosDesktop-{version}-macos.zip` (`.app` + `config.example.json`) |
| Linux | `ZeebrooPosDesktop-{version}-linux-x86_64.tar.gz` |
| Windows | `ZeebrooPosDesktop-{version}-windows-x64.zip` |

**Cut a release**

```bash
git tag v1.0.0
git push origin v1.0.0
```

Download installers from the [Releases](https://github.com/Zeebroo-Team/zeebroo-pos-desktop/releases) page or from the workflow run’s **Artifacts** tab.

---

## Optional: git submodule in Zeebroo portal

You can embed this repo inside the main Zeebroo monorepo (e.g. as `pos-desktop/`):

```bash
cd zeebroo-portal
git submodule add https://github.com/Zeebroo-Team/zeebroo-pos-desktop.git pos-desktop
git submodule update --init --recursive
```

Update the submodule:

```bash
cd pos-desktop
git fetch origin && git checkout main && git pull origin main
cd ..
git add pos-desktop
git commit -m "Bump pos-desktop submodule."
```

---

## API reference

| Resource | Location |
|----------|----------|
| OpenAPI / Swagger UI | `GET /api/v1/pos/docs` on your Zeebroo host |
| Markdown guide | `Modules/Pos/docs/API.md` in the Zeebroo repo |

Authenticated requests send `Authorization: Bearer {token}` and `X-Business-Id` after login.

---

## Project layout

```
zeebroo-pos-desktop/
  .github/
    workflows/build-deploy.yml
    scripts/package-desktop.sh
  LICENSE
  README.md
  pos-desktop/
    CMakeLists.txt
    config.example.json
    public/
      logo.png
    resources/
      styles.qss
      sounds/beep.wav
      resources.qrc
    src/
      main.cpp
      core/          ApiClient, Cart, Models, Config, SaleReceipt
      ui/            LoginDialog, MainWindow, LayerPickerDialog, ReceiptDialog
```

---

## Notes

- `X-Business-Id` is sent on every authenticated request (handled automatically after login).
- Cash and card payments require a deposit account from POS settings / bootstrap `accounts`.
- When `requires_layer_pick` is true, the app prompts for a stock batch before adding to cart.

---

## Related repositories

| Repo | Role |
|------|------|
| [Zeebroo-Team/zeebroo-portal](https://github.com/Zeebroo-Team/zeebroo-portal) | Laravel application; hosts the POS API |
| [Zeebroo-Team/zeebroo-pos-desktop](https://github.com/Zeebroo-Team/zeebroo-pos-desktop) | This Qt desktop client |
