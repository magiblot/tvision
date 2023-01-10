#define Uses_TKeys
#define Uses_TEvent
#include <tvision/tv.h>

#include <internal/codepage.h>
#include <internal/strings.h>
#include <internal/getenv.h>
#include <internal/utf8.h>

namespace tvision
{

static const TStringView cp437toUtf8[256] =
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

static const std::array<uint32_t, 256> cp437toUtf8Int = make_utf8int<256>(cp437toUtf8);

static const TStringView cp850toUtf8[256] =
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
    "É", "æ", "Æ", "ô", "ö", "ò", "û", "ù", "ÿ", "Ö", "Ü", "ø", "£", "Ø", "×", "ƒ",
    "á", "í", "ó", "ú", "ñ", "Ñ", "ª", "º", "¿", "®", "¬", "½", "¼", "¡", "«", "»",
    "░", "▒", "▓", "│", "┤", "Á", "Â", "À", "©", "╣", "║", "╗", "╝", "¢", "¥", "┐",
    "└", "┴", "┬", "├", "─", "┼", "ã", "Ã", "╚", "╔", "╩", "╦", "╠", "═", "╬", "¤",
    "ð", "Ð", "Ê", "Ë", "È", "ı", "Í", "Î", "Ï", "┘", "┌", "█", "▄", "¦", "Ì", "▀",
    "Ó", "ß", "Ô", "Ò", "õ", "Õ", "µ", "þ", "Þ", "Ú", "Û", "Ù", "ý", "Ý", "¯", "´",
    "-", "±", "‗", "¾", "¶", "§", "÷", "¸", "°", "¨", "·", "¹", "³", "²", "■", " "
    // Note that <last row, first column> should be soft hyphen ("\u00AD"), but
    // it is often represented as a regular hyphen.
};

static const std::array<uint32_t, 256> cp850toUtf8Int = make_utf8int<256>(cp850toUtf8);

static std::unordered_map<uint32_t, char> initMap(const TStringView toUtf8[256]) noexcept
{
    std::unordered_map<uint32_t, char> map;
    for (size_t i = 0; i < 256; ++i)
        map.emplace(string_as_int<uint32_t>(toUtf8[i]), char(i));
    return map;
}

CpTranslator::CpTable::CpTable( TStringView cp,
                                const TStringView toUtf8[256],
                                const std::array<uint32_t, 256> &toUtf8Int ) noexcept :
    cp(cp),
    toUtf8Int(toUtf8Int.data()),
    fromUtf8(initMap(toUtf8))
{
}

const CpTranslator::CpTable CpTranslator::tables[] = {
    { "437", cp437toUtf8, cp437toUtf8Int },
    { "850", cp850toUtf8, cp850toUtf8Int }
};

const CpTranslator::CpTable *CpTranslator::activeTable = nullptr;
CpTranslator CpTranslator::instance;

CpTranslator::CpTranslator() noexcept
{
    // Set the active codepage. 437 is the default.
    use(getEnv<TStringView>("TVISION_CODEPAGE", "437"));
}

char CpTranslator::fromUtf8(TStringView s) noexcept
{
    auto it = activeTable->fromUtf8.find(string_as_int<uint32_t>(s));
    if (it != activeTable->fromUtf8.end())
        return it->second;
    return 0;
}

} // namespace
