#pragma once

#define tozic1 retard
#define tozic2 9 year old
#define tozic3 paster
#define tozic4 skid
#define tozic5 80kg
#define tozic6 4'10
#define tozic7 6'4 sideways

#pragma region DirectClassAccess

template <typename Ret, typename Type>
// The type and offset
Ret& DirectAccess(Type* type, size_t offset) {
    union {
        size_t raw;
        Type* source;
        Ret* target;
    } u;
    u.source = type;
    u.raw += offset;
    return *u.target;
}

#define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name
#define DEF_FIELD_RW(type, name) __declspec(property(get = get##name, put = set##name)) type name

#define FAKE_FIELD(type, name)                                                                                       \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name()

#define BUILD_ACCESS(type, name, offset)                                                                             \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name() const { return DirectAccess<type>(this, offset); }												 \
void set##name(type v) const { DirectAccess<type>(this, offset) = v; }

#pragma endregion

namespace Utils {

#pragma region Memory & Debug
#define logDebug(str) OutputDebugString(str)

    // CallFunc is what we'll be using to call the original functions.
    // CallVFunc is what we'll be using to call the vftables by there index.

    // Combine is what we''ll be using to combine strings, ints, arrays etc together.

    // HookFunction is just a clean MinHook util it'll initialize minhook for us if it hasn't already been initialized and created/enabled 
    // the hook aswell as return log information to the debug stream so we can see if hooks fail to create/enable and where they failed

    template <typename R, typename... Args>
    R CallFunc(void* func, Args... args)
    {
        // Call the function and return the result.
        return ((R(*)(Args...))func)(args...);
    }

    template <unsigned int IIdx, typename TRet, typename... TArgs>
    static inline auto CallVFunc(void* thisptr, TArgs... argList) -> TRet {
        using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
        // Call the function and return the result.
        return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
    }

    uintptr_t ResolveRef(uintptr_t ptr, int offset = 1) {
        if (!ptr)
            return 0;

        uintptr_t newAddr = ptr + *reinterpret_cast<int*>(ptr + offset) + (offset + 4);
        return newAddr;
    }

    // template function that converts the specified value to a string using a stringstream
    template <typename T>
    std::string combine(T t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

    // template function that converts the specified values to a string using a stringstream
    template <typename T, typename... Args>
    std::string combine(T t, Args... args)
    {
        std::stringstream ss;
        ss << t << combine(args...);
        return ss.str();
    }

    static const uintptr_t RangeStart = reinterpret_cast<uintptr_t>(GetModuleHandleA("Minecraft.Windows.exe"));

    // template function that creates a hook at the specified target function with the provided detour function and stores the original function in the specified location
    template <typename T>
    bool HookFunction(void* pTarget, T pDetour, void* pOriginal, const char* SignatureIdName) {
        static bool initialized = false;
        if (!initialized)
        {
            initialized = true;
            if (MH_Initialize() != MH_OK)
            {
                //log("Error: Failed to initialize MinHook\n");
                FileUtils::debugOutput(xorstr_("[Rise] failed to initialize MinHook for the client"));
                return false;
            }
        }

        if (MH_CreateHook(pTarget, pDetour, (LPVOID*)pOriginal) != MH_OK)
        {
            //log("Error: Failed to create hook\n");
            FileUtils::debugOutput(combine(xorstr_("[Rise] failed to create hook for "), SignatureIdName).c_str());
            return false;
        }

        if (MH_EnableHook(pTarget) != MH_OK)
        {
            //log("Error: Failed to enable hook\n");
            FileUtils::debugOutput(combine(xorstr_("[Rise] failed to enable hook for "), SignatureIdName).c_str());
            return false;
        }

        //log(combine("Success: Enabled hook for ", pTarget, "\n").c_str());
        FileUtils::debugOutput(combine(xorstr_("[Rise] successfully applied hook for "), SignatureIdName).c_str());

        return true;
    }

    static inline void nopBytes(void* dst, unsigned int size) {
        DWORD oldprotect;
        VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memset(dst, 0x90, size);
        VirtualProtect(dst, size, oldprotect, &oldprotect);
    };

    static inline void patchBytes(void* dst, void* src, unsigned int size) {
        DWORD oldprotect;
        VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memcpy(dst, src, size);
        VirtualProtect(dst, size, oldprotect, &oldprotect);
    };

    // macro that checks if a value is within a range
#define INRANGE(x, a, b) (x >= a && x <= b)

// macro that converts two characters to a single byte
#define GET_BYTE(x) ((GET_BITS(x[0]) << 4) | GET_BITS(x[1]))

// macro that converts a character to a nibble (4 bits)
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? (x - '0') : 0))

// This function searches for a pattern of bytes within the memory range specified by range_start and range_end
// The pattern to search for is specified as a string, where each byte in the pattern is represented by 2 hexadecimal characters
// For example, the pattern "90 90 90 90" would represent the byte sequence 0x90 0x90 0x90 0x90
// If the pattern is found within the specified memory range, a pointer to the start of the pattern is returned
// Otherwise, a null pointer is returne
    static void* findSig(const char* sig) {
        // pointer to the current position in the pattern
        const char* pattern = sig;

        // store the first matched address
        uintptr_t first_match = 0;

        // store the first matched address
        static const uintptr_t range_start = reinterpret_cast<uintptr_t>(GetModuleHandleA("Minecraft.Windows.exe"));

        // store information about the module
        static MODULEINFO mi_mod_info;

        // flag to initialize mi_mod_info only once
        static bool init = false;
        if (!init) {
            // initialize mi_mod_info
            init = true;
            GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(range_start), &mi_mod_info, sizeof(MODULEINFO));
        }
        // calculate the end address of the module
        static const uintptr_t range_end = range_start + mi_mod_info.SizeOfImage;

        // store the current byte in the pattern as a BYTE
        BYTE pat_byte = GET_BYTE(pattern);

        // store the original position in the pattern
        const char* old_pat = pattern;

        // iterate through all addresses in the module
        for (uintptr_t p_cur = range_start; p_cur < range_end; p_cur++) {
            // if we reached the end of the pattern, return the first matched address
            if (!*pattern) {
                return reinterpret_cast<void*>(first_match);
            }

            // skip spaces in the pattern
            while (*(PBYTE)pattern == ' ') {
                pattern++;
            }

            // if we reached the end of the pattern, return the first matched address
            if (!*pattern) {
                return reinterpret_cast<void*>(first_match);
            }

            // if the pattern has changed from the last iteration, update the pat_byte value
            if (old_pat != pattern) {
                old_pat = pattern;
                // if the current byte in the pattern is not a wildcard '?', update pat_byte to the current byte value
                if (*(PBYTE)pattern != '\?') {
                    pat_byte = GET_BYTE(pattern);
                }
            }

            // if the current byte in the pattern is a wildcard or if the current byte in the pattern matches the current byte in the range,
            // increment the pattern by 2 and set first_match to the current address if it is not already set
            if (*(PBYTE)pattern == '\?' || *(BYTE*)p_cur == pat_byte) {
                if (!first_match) {
                    first_match = p_cur;
                }

                // if the next byte in the pattern is the end, return the first matched address
                if (!pattern[2]) {
                    return reinterpret_cast<void*>(first_match);
                }

                pattern += 2;
            }
            else { // if the current byte in the pattern does not match the current byte in the range, reset the pattern to the start and clear first_match
                pattern = sig;
                first_match = 0;
            }
        }
    }
#pragma endregion

#pragma region Utils

    static inline bool leftClick, leftDown, rightClick, rightDown, middleClick, middleDown;

    static void onMouseClick(int key, bool isDown)
    {
        switch (key)
        {
        case 1:
            leftClick = isDown;
            leftDown = isDown ? true : leftDown;
            break;
        case 2:
            rightClick = isDown;
            rightDown = isDown ? true : rightDown;
            break;
        case 3:
            middleClick = isDown;
            middleDown = isDown ? true : middleDown;
            break;
        }
    }

    static bool invalidChar(char c) {
        return !(c >= 0 && *reinterpret_cast<unsigned char*>(&c) < 128);
    }

    static void systemPlaySound(std::string name) {
        std::string path = FileUtils::getResourcePath() + "\\" + name.c_str();
        PlaySoundA((LPCSTR)path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    }

    // UTF-8
    static size_t getUTF8CharLength(unsigned char uc) {
        if (uc <= 0x7F) return 1;
        if (uc >= 0xC2 && uc <= 0xDF) return 2;
        if (uc >= 0xE0 && uc <= 0xEF) return 3;
        if (uc >= 0xF0 && uc <= 0xF4) return 4;
        return 0;  // Handle invalid UTF-8 characters if needed
    }

    static bool isValidContinuationByte(unsigned char uc) {
        return (uc & 0xC0) == 0x80;
    }

    // UTF-16
    static bool isHighSurrogate(uint16_t uc) {
        return (uc >= 0xD800 && uc <= 0xDBFF);
    }

    static bool isLowSurrogate(uint16_t uc) {
        return (uc >= 0xDC00 && uc <= 0xDFFF);
    }

    static bool isValidUTF16Start(uint16_t uc) {
        return (uc < 0xD800 || uc > 0xDFFF);
    }

    // UTF-32 
    static bool isValidUTF32Char(uint32_t uc) {
        return (uc <= 0x10FFFF);
    }

    static std::string sanitize(const std::string& text) {
        std::string sanitizedText;
        sanitizedText.reserve(text.size());
        size_t index = 0;
        size_t textLength = text.size();

        while (index < textLength) {
            unsigned char currentChar = static_cast<unsigned char>(text[index]);

            // skip § cuz we homeless sorry
            if (currentChar == 0xC2 && index + 1 < textLength && static_cast<unsigned char>(text[index + 1]) == 0xA7) {
                index += 2; // Move past '§'
                if (index < textLength) {
                    ++index; // skip 1 more lol
                }
                continue;
            }

            size_t charLength = getUTF8CharLength(currentChar);

            if (charLength == 0 || index + charLength > textLength) {
                std::cerr << "Invalid start byte at index " << index << ": " << static_cast<int>(currentChar) << std::endl;
                ++index;
                continue;
            }

            bool isValidSequence = true;
            for (size_t offset = 1; offset < charLength; ++offset) {
                if (!isValidContinuationByte(static_cast<unsigned char>(text[index + offset]))) {
                    std::cerr << "Invalid continuation byte at index " << (index + offset) << ": " << static_cast<int>(text[index + offset]) << std::endl;
                    isValidSequence = false;
                    break;
                }
            }

            if (isValidSequence) {
                sanitizedText.append(text, index, charLength);
                index += charLength;
            }
            else {
                ++index;
            }
        }

        return sanitizedText;
    }

    //utf16
    static std::u16string sanitize(const std::u16string& text) {
        std::u16string sanitizedText;
        sanitizedText.reserve(text.size());
        size_t index = 0;
        size_t textLength = text.size();

        while (index < textLength) {
            uint16_t currentChar = text[index];

            // skip § cuz we homeless sorry
            if (currentChar == 0x00A7 && index + 1 < textLength) {
                index += 2; // Move past '§'
                if (index < textLength) {
                    ++index; // skip 1 more lol
                }
                continue;
            }

            if (isHighSurrogate(currentChar)) {
                if (index + 1 < textLength && isLowSurrogate(text[index + 1])) {
                    sanitizedText.append(text, index, 2);
                    index += 2;
                }
                else {
                    std::cerr << "Invalid high surrogate at index " << index << ": " << currentChar << std::endl;
                    ++index;
                }
            }
            else if (isValidUTF16Start(currentChar)) {
                sanitizedText.append(text, index, 1);
                ++index;
            }
            else {
                std::cerr << "Invalid UTF-16 character at index " << index << ": " << currentChar << std::endl;
                ++index;
            }
        }

        return sanitizedText;
    }

    //utf32
    static std::u32string sanitize(const std::u32string& text) {
        std::u32string sanitizedText;
        sanitizedText.reserve(text.size());
        size_t index = 0;
        size_t textLength = text.size();

        while (index < textLength) {
            uint32_t currentChar = text[index];

            // skip § cuz we homeless sorry
            if (currentChar == 0x000000A7 && index + 1 < textLength) {
                index += 2;
                if (index < textLength) {
                    ++index; // skip 1 more lol
                }
                continue;
            }

            if (isValidUTF32Char(currentChar)) {
                sanitizedText.append(text, index, 1);
            }
            else {
                std::cerr << "Invalid UTF-32 character at index " << index << ": " << currentChar << std::endl;
            }
            ++index;
        }

        return sanitizedText;
    }

    static inline void toastNotification(std::string title, std::string message) {
        ToastNotification toast = ToastNotification(ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastImageAndText02));
        winrt::Windows::Data::Xml::Dom::IXmlNodeList elem = toast.Content().GetElementsByTagName(L"text");
        elem.Item(0).InnerText(winrt::to_hstring(title));
        elem.Item(1).InnerText(winrt::to_hstring(message));
        ToastNotificationManager::CreateToastNotifier().Show(toast);
    }

#pragma endregion
}