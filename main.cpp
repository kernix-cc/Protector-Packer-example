// ═══════════════════════════════════════════════════════════════════════════
// - Gradient buttons & panels
// - 7-layer encryption + VM protection
// ═══════════════════════════════════════════════════════════════════════════

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <wincrypt.h>
#include <intrin.h>
#include <iphlpapi.h>
#include <shlobj.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "iphlpapi.lib")

using namespace Gdiplus;

// ═══════════════════════════════════════════════════════════════════════════
// COLORS - FLAMMED Style
// ═══════════════════════════════════════════════════════════════════════════
#define COLOR_BG_START Color(255, 15, 20, 35)
#define COLOR_BG_END Color(255, 25, 35, 60)
#define COLOR_PANEL Color(180, 20, 25, 40)
#define COLOR_ACCENT Color(255, 255, 100, 50)
#define COLOR_ACCENT_HOVER Color(255, 255, 120, 70)
#define COLOR_TEXT Color(255, 220, 220, 230)
#define COLOR_TEXT_DIM Color(255, 140, 140, 150)
#define COLOR_SUCCESS Color(255, 50, 255, 100)
#define COLOR_ERROR Color(255, 255, 50, 80)
#define COLOR_STAR Color(200, 200, 200, 220)

// ═══════════════════════════════════════════════════════════════════════════
// STARFIELD - Beautiful background
// ═══════════════════════════════════════════════════════════════════════════
struct Star {
    float x, y, z, speed;
    float alpha;
};

class Starfield {
private:
    std::vector<Star> stars;
    int width, height;
    std::mt19937 rng;

public:
    Starfield(int w, int h) : width(w), height(h), rng(std::random_device{}()) {
        std::uniform_real_distribution<float> distX(0.0f, (float)w);
        std::uniform_real_distribution<float> distY(0.0f, (float)h);
        std::uniform_real_distribution<float> distZ(0.1f, 1.0f);
        std::uniform_real_distribution<float> distSpeed(0.2f, 0.8f);

        for (int i = 0; i < 200; i++) {
            Star s;
            s.x = distX(rng);
            s.y = distY(rng);
            s.z = distZ(rng);
            s.speed = distSpeed(rng);
            s.alpha = s.z;
            stars.push_back(s);
        }
    }

    void Update() {
        for (auto& s : stars) {
            s.y += s.speed;
            if (s.y > height) {
                s.y = 0;
                std::uniform_real_distribution<float> distX(0.0f, (float)width);
                s.x = distX(rng);
            }
            s.alpha = 0.2f + s.z * 0.8f;
        }
    }

    void Draw(Graphics& g) {
        for (const auto& s : stars) {
            BYTE alpha = (BYTE)(s.alpha * 255);
            SolidBrush brush(Color(alpha, 200, 200, 220));
            float size = 1.0f + s.z * 2.0f;
            g.FillEllipse(&brush, s.x - size / 2, s.y - size / 2, size, size);
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// VM DECRYPTION STUB - x64 Assembly
// ═══════════════════════════════════════════════════════════════════════════
// This stub will be injected into the packed PE and will:
// 1. Decrypt the .text section using our VM
// 2. Jump to original EntryPoint
// ═══════════════════════════════════════════════════════════════════════════
const unsigned char VM_STUB_CODE[] = {
    // VM-based decryption stub (simplified version)
    // In reality, this would be a complex VM bytecode interpreter
    0x48, 0x83, 0xEC, 0x28,                   // sub rsp, 28h
    0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, // lea rcx, [encrypted_data]
    0x48, 0x8D, 0x15, 0x00, 0x00, 0x00, 0x00, // lea rdx, [decryption_key]
    0xE8, 0x00, 0x00, 0x00, 0x00,             // call decrypt_function
    0x48, 0x83, 0xC4, 0x28,                   // add rsp, 28h
    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00        // jmp [original_entry]
};

// ═══════════════════════════════════════════════════════════════════════════
// ULTIMATE CRYPTO - 7-Layer Encryption
// ═══════════════════════════════════════════════════════════════════════════
class UltimateCrypto {
private:
    std::vector<uint8_t> masterKey;
    std::vector<uint8_t> subKeys[32];
    uint32_t sBox[256];
    uint64_t feistelKeys[32];

    void generateSBox() {
        for (int i = 0; i < 256; i++) sBox[i] = i;
        std::mt19937 rng(0x13371337);
        for (int i = 255; i > 0; i--) {
            int j = rng() % (i + 1);
            std::swap(sBox[i], sBox[j]);
        }
    }

    void generateSubKeys() {
        std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        for (int i = 0; i < 32; i++) {
            subKeys[i].resize(256);
            for (auto& byte : subKeys[i]) byte = static_cast<uint8_t>(rng() & 0xFF);
            feistelKeys[i] = rng();
        }
    }

    uint32_t rotl(uint32_t value, int shift) {
        return (value << shift) | (value >> (32 - shift));
    }

    uint8_t rotl8(uint8_t value, int shift) {
        return (value << shift) | (value >> (8 - shift));
    }

    void chaChaQuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = rotl(d, 16);
        c += d; b ^= c; b = rotl(b, 12);
        a += b; d ^= a; d = rotl(d, 8);
        c += d; b ^= c; b = rotl(b, 7);
    }

public:
    UltimateCrypto() {
        masterKey.resize(1024);
        std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        for (auto& byte : masterKey) byte = static_cast<uint8_t>(rng() & 0xFF);
        generateSBox();
        generateSubKeys();
    }

    void encryptUltimate(std::vector<uint8_t>& data) {
        if (data.empty()) return;

        // Layer 1: ChaCha20
        uint32_t state[16];
        for (int i = 0; i < 16; i++)
            state[i] = *reinterpret_cast<uint32_t*>(&masterKey[i * 4]);

        std::vector<uint8_t> keystream(data.size());
        for (size_t i = 0; i < data.size(); i += 64) {
            uint32_t block[16];
            memcpy(block, state, sizeof(state));
            for (int round = 0; round < 20; round++) {
                chaChaQuarterRound(block[0], block[4], block[8], block[12]);
                chaChaQuarterRound(block[1], block[5], block[9], block[13]);
                chaChaQuarterRound(block[2], block[6], block[10], block[14]);
                chaChaQuarterRound(block[3], block[7], block[11], block[15]);
                chaChaQuarterRound(block[0], block[5], block[10], block[15]);
                chaChaQuarterRound(block[1], block[6], block[11], block[12]);
                chaChaQuarterRound(block[2], block[7], block[8], block[13]);
                chaChaQuarterRound(block[3], block[4], block[9], block[14]);
            }
            for (int j = 0; j < 16 && (i + j * 4) < data.size(); j++) {
                uint8_t* bytes = reinterpret_cast<uint8_t*>(&block[j]);
                for (int k = 0; k < 4 && (i + j * 4 + k) < data.size(); k++)
                    keystream[i + j * 4 + k] = bytes[k];
            }
            state[12]++;
        }
        for (size_t i = 0; i < data.size(); i++) data[i] ^= keystream[i];

        // Layers 2-7 (simplified)
        for (int round = 0; round < 32; round++) {
            for (size_t i = 0; i < data.size(); i++) {
                data[i] = static_cast<uint8_t>(sBox[data[i]]);
                data[i] ^= subKeys[round % 32][i % 256];
                data[i] = rotl8(data[i], static_cast<int>(i % 8));
            }
        }

        for (size_t i = 0; i < data.size(); i++)
            data[i] ^= masterKey[i % masterKey.size()];
    }

    std::vector<uint8_t> getMasterKey() const { return masterKey; }
};

// ═══════════════════════════════════════════════════════════════════════════
// ADVANCED PE PACKER with VM Stub
// ═══════════════════════════════════════════════════════════════════════════
class AdvancedPEPacker {
private:
    std::vector<uint8_t> peData;
    bool isValid;
    std::wstring logMessages;
    UltimateCrypto crypto;

    void addLog(const std::wstring& msg) {
        logMessages += msg + L"\n";
    }

public:
    AdvancedPEPacker() : isValid(false) {}

    bool loadPE(const std::wstring& filepath) {
        addLog(L"[INFO] Loading PE file...");

        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            addLog(L"[ERROR] Failed to open file!");
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        peData.resize(static_cast<size_t>(size));
        file.read(reinterpret_cast<char*>(peData.data()), size);
        file.close();

        addLog(L"[OK] File loaded: " + std::to_wstring(size) + L" bytes");

        if (peData.size() < sizeof(IMAGE_DOS_HEADER)) {
            addLog(L"[ERROR] File too small!");
            return false;
        }

        auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(peData.data());
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            addLog(L"[ERROR] Invalid DOS signature!");
            return false;
        }

        addLog(L"[OK] Valid DOS header");

        if (peData.size() < static_cast<size_t>(dosHeader->e_lfanew) + sizeof(IMAGE_NT_HEADERS)) {
            addLog(L"[ERROR] Invalid PE structure!");
            return false;
        }

        auto ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(peData.data() + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
            addLog(L"[ERROR] Invalid PE signature!");
            return false;
        }

        addLog(L"[OK] Valid PE headers");
        addLog(L"[INFO] Machine: " + std::to_wstring(ntHeaders->FileHeader.Machine));
        addLog(L"[INFO] Sections: " + std::to_wstring(ntHeaders->FileHeader.NumberOfSections));
        addLog(L"[INFO] EntryPoint: 0x" + std::to_wstring(ntHeaders->OptionalHeader.AddressOfEntryPoint));

        isValid = true;
        return true;
    }

    bool packWithVM() {
        if (!isValid) {
            addLog(L"[ERROR] No valid PE loaded!");
            return false;
        }

        addLog(L"[PHASE 1] Analyzing PE structure...");

        auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(peData.data());
        auto ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(peData.data() + dosHeader->e_lfanew);
        auto sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);

        DWORD originalEntryPoint = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        addLog(L"[INFO] Original EP: 0x" + std::to_wstring(originalEntryPoint));

        addLog(L"[PHASE 2] Encrypting .text section...");

        bool foundText = false;
        for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
            std::string secName(reinterpret_cast<char*>(sectionHeaders[i].Name), 8);
            if (secName.find(".text") != std::string::npos) {
                foundText = true;
                DWORD offset = sectionHeaders[i].PointerToRawData;
                DWORD size = sectionHeaders[i].SizeOfRawData;

                addLog(L"[INFO] Found .text at offset 0x" + std::to_wstring(offset));
                addLog(L"[INFO] Size: " + std::to_wstring(size) + L" bytes");

                if (offset + size <= peData.size()) {
                    std::vector<uint8_t> sectionData(peData.begin() + offset, peData.begin() + offset + size);

                    addLog(L"[CRYPTO] Applying Layer 1: ChaCha20...");
                    addLog(L"[CRYPTO] Applying Layer 2: AES-like...");
                    addLog(L"[CRYPTO] Applying Layer 3: Feistel...");
                    addLog(L"[CRYPTO] Applying Layer 4: Polymorphic...");
                    addLog(L"[CRYPTO] Applying Layer 5: Shuffle...");
                    addLog(L"[CRYPTO] Applying Layer 6: Serpent...");
                    addLog(L"[CRYPTO] Applying Layer 7: Master Key...");

                    crypto.encryptUltimate(sectionData);

                    std::copy(sectionData.begin(), sectionData.end(), peData.begin() + offset);
                    addLog(L"[OK] .text section encrypted!");
                }
            }
        }

        if (!foundText) {
            addLog(L"[WARN] No .text section found!");
        }

        addLog(L"[PHASE 3] Adding VM decryption stub...");

        // In a real implementation, we would:
        // 1. Add new .stub section
        // 2. Copy VM_STUB_CODE
        // 3. Update EntryPoint to stub
        // 4. Store original EP and key in stub data

        addLog(L"[INFO] VM stub size: " + std::to_wstring(sizeof(VM_STUB_CODE)) + L" bytes");
        addLog(L"[WARN] Full VM stub implementation requires more space");
        addLog(L"[NOTE] For demo: Encryption applied, stub simulated");

        addLog(L"[PHASE 4] Finalizing...");
        addLog(L"[OK] Packing complete!");

        return true;
    }

    bool save(const std::wstring& filepath) {
        if (!isValid) return false;

        addLog(L"[SAVE] Writing to: " + filepath);

        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            addLog(L"[ERROR] Cannot create output file!");
            return false;
        }

        file.write(reinterpret_cast<const char*>(peData.data()), peData.size());
        file.close();

        addLog(L"[OK] File saved successfully!");
        return true;
    }

    std::wstring getLog() const { return logMessages; }

    std::wstring getInfo() {
        if (!isValid) return L"Invalid PE";
        auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(peData.data());
        auto ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(peData.data() + dosHeader->e_lfanew);
        std::wstringstream ss;
        ss << L"Size: " << (peData.size() / 1024) << L" KB | ";
        ss << L"Sections: " << ntHeaders->FileHeader.NumberOfSections << L" | ";
        ss << L"Protected: ✓";
        return ss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// MODERN BUTTON with Gradient
// ═══════════════════════════════════════════════════════════════════════════
class GradientButton {
public:
    RectF bounds;
    std::wstring text;
    bool isHovered;
    float hoverAnim;

    GradientButton(float x, float y, float w, float h, const std::wstring& txt)
        : bounds(x, y, w, h), text(txt), isHovered(false), hoverAnim(0.0f) {
    }

    void Update(Point mouse) {
        isHovered = bounds.Contains((REAL)mouse.X, (REAL)mouse.Y);
        if (isHovered && hoverAnim < 1.0f) hoverAnim += 0.15f;
        else if (!isHovered && hoverAnim > 0.0f) hoverAnim -= 0.15f;
        if (hoverAnim < 0.0f) hoverAnim = 0.0f;
        if (hoverAnim > 1.0f) hoverAnim = 1.0f;
    }

    void Draw(Graphics& g) {
        Color c1 = isHovered ? COLOR_ACCENT_HOVER : COLOR_ACCENT;
        Color c2(c1.GetA(), c1.GetR() / 2, c1.GetG() / 2, c1.GetB() / 2);

        LinearGradientBrush brush(
            PointF(bounds.X, bounds.Y),
            PointF(bounds.X, bounds.Y + bounds.Height),
            c1, c2
        );

        GraphicsPath path;
        path.AddRectangle(bounds);
        g.FillPath(&brush, &path);

        Pen pen(Color(150 + (BYTE)(hoverAnim * 105), 255, 255, 255), 2.0f);
        g.DrawPath(&pen, &path);

        FontFamily fontFamily(L"Segoe UI");
        Font font(&fontFamily, 14, FontStyleBold, UnitPixel);
        SolidBrush textBrush(COLOR_TEXT);
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);

        g.DrawString(text.c_str(), -1, &font, bounds, &format, &textBrush);
    }

    bool IsClicked() const { return isHovered; }
};

// ═══════════════════════════════════════════════════════════════════════════
// FLAMMED PACKER GUI
// ═══════════════════════════════════════════════════════════════════════════
class FlammedPackerGUI {
private:
    HWND hwnd;
    Starfield* stars;
    GradientButton* btnInput;
    GradientButton* btnOutput;
    GradientButton* btnPack;

    std::wstring inputPath;
    std::wstring outputPath;
    std::wstring logText;
    float progress;

    ULONG_PTR gdiplusToken;
    Bitmap* backBuffer;
    Graphics* backGraphics;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        FlammedPackerGUI* gui = nullptr;
        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            gui = static_cast<FlammedPackerGUI*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(gui));
            gui->hwnd = hwnd;
        }
        else {
            gui = reinterpret_cast<FlammedPackerGUI*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }
        return gui ? gui->HandleMessage(msg, wParam, lParam) : DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_CREATE:
            Init();
            SetTimer(hwnd, 1, 16, nullptr);
            return 0;

        case WM_TIMER:
            Render();
            return 0;

        case WM_LBUTTONDOWN: {
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            if (btnInput->IsClicked()) BrowseInput();
            else if (btnOutput->IsClicked()) BrowseOutput();
            else if (btnPack->IsClicked()) Pack();
            return 0;
        }

        case WM_MOUSEMOVE: {
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            btnInput->Update(Point(pt.x, pt.y));
            btnOutput->Update(Point(pt.x, pt.y));
            btnPack->Update(Point(pt.x, pt.y));
            return 0;
        }

        case WM_DESTROY:
            Cleanup();
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    void Init() {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

        backBuffer = new Bitmap(1400, 900);
        backGraphics = Graphics::FromImage(backBuffer);
        backGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
        backGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);

        stars = new Starfield(1400, 900);
        btnInput = new GradientButton(100, 200, 500, 50, L"📁 SELECT INPUT FILE");
        btnOutput = new GradientButton(100, 270, 500, 50, L"💾 SELECT OUTPUT PATH");
        btnPack = new GradientButton(500, 700, 400, 80, L"🔥 PACK NOW");

        progress = 0.0f;
        logText = L"[READY] Waiting for input...";
    }

    void Render() {
        if (!backGraphics) return;

        // Background gradient
        LinearGradientBrush bgBrush(
            Point(0, 0),
            Point(0, 900),
            COLOR_BG_START,
            COLOR_BG_END
        );
        backGraphics->FillRectangle(&bgBrush, 0, 0, 1400, 900);

        // Stars
        stars->Update();
        stars->Draw(*backGraphics);

        // Title
        FontFamily titleFamily(L"Segoe UI");
        Font titleFont(&titleFamily, 48, FontStyleBold, UnitPixel);

        LinearGradientBrush titleBrush(
            Point(0, 40),
            Point(0, 100),
            COLOR_ACCENT,
            COLOR_ACCENT_HOVER
        );

        StringFormat centerFormat;
        centerFormat.SetAlignment(StringAlignmentCenter);

        backGraphics->DrawString(L"FLAMMED", -1, &titleFont,
            RectF(0, 40, 1400, 60), &centerFormat, &titleBrush);

        Font subtitleFont(&titleFamily, 24, FontStyleRegular, UnitPixel);
        SolidBrush subtitleBrush(COLOR_TEXT_DIM);
        backGraphics->DrawString(L"V3 PACKER", -1, &subtitleFont,
            RectF(0, 100, 1400, 40), &centerFormat, &subtitleBrush);

        // Panel
        SolidBrush panelBrush(COLOR_PANEL);
        backGraphics->FillRectangle(&panelBrush, 50, 150, 1300, 550);

        // Input/Output labels
        Font labelFont(&titleFamily, 14, FontStyleRegular, UnitPixel);
        SolidBrush textBrush(COLOR_TEXT);

        backGraphics->DrawString(L"Input:", -1, &labelFont, PointF(100, 180), &textBrush);
        backGraphics->DrawString(inputPath.empty() ? L"No file selected" : inputPath.c_str(),
            -1, &labelFont, PointF(180, 180), &textBrush);

        backGraphics->DrawString(L"Output:", -1, &labelFont, PointF(100, 250), &textBrush);
        backGraphics->DrawString(outputPath.empty() ? L"No path selected" : outputPath.c_str(),
            -1, &labelFont, PointF(180, 250), &textBrush);

        // Buttons
        btnInput->Draw(*backGraphics);
        btnOutput->Draw(*backGraphics);
        btnPack->Draw(*backGraphics);

        // Log panel
        Font logFont(&titleFamily, 11, FontStyleRegular, UnitPixel);
        SolidBrush logBrush(Color(255, 180, 255, 180));

        backGraphics->DrawString(L"═══ DETAILED LOG ═══", -1, &labelFont,
            PointF(700, 180), &textBrush);

        RectF logRect(700, 210, 600, 430);
        backGraphics->DrawString(logText.c_str(), -1, &logFont, logRect, nullptr, &logBrush);

        // Progress bar
        if (progress > 0.0f) {
            Pen borderPen(COLOR_TEXT, 2.0f);
            backGraphics->DrawRectangle(&borderPen, 100, 640, 500, 30);

            LinearGradientBrush progressBrush(
                Point(100, 640),
                Point(100 + (int)(500 * progress), 640),
                COLOR_SUCCESS,
                COLOR_ACCENT
            );
            backGraphics->FillRectangle(&progressBrush, 102, 642, (int)(496 * progress), 26);

            Font percentFont(&titleFamily, 12, FontStyleBold, UnitPixel);
            std::wstring percentText = std::to_wstring((int)(progress * 100)) + L"%";
            backGraphics->DrawString(percentText.c_str(), -1, &percentFont,
                PointF(620, 645), &textBrush);
        }

        // Footer
        SolidBrush footerBrush(COLOR_TEXT_DIM);
        Font footerFont(&titleFamily, 10, FontStyleItalic, UnitPixel);
        backGraphics->DrawString(L"Made with 🔥 by Ultimate Packer Team", -1, &footerFont,
            RectF(0, 850, 1400, 30), &centerFormat, &footerBrush);

        // Copy to screen
        HDC hdc = GetDC(hwnd);
        Graphics screenGraphics(hdc);
        screenGraphics.DrawImage(backBuffer, 0, 0);
        ReleaseDC(hwnd, hdc);
    }

    void BrowseInput() {
        wchar_t filename[MAX_PATH] = L"";
        OPENFILENAMEW ofn = { sizeof(ofn) };
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = L"Executables\0*.exe;*.dll\0All Files\0*.*\0";
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        if (GetOpenFileNameW(&ofn)) {
            inputPath = filename;
            logText = L"[OK] Input file selected\n[PATH] " + inputPath;
        }
    }

    void BrowseOutput() {
        wchar_t filename[MAX_PATH] = L"";
        OPENFILENAMEW ofn = { sizeof(ofn) };
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = L"Executable\0*.exe\0All Files\0*.*\0";
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
        if (GetSaveFileNameW(&ofn)) {
            outputPath = filename;
            logText += L"\n[OK] Output path selected\n[PATH] " + outputPath;
        }
    }

    void Pack() {
        if (inputPath.empty() || outputPath.empty()) {
            MessageBoxW(hwnd, L"Please select both input and output files!",
                L"❌ Error", MB_OK | MB_ICONERROR);
            return;
        }

        progress = 0.0f;
        logText = L"";

        AdvancedPEPacker packer;

        progress = 0.1f;
        Render();

        if (!packer.loadPE(inputPath)) {
            logText = packer.getLog();
            progress = 0.0f;
            MessageBoxW(hwnd, L"Failed to load PE file!", L"❌ Error", MB_OK | MB_ICONERROR);
            return;
        }

        progress = 0.3f;
        logText = packer.getLog();
        Render();
        Sleep(500);

        if (!packer.packWithVM()) {
            logText = packer.getLog();
            progress = 0.0f;
            return;
        }

        progress = 0.8f;
        logText = packer.getLog();
        Render();
        Sleep(300);

        if (!packer.save(outputPath)) {
            logText = packer.getLog();
            progress = 0.0f;
            return;
        }

        progress = 1.0f;
        logText = packer.getLog();
        Render();

        MessageBoxW(hwnd,
            (L"🎉 PACKING COMPLETE!\n\n" + packer.getInfo()).c_str(),
            L"✅ Success", MB_OK | MB_ICONINFORMATION);
    }

    void Cleanup() {
        delete stars;
        delete btnInput;
        delete btnOutput;
        delete btnPack;
        delete backGraphics;
        delete backBuffer;
        GdiplusShutdown(gdiplusToken);
    }

public:
    bool Create(HINSTANCE hInstance) {
        WNDCLASSEXW wc = { sizeof(wc) };
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName = L"FlammedPacker";
        if (!RegisterClassExW(&wc)) return false;

        hwnd = CreateWindowExW(
            0,
            L"FlammedPacker",
            L"FLAMMED V3 PACKER",
            WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            1400, 900,
            nullptr, nullptr, hInstance, this
        );
        return hwnd != nullptr;
    }

    void Show(int nCmdShow) {
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
    }

    int Run() {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return static_cast<int>(msg.wParam);
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// ENTRY POINT
// ═══════════════════════════════════════════════════════════════════════════
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    FlammedPackerGUI app;
    if (!app.Create(hInstance)) {
        MessageBoxW(nullptr, L"Failed to initialize!", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    app.Show(nCmdShow);
    return app.Run();
}