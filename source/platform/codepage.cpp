#include <tvision/tv.h>

#include <internal/codepage.h>
#include <internal/strings.h>
#include <internal/utf8.h>

#include <unordered_map>

namespace tvision
{

constexpr char cp437toUtf8[256][4] =
{
    "\0", "☺", "☻", "♥", "♦", "♣", "♠", "•", "◘", "○", "◙", "♂", "♀", "♪", "♫", "☼",
    "►", "◄", "↕", "‼", "¶", "§", "▬", "↨", "↑", "↓", "→", "←", "∟", "↔", "▲", "▼",
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
    "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
    "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "⌂",
    "Ç", "ü", "é", "â", "ä", "à", "å", "ç", "ê", "ë", "è", "ï", "î", "ì", "Ä", "Å",
    "É", "æ", "Æ", "ô", "ö", "ò", "û", "ù", "ÿ", "Ö", "Ü", "¢", "£", "¥", "₧", "ƒ",
    "á", "í", "ó", "ú", "ñ", "Ñ", "ª", "º", "¿", "⌐", "¬", "½", "¼", "¡", "«", "»",
    "░", "▒", "▓", "│", "┤", "╡", "╢", "╖", "╕", "╣", "║", "╗", "╝", "╜", "╛", "┐",
    "└", "┴", "┬", "├", "─", "┼", "╞", "╟", "╚", "╔", "╩", "╦", "╠", "═", "╬", "╧",
    "╨", "╤", "╥", "╙", "╘", "╒", "╓", "╫", "╪", "┘", "┌", "█", "▄", "▌", "▐", "▀",
    "α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε", "∩",
    "≡", "±", "≥", "≤", "⌠", "⌡", "÷", "≈", "°", "∙", "·", "√", "ⁿ", "²", "■", " "
};

const char (*CpTranslator::cpToUtf8)[256][4] = &cp437toUtf8;

static void initUtf8ToCp( std::unordered_map<uint32_t, char> &map,
                          const char (&toUtf8)[256][4] ) noexcept
{
    map.clear();
    for (size_t i = 0; i < 256; ++i)
    {
        const char *ch = toUtf8[i];
        size_t length = 1 + Utf8BytesLeft(ch[0]);
        map.emplace(string_as_int<uint32_t>({ch, length}), char(i));
    }
}

static std::unordered_map<uint32_t, char> &utf8ToCp() noexcept
{
    // Ensure the map only gets created on first use, to avoid issues with the
    // static initialization order, and also prevent it from being destroyed on
    // program exit, since there could still be secondary threads using it.
    static auto &map = [] () -> auto &
    {
        auto &map = *new std::unordered_map<uint32_t, char>;
        initUtf8ToCp(map, cp437toUtf8);
        return map;
    }();

    return map;
}

void CpTranslator::setTranslation(const char (*aTranslation)[256][4]) noexcept
{
    auto &map = utf8ToCp();
    if (aTranslation)
    {
        static char translation[256][4];

        memcpy(translation, aTranslation, sizeof(translation));
        cpToUtf8 = &translation;
        initUtf8ToCp(map, translation);
    }
    else
    {
        cpToUtf8 = &cp437toUtf8;
        initUtf8ToCp(map, cp437toUtf8);
    }
}

char CpTranslator::fromUtf8(TStringView s) noexcept
{
    auto &map = utf8ToCp();

    auto it = map.find(string_as_int<uint32_t>(s));
    if (it != map.end())
        return it->second;
    return 0;
}

} // namespace tvision
